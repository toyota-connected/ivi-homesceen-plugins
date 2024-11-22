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
#include <plugin_registrar.h>
#include <plugins/common/common.h>
#include <standard_method_codec.h>

namespace plugin_filament_view {

////////////////////////////////////////////////////////////////////////////////////
void AnimationSystem::vInitSystem() {
  // Handler for AnimationEnqueue
  vRegisterMessageHandler(
      ECSMessageType::AnimationEnqueue, [this](const ECSMessage& msg) {
        spdlog::debug("AnimationEnqueue");

        const EntityGUID& guid =
            msg.getData<EntityGUID>(ECSMessageType::EntityToTarget);
        const auto animationIndex =
            msg.getData<int32_t>(ECSMessageType::AnimationEnqueue);

        if (const auto it = _entities.find(guid); it != _entities.end()) {
          const auto animationComponent = dynamic_cast<Animation*>(
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

        if (const auto it = _entities.find(guid); it != _entities.end()) {
          const auto animationComponent = dynamic_cast<Animation*>(
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
        const auto animationIndex =
            msg.getData<int32_t>(ECSMessageType::AnimationPlay);

        if (const auto it = _entities.find(guid); it != _entities.end()) {
          const auto animationComponent = dynamic_cast<Animation*>(
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
        const auto newSpeed =
            msg.getData<float>(ECSMessageType::AnimationChangeSpeed);

        if (const auto it = _entities.find(guid); it != _entities.end()) {
          const auto animationComponent = dynamic_cast<Animation*>(
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

        if (const auto it = _entities.find(guid); it != _entities.end()) {
          const auto animationComponent = dynamic_cast<Animation*>(
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

        if (const auto it = _entities.find(guid); it != _entities.end()) {
          const auto animationComponent = dynamic_cast<Animation*>(
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
        const auto shouldLoop =
            msg.getData<bool>(ECSMessageType::AnimationSetLooping);

        if (const auto it = _entities.find(guid); it != _entities.end()) {
          const auto animationComponent = dynamic_cast<Animation*>(
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
void AnimationSystem::vUpdate(const float fElapsedTime) {
  for (auto& [fst, snd] : _entities) {
    const auto animator = dynamic_cast<Animation*>(
        snd->GetComponentByStaticTypeID(Animation::StaticGetTypeID()).get());
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
void AnimationSystem::vNotifyOfAnimationEvent(
    const EntityGUID& entityGuid,
    const AnimationEventType& eType,
    const std::string& eventData) const {
  const auto event =
      flutter::EncodableMap({{flutter::EncodableValue("event"),
                              flutter::EncodableValue(kAnimationEvent)},
                             {flutter::EncodableValue(kAnimationEventType),
                              flutter::EncodableValue(static_cast<int>(eType))},
                             {flutter::EncodableValue(kGlobalGuid),
                              flutter::EncodableValue(entityGuid)},
                             {flutter::EncodableValue(kAnimationEventData),
                              flutter::EncodableValue(eventData)}});

    vSendDataToEventChannel(event);
}

}  // namespace plugin_filament_view