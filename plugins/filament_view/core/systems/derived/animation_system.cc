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
#include "animation_system.h"

#include <core/entity/base/entityobject.h>
#include <core/include/literals.h>
#include <core/systems/ecsystems_manager.h>
#include <method_channel.h>
#include <plugin_registrar.h>
#include <plugins/common/common.h>
#include <standard_method_codec.h>

namespace plugin_filament_view {

////////////////////////////////////////////////////////////////////////////////////
void AnimationSystem::vInitSystem() {
  vRegisterMessageHandler(
      ECSMessageType::SetupMessageChannels, [this](const ECSMessage& msg) {
        spdlog::debug("SetupMessageChannels");

        const auto registrar = msg.getData<flutter::PluginRegistrar*>(
            ECSMessageType::SetupMessageChannels);

        setupMessageChannels(registrar);

        spdlog::debug("SetupMessageChannels Complete");
      });

  // Handler for AnimationEnqueue
  vRegisterMessageHandler(
      ECSMessageType::AnimationEnqueue, [this](const ECSMessage& msg) {
        spdlog::debug("AnimationEnqueue");

        const EntityGUID& guid =
            msg.getData<EntityGUID>(ECSMessageType::EntityToTarget);
        auto animationIndex =
            msg.getData<int32_t>(ECSMessageType::AnimationEnqueue);

        auto it = _entities.find(guid);
        if (it != _entities.end()) {
          auto animationComponent = dynamic_cast<Animation*>(
              it->second
                  ->GetComponentByStaticTypeID(Animation::StaticGetTypeID())
                  .get());
          if (animationComponent) {
            animationComponent->vEnqueueAnimation(animationIndex);
            spdlog::debug("AnimationEnqueue Complete for GUID: {}", guid);
          }
        }
      });

  // Handler for AnimationClearQueue
  vRegisterMessageHandler(
      ECSMessageType::AnimationClearQueue, [this](const ECSMessage& msg) {
        spdlog::debug("AnimationClearQueue");

        const EntityGUID& guid =
            msg.getData<EntityGUID>(ECSMessageType::EntityToTarget);

        auto it = _entities.find(guid);
        if (it != _entities.end()) {
          auto animationComponent = dynamic_cast<Animation*>(
              it->second
                  ->GetComponentByStaticTypeID(Animation::StaticGetTypeID())
                  .get());
          if (animationComponent) {
            animationComponent->vClearQueue();
            spdlog::debug("AnimationClearQueue Complete for GUID: {}", guid);
          }
        }
      });

  // Handler for AnimationPlay
  vRegisterMessageHandler(
      ECSMessageType::AnimationPlay, [this](const ECSMessage& msg) {
        spdlog::debug("AnimationPlay");

        const EntityGUID& guid =
            msg.getData<EntityGUID>(ECSMessageType::EntityToTarget);
        auto animationIndex =
            msg.getData<int32_t>(ECSMessageType::AnimationPlay);

        auto it = _entities.find(guid);
        if (it != _entities.end()) {
          auto animationComponent = dynamic_cast<Animation*>(
              it->second
                  ->GetComponentByStaticTypeID(Animation::StaticGetTypeID())
                  .get());
          if (animationComponent) {
            animationComponent->vPlayAnimation(animationIndex);
            spdlog::debug("AnimationPlay Complete for GUID: {}", guid);
          }
        }
      });

  // Handler for AnimationChangeSpeed
  vRegisterMessageHandler(
      ECSMessageType::AnimationChangeSpeed, [this](const ECSMessage& msg) {
        spdlog::debug("AnimationChangeSpeed");

        const EntityGUID& guid =
            msg.getData<EntityGUID>(ECSMessageType::EntityToTarget);
        auto newSpeed =
            msg.getData<float>(ECSMessageType::AnimationChangeSpeed);

        auto it = _entities.find(guid);
        if (it != _entities.end()) {
          auto animationComponent = dynamic_cast<Animation*>(
              it->second
                  ->GetComponentByStaticTypeID(Animation::StaticGetTypeID())
                  .get());
          if (animationComponent) {
            animationComponent->vSetPlaybackSpeedScalar(newSpeed);
            spdlog::debug("AnimationChangeSpeed Complete for GUID: {}", guid);
          }
        }
      });

  // Handler for AnimationPause
  vRegisterMessageHandler(
      ECSMessageType::AnimationPause, [this](const ECSMessage& msg) {
        spdlog::debug("AnimationPause");

        const EntityGUID& guid =
            msg.getData<EntityGUID>(ECSMessageType::EntityToTarget);

        auto it = _entities.find(guid);
        if (it != _entities.end()) {
          auto animationComponent = dynamic_cast<Animation*>(
              it->second
                  ->GetComponentByStaticTypeID(Animation::StaticGetTypeID())
                  .get());
          if (animationComponent) {
            animationComponent->vPause();
            spdlog::debug("AnimationPause Complete for GUID: {}", guid);
          }
        }
      });

  // Handler for AnimationResume
  vRegisterMessageHandler(
      ECSMessageType::AnimationResume, [this](const ECSMessage& msg) {
        spdlog::debug("AnimationResume");

        const EntityGUID& guid =
            msg.getData<EntityGUID>(ECSMessageType::EntityToTarget);

        auto it = _entities.find(guid);
        if (it != _entities.end()) {
          auto animationComponent = dynamic_cast<Animation*>(
              it->second
                  ->GetComponentByStaticTypeID(Animation::StaticGetTypeID())
                  .get());
          if (animationComponent) {
            animationComponent->vResume();
            spdlog::debug("AnimationResume Complete for GUID: {}", guid);
          }
        }
      });

  // Handler for AnimationSetLooping
  vRegisterMessageHandler(
      ECSMessageType::AnimationSetLooping, [this](const ECSMessage& msg) {
        spdlog::debug("AnimationSetLooping");

        const EntityGUID& guid =
            msg.getData<EntityGUID>(ECSMessageType::EntityToTarget);
        auto shouldLoop =
            msg.getData<bool>(ECSMessageType::AnimationSetLooping);

        auto it = _entities.find(guid);
        if (it != _entities.end()) {
          auto animationComponent = dynamic_cast<Animation*>(
              it->second
                  ->GetComponentByStaticTypeID(Animation::StaticGetTypeID())
                  .get());
          if (animationComponent) {
            animationComponent->vSetLooping(shouldLoop);
            spdlog::debug("AnimationSetLooping Complete for GUID: {}", guid);
          }
        }
      });
}

////////////////////////////////////////////////////////////////////////////////////
void AnimationSystem::setupMessageChannels(
    flutter::PluginRegistrar* plugin_registrar) {
  auto channel_name = std::string("plugin.filament_view.animation_info");

  animationInfoCallback_ = std::make_unique<flutter::MethodChannel<>>(
      plugin_registrar->messenger(), channel_name,
      &flutter::StandardMethodCodec::GetInstance());
}

////////////////////////////////////////////////////////////////////////////////////
void AnimationSystem::vUpdate(float fElapsedTime) {
  for (auto& animatorEntity : _entities) {
    auto animator = dynamic_cast<Animation*>(
        animatorEntity.second
            ->GetComponentByStaticTypeID(Animation::StaticGetTypeID())
            .get());
    animator->vUpdate(fElapsedTime);
  }
}

////////////////////////////////////////////////////////////////////////////////////
void AnimationSystem::vRegisterEntityObject(
    const std::shared_ptr<EntityObject>& entity) {
  if (_entities.find(entity->GetGlobalGuid()) != _entities.end()) {
    spdlog::error("{}::{}: Entity {} already registered", __FILE__,
                  __FUNCTION__, entity->GetGlobalGuid());
    return;
  }

  _entities.insert(std::pair(entity->GetGlobalGuid(), entity));
}

////////////////////////////////////////////////////////////////////////////////////
void AnimationSystem::vUnregisterEntityObject(
    const std::shared_ptr<EntityObject>& entity) {
  _entities.erase(entity->GetGlobalGuid());
}

////////////////////////////////////////////////////////////////////////////////////
void AnimationSystem::vShutdownSystem() {}

////////////////////////////////////////////////////////////////////////////////////
void AnimationSystem::DebugPrint() {
  spdlog::debug("{}::{}", __FILE__, __FUNCTION__);

  // todo list all animators
}

////////////////////////////////////////////////////////////////////////////////////
void AnimationSystem::vNotifyOfAnimationEvent(const EntityGUID& entityGuid,
                                              const AnimationEventType& eType,
                                              const std::string& eventData) {
  if (animationInfoCallback_ == nullptr) {
    return;
  }

  flutter::EncodableMap encodableMap;

  encodableMap[flutter::EncodableValue(kAnimationEventType)] =
      static_cast<int>(eType);

  // source guid
  encodableMap[flutter::EncodableValue(kGlobalGuid)] = entityGuid;

  // event data.
  encodableMap[flutter::EncodableValue(kAnimationEventData)] = eventData;

  animationInfoCallback_->InvokeMethod(
      kAnimationEvent, std::make_unique<flutter::EncodableValue>(
                           flutter::EncodableValue(encodableMap)));
}

}  // namespace plugin_filament_view