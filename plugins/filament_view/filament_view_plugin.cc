/*
 * Copyright 2020-2024 Toyota Connected North America
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "filament_view_plugin.h"

#include <core/scene/serialization/scene_text_deserializer.h>
#include <core/systems/derived/animation_system.h>
#include <core/systems/derived/collision_system.h>
#include <core/systems/derived/debug_lines_system.h>
#include <core/systems/derived/entityobject_locator_system.h>
#include <core/systems/derived/filament_system.h>
#include <core/systems/derived/indirect_light_system.h>
#include <core/systems/derived/light_system.h>
#include <core/systems/derived/model_system.h>
#include <core/systems/derived/shape_system.h>
#include <core/systems/derived/skybox_system.h>
#include <core/systems/derived/view_target_system.h>
#include <core/systems/ecsystems_manager.h>
#include <event_sink.h>
#include <event_stream_handler_functions.h>
#include <messages.g.h>
#include <plugins/common/common.h>
#include <asio/post.hpp>

class FlutterView;

class Display;

namespace plugin_filament_view {

std::unique_ptr<SceneTextDeserializer> sceneTextDeserializer;
SceneTextDeserializer* postSetupDeserializer = nullptr;
bool m_bHasSetupRegistrar = false;

//////////////////////////////////////////////////////////////////////////////////////////
void RunOnceCheckAndInitializeECSystems() {
  const auto ecsManager = ECSystemManager::GetInstance();

  if (ecsManager->getRunState() != ECSystemManager::RunState::NotInitialized) {
    return;
  }

  // Get the strand from the ECSystemManager
  const auto& strand = *ecsManager->GetStrand();

  std::promise<void> initPromise;
  const std::future<void> initFuture = initPromise.get_future();

  // Post the initialization code to the strand
  post(strand, [=, &initPromise]() mutable {
    // Add systems to the ECSystemManager
    ecsManager->vAddSystem(std::move(std::make_unique<FilamentSystem>()));
    ecsManager->vAddSystem(std::move(std::make_unique<DebugLinesSystem>()));
    ecsManager->vAddSystem(std::move(std::make_unique<CollisionSystem>()));
    ecsManager->vAddSystem(std::move(std::make_unique<ModelSystem>()));
    ecsManager->vAddSystem(std::move(std::make_unique<MaterialSystem>()));
    ecsManager->vAddSystem(std::move(std::make_unique<ShapeSystem>()));
    ecsManager->vAddSystem(std::move(std::make_unique<IndirectLightSystem>()));
    ecsManager->vAddSystem(std::move(std::make_unique<SkyboxSystem>()));
    ecsManager->vAddSystem(std::move(std::make_unique<LightSystem>()));
    ecsManager->vAddSystem(std::move(std::make_unique<ViewTargetSystem>()));
    ecsManager->vAddSystem(std::move(std::make_unique<AnimationSystem>()));
    // Internal debate whether we auto subscribe to systems on entity creation
    // or not.
    ecsManager->vAddSystem(
        std::move(std::make_unique<EntityObjectLocatorSystem>()));

    ecsManager->vInitSystems();

    initPromise.set_value();
  });

  initFuture.wait();
}

//////////////////////////////////////////////////////////////////////////////////////////
void KickOffRenderingLoops() {
  ECSMessage viewTargetStartRendering;
  viewTargetStartRendering.addData(
      ECSMessageType::ViewTargetStartRenderingLoops, true);
  ECSystemManager::GetInstance()->vRouteMessage(viewTargetStartRendering);
}

//////////////////////////////////////////////////////////////////////////////////////////
void DeserializeDataAndSetupMessageChannels(
    flutter::PluginRegistrar* registrar,
    const std::vector<uint8_t>& params) {
  const auto ecsManager = ECSystemManager::GetInstance();

  // Get the strand from the ECSystemManager
  const auto& strand = *ecsManager->GetStrand();

  std::promise<void> initPromise;
  const std::future<void> initFuture = initPromise.get_future();

  // Safeguarded to only be called once no matter how many times this method is
  // called.
  if (postSetupDeserializer == nullptr) {
    post(strand, [=, &initPromise]() mutable {
      sceneTextDeserializer = std::make_unique<SceneTextDeserializer>(params);
      postSetupDeserializer = sceneTextDeserializer.get();

      // making sure this is only called once!
      postSetupDeserializer->vRunPostSetupLoad();

      initPromise.set_value();
    });

    initFuture.wait();
  }

  const auto animationSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<AnimationSystem>(
          AnimationSystem::StaticGetTypeID(), __FUNCTION__);

  const auto viewTargetSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<ViewTargetSystem>(
          ViewTargetSystem::StaticGetTypeID(), __FUNCTION__);

  const auto collisionSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<CollisionSystem>(
          CollisionSystem::StaticGetTypeID(), __FUNCTION__);

  collisionSystem->vSetupMessageChannels(registrar,
                                         "plugin.filament_view.collision_info");
  viewTargetSystem->vSetupMessageChannels(registrar,
                                          "plugin.filament_view.frame_view");
  animationSystem->vSetupMessageChannels(registrar,
                                         "plugin.filament_view.animation_info");
}

//////////////////////////////////////////////////////////////////////////////////////////
void FilamentViewPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrar* registrar,
    int32_t id,
    std::string viewType,
    int32_t direction,
    double top,
    double left,
    double width,
    double height,
    const std::vector<uint8_t>& params,
    const std::string& assetDirectory,
    FlutterDesktopEngineRef engine,
    PlatformViewAddListener addListener,
    PlatformViewRemoveListener removeListener,
    void* platform_view_context) {
  pthread_setname_np(pthread_self(), "HomeScreenFilamentViewPlugin");

  const auto ecsManager = ECSystemManager::GetInstance();
  ecsManager->setConfigValue(kAssetPath, assetDirectory);

  /*bool bDebugAttached = false;
  int i = 0;
  while (!bDebugAttached) {
    int breakhere = 0;
    if (i++ == 10000000000) {
      bDebugAttached = true;
    }
  }*/

  // Safeguarded inside
  RunOnceCheckAndInitializeECSystems();

  // Every time this method is called, we should create a new view target
  ECSMessage viewTargetCreationRequest;
  viewTargetCreationRequest.addData(ECSMessageType::ViewTargetCreateRequest,
                                    engine);
  viewTargetCreationRequest.addData(ECSMessageType::ViewTargetCreateRequestTop,
                                    static_cast<int>(top));
  viewTargetCreationRequest.addData(ECSMessageType::ViewTargetCreateRequestLeft,
                                    static_cast<int>(left));
  viewTargetCreationRequest.addData(
      ECSMessageType::ViewTargetCreateRequestWidth,
      static_cast<uint32_t>(width));
  viewTargetCreationRequest.addData(
      ECSMessageType::ViewTargetCreateRequestHeight,
      static_cast<uint32_t>(height));
  ECSystemManager::GetInstance()->vRouteMessage(viewTargetCreationRequest);

  // Safeguarded to only be called once internal
  DeserializeDataAndSetupMessageChannels(registrar, params);

  if (!m_bHasSetupRegistrar) {
    m_bHasSetupRegistrar = true;

    //
    auto plugin = std::make_unique<FilamentViewPlugin>(
        id, std::move(viewType), direction, top, left, width, height, params,
        assetDirectory, addListener, removeListener, platform_view_context);

    // Set up message channels and APIs
    SetUp(registrar->messenger(), plugin.get());

    registrar->AddPlugin(std::move(plugin));

    setupMessageChannels(registrar);
  }

  // Ok to be called infinite times.
  KickOffRenderingLoops();

  SPDLOG_TRACE("Initialization completed");
}

