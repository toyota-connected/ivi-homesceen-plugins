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
#include "animation.h"

#include <core/include/literals.h>
#include <core/systems/derived/animation_system.h>
#include <core/systems/ecsystems_manager.h>
#include <core/utils/deserialize.h>
#include <plugins/common/common.h>
#include <filesystem>

namespace plugin_filament_view {

////////////////////////////////////////////////////////////////////////////
Animation::Animation(const flutter::EncodableMap& params)
    : Component(std::string(__FUNCTION__)) {
  Deserialize::DecodeParameterWithDefault(kAutoPlay, &m_bAutoPlay, params,
                                          false);
  Deserialize::DecodeParameterWithDefault(kIndex, &m_nCurrentPlayingIndex,
                                          params, 0);

  Deserialize::DecodeParameterWithDefault(kLoop, &m_bLoop, params, true);

  Deserialize::DecodeParameterWithDefault(
      kResetToTPoseOnReset, &m_bResetToTPoseOnReset, params, false);

  double speed;
  Deserialize::DecodeParameterWithDefault(kPlaybackSpeed, &speed, params, 1.0f);

  m_fPlaybackSpeedScalar = static_cast<float>(speed);

  Deserialize::DecodeParameterWithDefault(
      kNotifyOfAnimationEvents, &m_bNotifyOfAnimationEvents, params, false);

  m_bPaused = false;
}

////////////////////////////////////////////////////////////////////////////
void Animation::vUpdate(const float fElapsedTime) {
  if (m_poAnimator == nullptr || m_bPaused) {
    return;
  }

  if (m_nCurrentPlayingIndex < 0 && !m_queAnimationQueue.empty()) {
    // Dequeue the next animation if the current one is finished
    m_nCurrentPlayingIndex = m_queAnimationQueue.front();
    m_queAnimationQueue.pop();
    m_fTimeSinceStart = 0.0f;

    if (m_bNotifyOfAnimationEvents) {
      const auto animationSystem =
          ECSystemManager::GetInstance()->poGetSystemAs<AnimationSystem>(
              AnimationSystem::StaticGetTypeID(), "Animation::vUpdate");
      animationSystem->vNotifyOfAnimationEvent(
          GetOwner()->GetGlobalGuid(), eAnimationStarted,
          std::to_string(m_nCurrentPlayingIndex));
    }
  }

  if (m_nCurrentPlayingIndex < 0) {
    return;
  }

  m_fTimeSinceStart += fElapsedTime * m_fPlaybackSpeedScalar;

  m_poAnimator->applyAnimation(static_cast<size_t>(m_nCurrentPlayingIndex),
                               m_fTimeSinceStart);
  m_poAnimator->updateBoneMatrices();

  const auto currentAnimDuration = m_poAnimator->getAnimationDuration(
      static_cast<size_t>(m_nCurrentPlayingIndex));
  if (m_fTimeSinceStart > currentAnimDuration) {
    if (m_bNotifyOfAnimationEvents) {
      // send message here to dart
      const auto animationSystem =
          ECSystemManager::GetInstance()->poGetSystemAs<AnimationSystem>(
              AnimationSystem::StaticGetTypeID(), "Animation::vUpdate");

      animationSystem->vNotifyOfAnimationEvent(
          GetOwner()->GetGlobalGuid(), eAnimationEnded,
          std::to_string(m_nCurrentPlayingIndex));
    }

    // check queue

    // check loop
    if (m_bLoop) {
      m_fTimeSinceStart -= currentAnimDuration;

      if (m_bNotifyOfAnimationEvents) {
        // send message here to dart
        const auto animationSystem =
            ECSystemManager::GetInstance()->poGetSystemAs<AnimationSystem>(
                AnimationSystem::StaticGetTypeID(), "Animation::vUpdate");

        animationSystem->vNotifyOfAnimationEvent(
            GetOwner()->GetGlobalGuid(), eAnimationStarted,
            std::to_string(m_nCurrentPlayingIndex));
      }

    } else {
      m_fTimeSinceStart = 0.0f;
      m_nCurrentPlayingIndex = -1;

      if (!m_queAnimationQueue.empty()) {
        m_nCurrentPlayingIndex = m_queAnimationQueue.front();
        m_queAnimationQueue.pop();
      } else if (m_bResetToTPoseOnReset) {
        m_poAnimator->resetBoneMatrices();
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////
void Animation::vEnqueueAnimation(const int32_t index) {
  if (index < 0) {
    return;
  }

  if (index >= m_mapAnimationNamesToIndex.size()) {
    spdlog::warn(
        "Attempting to vEnqueueAnimation that is greater than total count of "
        "animations.");
    return;
  }

  m_queAnimationQueue.push(index);
}

////////////////////////////////////////////////////////////////////////////
void Animation::vClearQueue() {
  std::queue<int32_t> emptyQueue;
  std::swap(m_queAnimationQueue, emptyQueue);  // Efficiently clear the queue
}

////////////////////////////////////////////////////////////////////////////
void Animation::vSetAnimator(filament::gltfio::Animator& animator) {
  m_poAnimator = &animator;

  vSetupAnimationNameMapping();

  if (m_bAutoPlay) {
    vPlayAnimation(m_nCurrentPlayingIndex);
  }
}

////////////////////////////////////////////////////////////////////////////
void Animation::vPlayAnimation(int32_t index) {
  if (index < 0 || index >= m_mapAnimationNamesToIndex.size()) {
    spdlog::warn("Invalid animation index: {}", index);
    return;
  }

  vClearQueue();

  m_nCurrentPlayingIndex = index;
  m_fTimeSinceStart = 0.0f;
}

////////////////////////////////////////////////////////////////////////////
bool Animation::bPlayAnimation(const std::string& szName) {
  if (const auto foundIter = m_mapAnimationNamesToIndex.find(szName);
      foundIter != m_mapAnimationNamesToIndex.end()) {
    vPlayAnimation(static_cast<int32_t>(foundIter->second));
    return true;
  }

  spdlog::warn("Animation string not found in mapping table {}", szName);
  return false;
}

////////////////////////////////////////////////////////////////////////////
void Animation::vSetupAnimationNameMapping() {
  if (m_poAnimator) {
    const auto count = m_poAnimator->getAnimationCount();
    for (size_t i = 0; i < count; i++) {
      m_mapAnimationNamesToIndex.insert(
          std::make_pair(std::string(m_poAnimator->getAnimationName(i)), i));
    }
  }
}

////////////////////////////////////////////////////////////////////////
void Animation::DebugPrint(const std::string& tabPrefix) const {
  spdlog::debug("{}m_nCurrentPlayingIndex: {}", tabPrefix,
                m_nCurrentPlayingIndex);
  spdlog::debug("{}m_bPaused: {}", tabPrefix, m_bPaused);
  spdlog::debug("{}m_bAutoPlay: {}", tabPrefix, m_bAutoPlay);
  spdlog::debug("{}m_bLoop: {}", tabPrefix, m_bLoop);
  spdlog::debug("{}m_bResetToTPoseOnReset: {}", tabPrefix,
                m_bResetToTPoseOnReset);
  spdlog::debug("{}m_fPlaybackSpeedScalar: {}", tabPrefix,
                m_fPlaybackSpeedScalar);
  spdlog::debug("{}m_bNotifyOfAnimationEvents: {}", tabPrefix,
                m_bNotifyOfAnimationEvents);
  spdlog::debug("{}m_fTimeSinceStart: {}", tabPrefix, m_fTimeSinceStart);

  if (m_poAnimator) {
    const auto count = m_poAnimator->getAnimationCount();
    spdlog::debug("{}Animation Info: count[{}]", tabPrefix, count);
    for (size_t i = 0; i < count; i++) {
      spdlog::debug("{}Anim at [{}]: Name: '{}', Duration: {}", tabPrefix, i,
                    m_poAnimator->getAnimationName(i),
                    m_poAnimator->getAnimationDuration(i));
    }
  } else {
    spdlog::debug("{}m_poAnimator is nullptr", tabPrefix);
  }

  spdlog::debug("{}m_mapAnimationNamesToIndex:", tabPrefix);
  for (const auto& [name, index] : m_mapAnimationNamesToIndex) {
    spdlog::debug("{}  Name: '{}', Index: {}", tabPrefix, name, index);
  }

  spdlog::debug("{}m_queAnimationQueue size: {}", tabPrefix,
                m_queAnimationQueue.size());
  if (!m_queAnimationQueue.empty()) {
    std::queue<int32_t> tempQueue = m_queAnimationQueue;  // Copy to iterate
    spdlog::debug("{}Queue contents:", tabPrefix);
    while (!tempQueue.empty()) {
      int32_t index = tempQueue.front();
      tempQueue.pop();
      spdlog::debug("{}  Animation Index: {}", tabPrefix, index);
    }
  }
}

}  // namespace plugin_filament_view