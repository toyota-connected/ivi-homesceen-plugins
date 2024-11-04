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
#include "entityobject_locator_system.h"

#include <core/systems/ecsystems_manager.h>
#include <plugins/common/common.h>

namespace plugin_filament_view {

////////////////////////////////////////////////////////////////////////////////////
void EntityObjectLocatorSystem::vInitSystem() {}

////////////////////////////////////////////////////////////////////////////////////
void EntityObjectLocatorSystem::vUpdate(float /*fElapsedTime*/) {}

////////////////////////////////////////////////////////////////////////////////////
void EntityObjectLocatorSystem::vShutdownSystem() {}

////////////////////////////////////////////////////////////////////////////////////
void EntityObjectLocatorSystem::DebugPrint() {
  spdlog::debug("{}::{}", __FILE__, __FUNCTION__);
}

////////////////////////////////////////////////////////////////////////////////////
void EntityObjectLocatorSystem::vRegisterEntityObject(
    const std::shared_ptr<EntityObject>& entity) {
  if (_entities.find(entity->GetGlobalGuid()) != _entities.end()) {
    spdlog::error("{}::{}: Entity {} already registered", __FILE__,
                  __FUNCTION__, entity->GetGlobalGuid());
    return;
  }

  _entities.insert(std::pair(entity->GetGlobalGuid(), entity));
}

////////////////////////////////////////////////////////////////////////////////////
void EntityObjectLocatorSystem::vUnregisterEntityObject(
    const std::shared_ptr<EntityObject>& entity) {
  _entities.erase(entity->GetGlobalGuid());
}

////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<EntityObject> EntityObjectLocatorSystem::poGetEntityObjectById(
    EntityGUID id) const {
  const auto it = _entities.find(id);
  if (it == _entities.end()) {
    spdlog::debug("Unable to find entity with id {}", id);
    return nullptr;
  }
  return it->second;
}

}  // namespace plugin_filament_view