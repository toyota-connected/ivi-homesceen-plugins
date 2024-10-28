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

#include <core/entity/base/entityobject.h>
#include <core/systems/base/ecsystem.h>
#include <map>

namespace plugin_filament_view {

class EntityObjectLocatorSystem : public ECSystem {
 public:
  EntityObjectLocatorSystem() = default;

  // Disallow copy and assign.
  EntityObjectLocatorSystem(const EntityObjectLocatorSystem&) = delete;
  EntityObjectLocatorSystem& operator=(const EntityObjectLocatorSystem&) =
      delete;

  [[nodiscard]] size_t GetTypeID() const override { return StaticGetTypeID(); }

  [[nodiscard]] static size_t StaticGetTypeID() {
    return typeid(EntityObjectLocatorSystem).hash_code();
  }

  void vInitSystem() override;
  void vUpdate(float fElapsedTime) override;
  void vShutdownSystem() override;
  void DebugPrint() override;

  void vRegisterEntityObject(std::shared_ptr<EntityObject> entity);
  void vUnregisterEntityObject(std::shared_ptr<EntityObject> entity);

  std::shared_ptr<EntityObject> poGetEntityObjectById(EntityGUID id) const;

 private:
  std::map<EntityGUID, std::shared_ptr<EntityObject>> _entities;
};

}  // namespace plugin_filament_view