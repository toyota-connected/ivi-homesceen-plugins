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
#pragma once

#include <queue>

#include "shell/platform/common/client_wrapper/include/flutter/encodable_value.h"

#include <core/components/base/component.h>
#include <filament/math/quat.h>
#include <gltfio/Animator.h>
#include <gltfio/AssetLoader.h>

namespace plugin_filament_view {

class Animation final : public Component {
 public:
  // Constructor
  explicit Animation(const flutter::EncodableMap& params);

  void DebugPrint(const std::string& tabPrefix) const override;

  static size_t StaticGetTypeID() { return typeid(Animation).hash_code(); }

  [[nodiscard]] size_t GetTypeID() const override { return StaticGetTypeID(); }

  [[nodiscard]] Component* Clone() const override {
    return new Animation(*this);
  }

  void vSetAnimator(filament::gltfio::Animator& animator);
  void vPlayAnimation(int32_t index);
  [[maybe_unused]] bool bPlayAnimation(const std::string& szName);

  void vUpdate(float fElapsedTime);

  [[nodiscard]] float fGetPlaybackSpeedScalar() const {
    return m_fPlaybackSpeedScalar;
  }

  // Setter for m_fPlaybackSpeedScalar
  void vSetPlaybackSpeedScalar(float playbackSpeedScalar) {
    m_fPlaybackSpeedScalar = playbackSpeedScalar;
  }

  void vSetPaused(bool paused) { m_bPaused = paused; }

  void vPause() { m_bPaused = true; }

  void vResume() { m_bPaused = false; }

  // Queue management
  void vEnqueueAnimation(int32_t index);
  void vClearQueue();

  void vSetLooping(bool bValue) { m_bLoop = bValue; }

 private:
  int32_t m_nCurrentPlayingIndex{};
  bool m_bPaused;
  bool m_bAutoPlay{};
  bool m_bLoop{};
  bool m_bResetToTPoseOnReset{};
  float m_fPlaybackSpeedScalar{};
  bool m_bNotifyOfAnimationEvents{};

  float m_fTimeSinceStart{};

  filament::gltfio::Animator* m_poAnimator{};

  // Setup when the animator is set.
  std::map<std::string, size_t> m_mapAnimationNamesToIndex;
  void vSetupAnimationNameMapping();

  std::queue<int32_t> m_queAnimationQueue;
};

}  // namespace plugin_filament_view