//////////////////////////////////////////////////////////////////////////////////////////
FilamentViewPlugin::FilamentViewPlugin(
    const int32_t id,
    std::string viewType,
    const int32_t direction,
    const double top,
    const double left,
    const double width,
    const double height,
    const std::vector<uint8_t>& /*params*/,
    const std::string& /*assetDirectory*/,
    const PlatformViewAddListener addListener,
    const PlatformViewRemoveListener removeListener,
    void* platform_view_context)
    : PlatformView(id,
                   std::move(viewType),
                   direction,
                   top,
                   left,
                   width,
                   height),
      id_(id),
      platformViewsContext_(platform_view_context),
      removeListener_(removeListener) {
  SPDLOG_TRACE("++FilamentViewPlugin::FilamentViewPlugin");

  addListener(platformViewsContext_, id, &platform_view_listener_, this);

  SPDLOG_TRACE("--FilamentViewPlugin::FilamentViewPlugin");
}

//////////////////////////////////////////////////////////////////////////////////////////
FilamentViewPlugin::~FilamentViewPlugin() {
  removeListener_(platformViewsContext_, id_);

  ECSystemManager::GetInstance()->vShutdownSystems();
  ECSystemManager::GetInstance()->vRemoveAllSystems();
  // wait for thread to stop running. (Should be relatively quick)
  while (ECSystemManager::GetInstance()->bIsCompletedStopping() == false) {
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<flutter::EventSink<>> eventSink_;
void FilamentViewPlugin::setupMessageChannels(
    flutter::PluginRegistrar* registrar) {
  // Setup MethodChannel for readiness check
  const std::string readinessMethodChannel =
      "plugin.filament_view.readiness_checker";

  const auto methodChannel = std::make_unique<flutter::MethodChannel<>>(
      registrar->messenger(), readinessMethodChannel,
      &flutter::StandardMethodCodec::GetInstance());

  methodChannel->SetMethodCallHandler(
      [&](const flutter::MethodCall<>& call,
          const std::unique_ptr<flutter::MethodResult<>>& result) {
        if (call.method_name() == "isReady") {
          // Check readiness and respond
          bool isReady = true;  // Replace with your actual readiness check
          result->Success(flutter::EncodableValue(isReady));
        } else {
          result->NotImplemented();
        }
      });

  // Setup EventChannel for readiness events
  const std::string readinessEventChannel = "plugin.filament_view.readiness";

  const auto eventChannel = std::make_unique<flutter::EventChannel<>>(
      registrar->messenger(), readinessEventChannel,
      &flutter::StandardMethodCodec::GetInstance());

  eventChannel->SetStreamHandler(
      std::make_unique<flutter::StreamHandlerFunctions<>>(
          [&](const flutter::EncodableValue* /* arguments */,
              std::unique_ptr<flutter::EventSink<>>&& events)
              -> std::unique_ptr<flutter::StreamHandlerError<>> {
            eventSink_ = std::move(events);
            sendReadyEvent();  // Proactively send "ready" event
            return nullptr;
          },
          [&](const flutter::EncodableValue* /* arguments */)
              -> std::unique_ptr<flutter::StreamHandlerError<>> {
            eventSink_ = nullptr;
            return nullptr;
          }));
}

//////////////////////////////////////////////////////////////////////////////////////////
void FilamentViewPlugin::sendReadyEvent() {
  if (eventSink_) {
    eventSink_->Success(flutter::EncodableValue("ready"));
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ChangeMaterialParameter(
    const flutter::EncodableMap& params,
    const std::string& guid) {
  ECSMessage materialData;
  materialData.addData(ECSMessageType::ChangeMaterialParameter, params);
  materialData.addData(ECSMessageType::EntityToTarget, guid);
  ECSystemManager::GetInstance()->vRouteMessage(materialData);
  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ChangeMaterialDefinition(
    const flutter::EncodableMap& params,
    const std::string& guid) {
  ECSMessage materialData;
  materialData.addData(ECSMessageType::ChangeMaterialDefinitions, params);
  materialData.addData(ECSMessageType::EntityToTarget, guid);
  ECSystemManager::GetInstance()->vRouteMessage(materialData);
  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ToggleShapesInScene(
    const bool value) {
  ECSMessage toggleMessage;
  toggleMessage.addData(ECSMessageType::ToggleShapesInScene, value);
  ECSystemManager::GetInstance()->vRouteMessage(toggleMessage);
  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError>
FilamentViewPlugin::ToggleDebugCollidableViewsInScene(const bool value) {
  ECSMessage toggleMessage;
  toggleMessage.addData(ECSMessageType::ToggleDebugCollidableViewsInScene,
                        value);
  ECSystemManager::GetInstance()->vRouteMessage(toggleMessage);
  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ChangeCameraMode(
    const std::string& mode) {
  const auto viewTargetSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<ViewTargetSystem>(
          ViewTargetSystem::StaticGetTypeID(), __FUNCTION__);

  viewTargetSystem->vChangePrimaryCameraMode(0, mode);
  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ChangeCameraOrbitHomePosition(
    double x,
    double y,
    double z) {
  filament::math::float3 position(static_cast<float>(x), static_cast<float>(y),
                                  static_cast<float>(z));

  ECSMessage data;
  data.addData(ECSMessageType::ChangeCameraOrbitHomePosition, position);
  ECSystemManager::GetInstance()->vRouteMessage(data);
  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError>
FilamentViewPlugin::ChangeCameraTargetPosition(double x, double y, double z) {
  filament::math::float3 position(static_cast<float>(x), static_cast<float>(y),
                                  static_cast<float>(z));
  ECSMessage data;
  data.addData(ECSMessageType::ChangeCameraTargetPosition, position);
  ECSystemManager::GetInstance()->vRouteMessage(data);
  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ChangeCameraFlightStartPosition(
    double x,
    double y,
    double z) {
  filament::math::float3 position(static_cast<float>(x), static_cast<float>(y),
                                  static_cast<float>(z));
  ECSMessage data;
  data.addData(ECSMessageType::ChangeCameraFlightStartPosition, position);
  ECSystemManager::GetInstance()->vRouteMessage(data);
  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError>
FilamentViewPlugin::ResetInertiaCameraToDefaultValues() {
  const auto viewTargetSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<ViewTargetSystem>(
          ViewTargetSystem::StaticGetTypeID(), __FUNCTION__);

  viewTargetSystem->vResetInertiaCameraToDefaultValues(0);
  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ChangeViewQualitySettings() {
  static int qualitySettingsVal = 0;
  ECSMessage qualitySettings;
  qualitySettings.addData(ECSMessageType::ChangeViewQualitySettings,
                          qualitySettingsVal);
  ECSystemManager::GetInstance()->vRouteMessage(qualitySettings);

  qualitySettingsVal++;
  if (qualitySettingsVal > ViewTarget::ePredefinedQualitySettings::Ultra) {
    qualitySettingsVal = 0;
  }
  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::SetCameraRotation(
    const double value) {
  const auto viewTargetSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<ViewTargetSystem>(
          ViewTargetSystem::StaticGetTypeID(), __FUNCTION__);

  viewTargetSystem->vSetCurrentCameraOrbitAngle(0, static_cast<float>(value));
  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ChangeLightTransformByGUID(
    const std::string& guid,
    double posx,
    double posy,
    double posz,
    double dirx,
    double diry,
    double dirz) {
  filament::math::float3 position(static_cast<float>(posx),
                                  static_cast<float>(posy),
                                  static_cast<float>(posz));
  filament::math::float3 direction(static_cast<float>(dirx),
                                   static_cast<float>(diry),
                                   static_cast<float>(dirz));

  ECSMessage lightData;
  lightData.addData(ECSMessageType::ChangeSceneLightTransform, guid);
  lightData.addData(ECSMessageType::Position, position);
  lightData.addData(ECSMessageType::Direction, direction);
  ECSystemManager::GetInstance()->vRouteMessage(lightData);

  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ChangeLightColorByGUID(
    const std::string& guid,
    const std::string& color,
    const int64_t intensity) {
  ECSMessage lightData;
  lightData.addData(ECSMessageType::ChangeSceneLightProperties, guid);
  lightData.addData(ECSMessageType::ChangeSceneLightPropertiesColorValue,
                    color);
  lightData.addData(ECSMessageType::ChangeSceneLightPropertiesIntensity,
                    static_cast<float>(intensity));
  ECSystemManager::GetInstance()->vRouteMessage(lightData);

  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::EnqueueAnimation(
    const std::string& guid,
    const int64_t animation_index) {
  ECSMessage enqueueMessage;
  enqueueMessage.addData(ECSMessageType::AnimationEnqueue,
                         static_cast<int32_t>(animation_index));
  enqueueMessage.addData(ECSMessageType::EntityToTarget, guid);
  ECSystemManager::GetInstance()->vRouteMessage(enqueueMessage);

  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ClearAnimationQueue(
    const std::string& guid) {
  ECSMessage clearQueueMessage;
  clearQueueMessage.addData(ECSMessageType::AnimationClearQueue, guid);
  clearQueueMessage.addData(ECSMessageType::EntityToTarget, guid);
  ECSystemManager::GetInstance()->vRouteMessage(clearQueueMessage);

  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::PlayAnimation(
    const std::string& guid,
    const int64_t animation_index) {
  ECSMessage playMessage;
  playMessage.addData(ECSMessageType::AnimationPlay,
                      static_cast<int32_t>(animation_index));
  playMessage.addData(ECSMessageType::EntityToTarget, guid);
  ECSystemManager::GetInstance()->vRouteMessage(playMessage);

  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ChangeAnimationSpeed(
    const std::string& guid,
    const double speed) {
  ECSMessage changeSpeedMessage;
  changeSpeedMessage.addData(ECSMessageType::AnimationChangeSpeed,
                             static_cast<float>(speed));
  changeSpeedMessage.addData(ECSMessageType::EntityToTarget, guid);
  ECSystemManager::GetInstance()->vRouteMessage(changeSpeedMessage);

  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::PauseAnimation(
    const std::string& guid) {
  ECSMessage pauseMessage;
  pauseMessage.addData(ECSMessageType::AnimationPause, guid);
  pauseMessage.addData(ECSMessageType::EntityToTarget, guid);
  ECSystemManager::GetInstance()->vRouteMessage(pauseMessage);

  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ResumeAnimation(
    const std::string& guid) {
  ECSMessage resumeMessage;
  resumeMessage.addData(ECSMessageType::AnimationResume, guid);
  resumeMessage.addData(ECSMessageType::EntityToTarget, guid);
  ECSystemManager::GetInstance()->vRouteMessage(resumeMessage);

  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::SetAnimationLooping(
    const std::string& guid,
    const bool looping) {
  ECSMessage setLoopingMessage;
  setLoopingMessage.addData(ECSMessageType::AnimationSetLooping, looping);
  setLoopingMessage.addData(ECSMessageType::EntityToTarget, guid);
  ECSystemManager::GetInstance()->vRouteMessage(setLoopingMessage);

  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::RequestCollisionCheckFromRay(
    const std::string& query_id,
    double origin_x,
    double origin_y,
    double origin_z,
    double direction_x,
    double direction_y,
    double direction_z,
    double length) {
  filament::math::float3 origin(static_cast<float>(origin_x),
                                static_cast<float>(origin_y),
                                static_cast<float>(origin_z));
  filament::math::float3 direction(static_cast<float>(direction_x),
                                   static_cast<float>(direction_y),
                                   static_cast<float>(direction_z));

  Ray rayInfo(origin, direction, static_cast<float>(length));

  // Debug line message
  ECSMessage rayInformation;
  rayInformation.addData(ECSMessageType::DebugLine, rayInfo);
  ECSystemManager::GetInstance()->vRouteMessage(rayInformation);

  // Collision request message
  ECSMessage collisionRequest;
  collisionRequest.addData(ECSMessageType::CollisionRequest, rayInfo);
  collisionRequest.addData(ECSMessageType::CollisionRequestRequestor, query_id);
  collisionRequest.addData(ECSMessageType::CollisionRequestType,
                           eFromNonNative);
  ECSystemManager::GetInstance()->vRouteMessage(collisionRequest);

  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ChangeScaleByGUID(
    const std::string& guid,
    const double x,
    const double y,
    const double z) {
  const filament::math::float3 values(
      static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

  ECSMessage changeRequest;
  changeRequest.addData(ECSMessageType::ChangeScaleByGUID, guid);
  changeRequest.addData(ECSMessageType::floatVec3, values);
  ECSystemManager::GetInstance()->vRouteMessage(changeRequest);

  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ChangeTranslationByGUID(
    const std::string& guid,
    const double x,
    const double y,
    const double z) {
  const filament::math::float3 values(
      static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

  ECSMessage changeRequest;
  changeRequest.addData(ECSMessageType::ChangeTranslationByGUID, guid);
  changeRequest.addData(ECSMessageType::floatVec3, values);
  ECSystemManager::GetInstance()->vRouteMessage(changeRequest);

  return std::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////
std::optional<FlutterError> FilamentViewPlugin::ChangeRotationByGUID(
    const std::string& guid,
    const double x,
    const double y,
    const double z,
    const double w) {
  const filament::math::float4 values(
      static_cast<float>(x), static_cast<float>(y), static_cast<float>(z),
      static_cast<float>(w));

  ECSMessage changeRequest;
  changeRequest.addData(ECSMessageType::ChangeRotationByGUID, guid);
  changeRequest.addData(ECSMessageType::floatVec4, values);
  ECSystemManager::GetInstance()->vRouteMessage(changeRequest);

  return std::nullopt;
}

// TODO this function will need to change to say 'which' view is being changed.
void FilamentViewPlugin::on_resize(const double width,
                                   const double height,
                                   void* /*data*/) {
  if (width <= 0 || height <= 0) {
    return;
  }

  ECSMessage resizeMessage;
  resizeMessage.addData(ECSMessageType::ResizeWindow, static_cast<size_t>(0));
  resizeMessage.addData(ECSMessageType::ResizeWindowWidth, width);
  resizeMessage.addData(ECSMessageType::ResizeWindowHeight, height);
  ECSystemManager::GetInstance()->vRouteMessage(resizeMessage);
}

void FilamentViewPlugin::on_set_direction(const int32_t direction, void* data) {
  const auto plugin = static_cast<FilamentViewPlugin*>(data);
  if (plugin) {
    plugin->direction_ = direction;
  }
  SPDLOG_TRACE("SetDirection: {}", plugin->direction_);
}

// TODO this function will need to change to say 'which' view is being changed.
void FilamentViewPlugin::on_set_offset(const double left,
                                       const double top,
                                       void* /*data*/) {
  ECSMessage moveMessage;
  moveMessage.addData(ECSMessageType::MoveWindow, static_cast<size_t>(0));
  moveMessage.addData(ECSMessageType::MoveWindowLeft, left);
  moveMessage.addData(ECSMessageType::MoveWindowTop, top);
  ECSystemManager::GetInstance()->vRouteMessage(moveMessage);
}

// TODO this function will need to change to say 'which' view is being changed.
void FilamentViewPlugin::on_touch(const int32_t action,
                                  const int32_t point_count,
                                  const size_t point_data_size,
                                  const double* point_data,
                                  void* data) {
  if (const auto plugin = static_cast<FilamentViewPlugin*>(data); plugin) {
    const auto viewTargetSystem =
        ECSystemManager::GetInstance()->poGetSystemAs<ViewTargetSystem>(
            ViewTargetSystem::StaticGetTypeID(),
            "FilamentViewPlugin::on_touch");

    // has to be changed to 'which' on touch was hit
    viewTargetSystem->vOnTouch(0, action, point_count, point_data_size,
                               point_data);
  }
}

void FilamentViewPlugin::on_dispose(bool /* hybrid */, void* data) {
  if (const auto plugin = static_cast<FilamentViewPlugin*>(data); plugin) {
    // Todo ? Note? Should we destroy all systems here?
  }
}

const platform_view_listener FilamentViewPlugin::platform_view_listener_ = {
    .resize = on_resize,
    .set_direction = on_set_direction,
    .set_offset = on_set_offset,
    .on_touch = on_touch,
    .dispose = on_dispose};

}  // namespace plugin_filament_view