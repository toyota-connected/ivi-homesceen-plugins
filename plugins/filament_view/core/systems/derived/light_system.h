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

#include <future>

#include <core/include/resource.h>
#include <core/scene/light/light.h>
#include <core/scene/view_target.h>
#include <core/systems/base/ecsystem.h>

namespace plugin_filament_view {

class Light;

class LightSystem : public ECSystem {
 public:
  LightSystem() = default;

  void setDefaultLight();

  std::future<Resource<std::string_view>> changeLight(Light* light);

  // Disallow copy and assign.
  LightSystem(const LightSystem&) = delete;
  LightSystem& operator=(const LightSystem&) = delete;

  [[nodiscard]] size_t GetTypeID() const override { return StaticGetTypeID(); }

  [[nodiscard]] static size_t StaticGetTypeID() {
    return typeid(LightSystem).hash_code();
  }

  void vInitSystem() override;
  void vUpdate(float fElapsedTime) override;
  void vShutdownSystem() override;
  void DebugPrint() override;

 private:
  utils::Entity entityLight_;
  std::unique_ptr<Light> defaultlight_;
};
}  // namespace plugin_filament_view