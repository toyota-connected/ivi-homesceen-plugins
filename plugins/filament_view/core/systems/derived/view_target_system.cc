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

#include "view_target_system.h"
#include <core/scene/view_target.h>

namespace plugin_filament_view {

////////////////////////////////////////////////////////////////////////////////////
void ViewTargetSystem::vInitSystem() {
  vRegisterMessageHandler(
      ECSMessageType::ViewTargetCreateRequest, [this](const ECSMessage& msg) {
        spdlog::debug("ViewTargetCreateRequest");

        const auto state = msg.getData<FlutterDesktopEngineState*>(
            ECSMessageType::ViewTargetCreateRequest);
        const auto top =
            msg.getData<int>(ECSMessageType::ViewTargetCreateRequestTop);
        const auto left =
            msg.getData<int>(ECSMessageType::ViewTargetCreateRequestLeft);
        const auto width =
            msg.getData<uint32_t>(ECSMessageType::ViewTargetCreateRequestWidth);
        const auto heigth = msg.getData<uint32_t>(
            ECSMessageType::ViewTargetCreateRequestHeight);

        const auto nWhich = nSetupViewTargetFromDesktopState(top, left, state);
        vInitializeFilamentInternalsWithViewTargets(nWhich, width, heigth);

        if (m_poCamera != nullptr) {
          vSetCameraFromSerializedData();
        }

        spdlog::debug("ViewTargetCreateRequest Complete");
      });

  vRegisterMessageHandler(
      ECSMessageType::ViewTargetStartRenderingLoops,
      [this](const ECSMessage& /*msg*/) {
        spdlog::debug("ViewTargetStartRenderingLoops");
        vKickOffFrameRenderingLoops();
        spdlog::debug("ViewTargetStartRenderingLoops Complete");
      });

  vRegisterMessageHandler(
      ECSMessageType::ChangeCameraOrbitHomePosition,
      [this](const ECSMessage& msg) {
        spdlog::debug("ChangeCameraOrbitHomePosition");

        const auto values = msg.getData<filament::math::float3>(
            ECSMessageType::ChangeCameraOrbitHomePosition);

        if (m_poCamera != nullptr) {
          m_poCamera->orbitHomePosition_ =
              std::make_unique<filament::math::float3>(values);

          const auto camera =
              m_lstViewTargets[0]->getCameraManager()->poGetPrimaryCamera();
          camera->orbitHomePosition_ =
              std::make_unique<filament::math::float3>(values);
          camera->forceSingleFrameUpdate_ = true;
        }

        spdlog::debug("ChangeCameraOrbitHomePosition Complete");
      });

  vRegisterMessageHandler(
      ECSMessageType::ChangeCameraTargetPosition,
      [this](const ECSMessage& msg) {
        spdlog::debug("ChangeCameraTargetPosition");

        const auto values = msg.getData<filament::math::float3>(
            ECSMessageType::ChangeCameraTargetPosition);

        if (m_poCamera != nullptr) {
          m_poCamera->targetPosition_ =
              std::make_unique<filament::math::float3>(values);

          const auto camera =
              m_lstViewTargets[0]->getCameraManager()->poGetPrimaryCamera();
          camera->targetPosition_ =
              std::make_unique<filament::math::float3>(values);
          camera->forceSingleFrameUpdate_ = true;
        }

        spdlog::debug("ChangeCameraTargetPosition Complete");
      });

  vRegisterMessageHandler(
      ECSMessageType::ChangeCameraFlightStartPosition,
      [this](const ECSMessage& msg) {
        spdlog::debug("ChangeCameraFlightStartPosition");

        const auto values = msg.getData<filament::math::float3>(
            ECSMessageType::ChangeCameraFlightStartPosition);

        if (m_poCamera != nullptr) {
          m_poCamera->flightStartPosition_ =
              std::make_unique<filament::math::float3>(values);

          const auto camera =
              m_lstViewTargets[0]->getCameraManager()->poGetPrimaryCamera();
          camera->flightStartPosition_ =
              std::make_unique<filament::math::float3>(values);
          camera->forceSingleFrameUpdate_ = true;
        }

        spdlog::debug("ChangeCameraFlightStartPosition Complete");
      });

  vRegisterMessageHandler(
      ECSMessageType::SetCameraFromDeserializedLoad,
      [this](const ECSMessage& msg) {
        spdlog::debug("SetCameraFromDeserializedLoad");
        m_poCamera =
            msg.getData<Camera*>(ECSMessageType::SetCameraFromDeserializedLoad)
                ->clone();
        spdlog::debug("SetCameraFromDeserializedLoad Complete");

        vSetCameraFromSerializedData();
      });

  vRegisterMessageHandler(
      ECSMessageType::ChangeViewQualitySettings, [this](const ECSMessage& msg) {
        spdlog::debug("ChangeViewQualitySettings");

        // Not Currently Implemented -- currently will change all view targes.
        // ChangeViewQualitySettingsWhichView
        auto settings =
            msg.getData<int>(ECSMessageType::ChangeViewQualitySettings);
        for (size_t i = 0; i < m_lstViewTargets.size(); ++i) {
          vChangeViewQualitySettings(
              i, static_cast<ViewTarget::ePredefinedQualitySettings>(settings));
        }

        spdlog::debug("ChangeViewQualitySettings Complete");

        vSetCameraFromSerializedData();
      });

  vRegisterMessageHandler(
      ECSMessageType::ResizeWindow, [this](const ECSMessage& msg) {
        spdlog::debug("ResizeWindow");
        const auto nWhich = msg.getData<size_t>(ECSMessageType::ResizeWindow);
        const auto fWidth =
            msg.getData<double>(ECSMessageType::ResizeWindowWidth);
        const auto fHeight =
            msg.getData<double>(ECSMessageType::ResizeWindowHeight);

        vResizeViewTarget(nWhich, fWidth, fHeight);

        spdlog::debug("ResizeWindow Complete");

        vSetCameraFromSerializedData();
      });

  vRegisterMessageHandler(
      ECSMessageType::MoveWindow, [this](const ECSMessage& msg) {
        spdlog::debug("MoveWindow");
        const auto nWhich = msg.getData<size_t>(ECSMessageType::ResizeWindow);
        const auto fLeft = msg.getData<double>(ECSMessageType::MoveWindowLeft);
        const auto fTop = msg.getData<double>(ECSMessageType::MoveWindowTop);

        vSetViewTargetOffSet(nWhich, fLeft, fTop);

        spdlog::debug("MoveWindow Complete");

        vSetCameraFromSerializedData();
      });
}

////////////////////////////////////////////////////////////////////////////////////
void ViewTargetSystem::vUpdate(float /*fElapsedTime*/) {}

////////////////////////////////////////////////////////////////////////////////////
void ViewTargetSystem::vShutdownSystem() {
  m_poCamera.reset();
}

////////////////////////////////////////////////////////////////////////////////////
void ViewTargetSystem::DebugPrint() {}

////////////////////////////////////////////////////////////////////////////////////
filament::View* ViewTargetSystem::getFilamentView(const size_t nWhich) const {
  if (nWhich >= m_lstViewTargets.size())
    return nullptr;

  return m_lstViewTargets[nWhich]->getFilamentView();
}

////////////////////////////////////////////////////////////////////////////////////
void ViewTargetSystem::vInitializeFilamentInternalsWithViewTargets(
    const size_t nWhich,
    const uint32_t width,
    const uint32_t height) const {
  m_lstViewTargets[nWhich]->InitializeFilamentInternals(width, height);
}

////////////////////////////////////////////////////////////////////////////////////
void ViewTargetSystem::vKickOffFrameRenderingLoops() const {
  for (const auto& viewTarget : m_lstViewTargets) {
    viewTarget->setInitialized();
  }
}

////////////////////////////////////////////////////////////////////////////////////
void ViewTargetSystem::vChangeViewQualitySettings(
    const size_t nWhich,
    const ViewTarget::ePredefinedQualitySettings settings) const {
  m_lstViewTargets[nWhich]->vChangeQualitySettings(settings);
}

////////////////////////////////////////////////////////////////////////////////////
void ViewTargetSystem::vSetCameraFromSerializedData() const {
  for (const auto& viewTarget : m_lstViewTargets) {
    // we might get request to add new view targets as they come online
    // make sure we're not resetting older ones.
    if (viewTarget->getCameraManager()->poGetPrimaryCamera() != nullptr)
      continue;

    std::unique_ptr<Camera> clonedCamera = m_poCamera->clone();

    viewTarget->vSetupCameraManagerWithDeserializedCamera(
        std::move(clonedCamera));
  }
}

////////////////////////////////////////////////////////////////////////////////////
size_t ViewTargetSystem::nSetupViewTargetFromDesktopState(
    int32_t top,
    int32_t left,
    FlutterDesktopEngineState* state) {
  m_lstViewTargets.emplace_back(std::make_unique<ViewTarget>(top, left, state));
  return m_lstViewTargets.size() - 1;
}

////////////////////////////////////////////////////////////////////////////////////
void ViewTargetSystem::vResizeViewTarget(const size_t nWhich,
                                         const double width,
                                         const double height) const {
  m_lstViewTargets[nWhich]->resize(width, height);
}

////////////////////////////////////////////////////////////////////////////////////
void ViewTargetSystem::vSetViewTargetOffSet(const size_t nWhich,
                                            const double left,
                                            const double top) const {
  m_lstViewTargets[nWhich]->setOffset(left, top);
}

////////////////////////////////////////////////////////////////////////////////////
void ViewTargetSystem::vOnTouch(const size_t nWhich,
                                const int32_t action,
                                const int32_t point_count,
                                const size_t point_data_size,
                                const double* point_data) const {
  m_lstViewTargets[nWhich]->vOnTouch(action, point_count, point_data_size,
                                     point_data);
}

////////////////////////////////////////////////////////////////////////////////////
void ViewTargetSystem::vChangePrimaryCameraMode(
    const size_t nWhich,
    const std::string& szValue) const {
  m_lstViewTargets[nWhich]->getCameraManager()->ChangePrimaryCameraMode(
      szValue);
}

////////////////////////////////////////////////////////////////////////////////////
void ViewTargetSystem::vResetInertiaCameraToDefaultValues(
    const size_t nWhich) const {
  m_lstViewTargets[nWhich]
      ->getCameraManager()
      ->vResetInertiaCameraToDefaultValues();
}

////////////////////////////////////////////////////////////////////////////////////
void ViewTargetSystem::vSetCurrentCameraOrbitAngle(const size_t nWhich,
                                                   const float fValue) const {
  const auto camera =
      m_lstViewTargets[nWhich]->getCameraManager()->poGetPrimaryCamera();
  camera->vSetCurrentCameraOrbitAngle(fValue);
}

}  // namespace plugin_filament_view