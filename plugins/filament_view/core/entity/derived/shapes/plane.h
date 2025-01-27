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

#include "baseshape.h"
#include "shell/platform/common/client_wrapper/include/flutter/encodable_value.h"

namespace plugin_filament_view {

using ::utils::Entity;

namespace shapes {

class Plane : public BaseShape {
 public:
  explicit Plane(const flutter::EncodableMap& params);
  Plane() = default;
  ~Plane() override = default;

  // Disallow copy and assign.
  Plane(const Plane&) = delete;
  Plane& operator=(const Plane&) = delete;

  void DebugPrint(const char* tag) const override;

  bool bInitAndCreateShape(::filament::Engine* engine_,
                           std::shared_ptr<Entity> entityObject) override;

 private:
  void createDoubleSidedPlane(::filament::Engine* engine_);

  void createSingleSidedPlane(::filament::Engine* engine_);
};

}  // namespace shapes
}  // namespace plugin_filament_view
