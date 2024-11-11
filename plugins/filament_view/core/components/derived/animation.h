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

#include "shell/platform/common/client_wrapper/include/flutter/encodable_value.h"

#include <core/components/base/component.h>
#include <filament/math/quat.h>

namespace plugin_filament_view {

class Animation : public Component {
 public:
  // Constructor
  explicit Animation(const flutter::EncodableMap& params);

  void DebugPrint(const std::string& tabPrefix) const override;

  static size_t StaticGetTypeID() { return typeid(Animation).hash_code(); }

  [[nodiscard]] size_t GetTypeID() const override { return StaticGetTypeID(); }

 [[nodiscard]] Component* Clone() const override {
   return new Animation(*this);
  }

 private:
  int32_t index_{};
  std::string name_;
  bool auto_play_{};
  std::string asset_path_;
};

}  // namespace plugin_filament_view