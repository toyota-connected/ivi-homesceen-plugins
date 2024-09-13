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

#include <filament/math/quat.h>
#include "shell/platform/common/client_wrapper/include/flutter/encodable_value.h"

namespace plugin_filament_view {

class CommonRenderable {
 public:
  // Constructor
  CommonRenderable()
      : m_bCullingOfObjectEnabled(true),
        m_bCastShadows(false),
        m_bReceiveShadows(false) {}
  explicit CommonRenderable(const flutter::EncodableMap& params);

  // Getters
  [[nodiscard]] bool IsCullingOfObjectEnabled() const {
    return m_bCullingOfObjectEnabled;
  }

  [[nodiscard]] bool IsReceiveShadowsEnabled() const {
    return m_bReceiveShadows;
  }

  [[nodiscard]] bool IsCastShadowsEnabled() const { return m_bCastShadows; }

  // Setters
  void SetCullingOfObjectEnabled(bool enabled) {
    m_bCullingOfObjectEnabled = enabled;
  }

  void SetReceiveShadows(bool enabled) { m_bReceiveShadows = enabled; }

  void SetCastShadows(bool enabled) { m_bCastShadows = enabled; }

  void DebugPrint() const;

 private:
  bool m_bCullingOfObjectEnabled;
  bool m_bReceiveShadows;
  bool m_bCastShadows;
};

}  // namespace plugin_filament_view