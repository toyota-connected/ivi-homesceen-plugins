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
#include "entityobject.h"

#include <core/components/derived/animation.h>
#include <core/components/derived/light.h>
#include <core/include/literals.h>
#include <core/systems/derived/animation_system.h>
#include <core/systems/derived/entityobject_locator_system.h>
#include <core/systems/derived/light_system.h>
#include <core/systems/ecsystems_manager.h>
#include <core/utils/uuidGenerator.h>
#include <plugins/common/common.h>
#include <utility>

namespace plugin_filament_view {

/////////////////////////////////////////////////////////////////////////////////////////
EntityObject::EntityObject(std::string name)
    : global_guid_(generateUUID()), name_(std::move(name)) {}

/////////////////////////////////////////////////////////////////////////////////////////
EntityObject::EntityObject(std::string name, std::string global_guid)
    : global_guid_(std::move(global_guid)), name_(std::move(name)) {}

/////////////////////////////////////////////////////////////////////////////////////////
void EntityObject::vOverrideName(const std::string& name) {
  name_ = name;
}

/////////////////////////////////////////////////////////////////////////////////////////
void EntityObject::vOverrideGlobalGuid(const std::string& global_guid) {
  global_guid_ = global_guid;
}

/////////////////////////////////////////////////////////////////////////////////////////
void EntityObject::DeserializeNameAndGlobalGuid(
    const flutter::EncodableMap& params) {
  if (const auto itName = params.find(flutter::EncodableValue(kName));
      itName != params.end() && !itName->second.IsNull()) {
    // they're requesting entity be named what they want.

    if (auto requestedName = std::get<std::string>(itName->second);
        !requestedName.empty()) {
      vOverrideName(requestedName);
      SPDLOG_INFO("OVERRIDING NAME: {}", requestedName);
    }
  }

  if (const auto itGUID = params.find(flutter::EncodableValue(kGlobalGuid));
      itGUID != params.end() && !itGUID->second.IsNull()) {
    // they're requesting entity have a guid they desire.
    // Note! There's no clash checking here.
    if (auto requestedGlobalGUID = std::get<std::string>(itGUID->second);
        !requestedGlobalGUID.empty()) {
      vOverrideGlobalGuid(requestedGlobalGUID);
      SPDLOG_INFO("OVERRIDING GLOBAL GUID: {}", requestedGlobalGUID);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void EntityObject::vDebugPrintComponents() const {
  spdlog::debug("EntityObject Name \'{}\' UUID {} ComponentCount {}", name_,
                global_guid_, components_.size());

  for (const auto& component : components_) {
    spdlog::debug("\tComponent Type \'{}\' Name \'{}\'",
                  component->GetRTTITypeName(), component->GetName());
    component->DebugPrint("\t\t");
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void EntityObject::vUnregisterEntity() {
  if (!m_bAlreadyRegistered) {
    return;
  }

  const auto objectLocatorSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<EntityObjectLocatorSystem>(
          EntityObjectLocatorSystem::StaticGetTypeID(), "vRegisterEntity");

  objectLocatorSystem->vUnregisterEntityObject(shared_from_this());

  m_bAlreadyRegistered = false;
}

/////////////////////////////////////////////////////////////////////////////////////////
void EntityObject::vRegisterEntity() {
  if (m_bAlreadyRegistered) {
    return;
  }

  const auto objectLocatorSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<EntityObjectLocatorSystem>(
          EntityObjectLocatorSystem::StaticGetTypeID(), "vRegisterEntity");

  objectLocatorSystem->vRegisterEntityObject(shared_from_this());

  m_bAlreadyRegistered = true;
}

/////////////////////////////////////////////////////////////////////////////////////////
void EntityObject::vShallowCopyComponentToOther(size_t staticTypeID,
                                                EntityObject& other) const {
  const auto component = GetComponentByStaticTypeID(staticTypeID);
  if (component == nullptr) {
    spdlog::warn("Unable to clone component of {}", staticTypeID);
    return;
  }

  other.vAddComponent(std::shared_ptr<Component>(component->Clone()));
}

/////////////////////////////////////////////////////////////////////////////////////////
void EntityObject::vAddComponent(std::shared_ptr<Component> component,
                                 const bool bAutoAddToSystems) {
  component->entityOwner_ = this;

  if (bAutoAddToSystems) {
    if (component->GetTypeID() == Light::StaticGetTypeID()) {
      const auto lightSystem =
          ECSystemManager::GetInstance()->poGetSystemAs<LightSystem>(
              LightSystem::StaticGetTypeID(), __FUNCTION__);

      lightSystem->vRegisterEntityObject(shared_from_this());
    }

    if (component->GetTypeID() == Animation::StaticGetTypeID()) {
      const auto animationSystem =
          ECSystemManager::GetInstance()->poGetSystemAs<AnimationSystem>(
              AnimationSystem::StaticGetTypeID(), "loadModelGltf");

      animationSystem->vRegisterEntityObject(shared_from_this());
    }
  }

  components_.emplace_back(std::move(component));
}

}  // namespace plugin_filament_view