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

#include "light_system.h"

#include <core/include/color.h>
#include <core/systems/derived/filament_system.h>
#include <core/systems/ecsystems_manager.h>
#include <filament/Color.h>
#include <filament/LightManager.h>
#include <plugins/common/common.h>
#include <asio/post.hpp>
#include <core/components/derived/light.h>
#include <core/entity/derived/nonrenderable_entityobject.h>

#include "entityobject_locator_system.h"

namespace plugin_filament_view {
  using filament::math::float3;
  using filament::math::mat3f;
  using filament::math::mat4f;

  ////////////////////////////////////////////////////////////////////////////////////
  void LightSystem::vCreateDefaultLight() {
    SPDLOG_DEBUG("{}", __FUNCTION__);
    m_poDefaultLight = std::make_shared<NonRenderableEntityObject>("DefaultLight");
    auto oLightComp = std::make_shared<Light>();
    m_poDefaultLight->vAddComponent(oLightComp);

    oLightComp->SetIntensity(200);
    oLightComp->SetDirection({0,-1,0});
    oLightComp->SetPosition({0, 5, 0});
    oLightComp->SetCastLight(true);
    // if you're in an closed space (IE Garage), it will self shadow cast
    oLightComp->SetCastShadows(false);

    vBuildLightAndAddToScene(*oLightComp);

    // register throughout systems
    const auto objectLocatorSystem =
          ECSystemManager::GetInstance()->poGetSystemAs<EntityObjectLocatorSystem>(
              EntityObjectLocatorSystem::StaticGetTypeID(), "vCreateDefaultLight");

    objectLocatorSystem->vRegisterEntityObject(m_poDefaultLight);
    vRegisterEntityObject(m_poDefaultLight);
  }

////////////////////////////////////////////////////////////////////////////////////
void LightSystem::vBuildLightAndAddToScene(Light& light) const {
  vBuildLight(light);
  vAddLightToScene(light);
}

////////////////////////////////////////////////////////////////////////////////////
void LightSystem::vBuildLight(Light& light) const {
    const auto filamentSystem =
                ECSystemManager::GetInstance()->poGetSystemAs<FilamentSystem>(
                    FilamentSystem::StaticGetTypeID(), "changeLight");
    const auto engine = filamentSystem->getFilamentEngine();

    if(light.m_poFilamentEntityLight == nullptr) {
      light.m_poFilamentEntityLight = std::make_shared<utils::Entity>(engine->getEntityManager().create());
    } else {
      vRemoveLightFromScene(light);
    }

    auto builder = filament::LightManager::Builder(light.GetLightType());

    // As of 11.18.2024 it seems like the color ranges are not the same
    // as their documentation expects 0-1 values, but the actual is 0-255 value
    if (!light.GetColor().empty()) {
      auto colorValue = colorOf(light.GetColor());
      builder.color({colorValue[0], colorValue[1], colorValue[2]});
    } else if (light.GetColorTemperature() > 0) {
      auto cct = filament::Color::cct(light.GetColorTemperature());
      auto red = cct.r;
      auto green = cct.g;
      auto blue = cct.b;
      builder.color({red * 255, green * 255, blue * 255});
    } else {
      builder.color({255, 255, 255});
    }

    // Note while not all of these vars are used in every scenario
    // we're expecting filament to throw away the values that are
    // not needed.
    builder.intensity(light.GetIntensity());
    builder.position(light.GetPosition());
    builder.direction(light.GetDirection());
    builder.castLight(light.GetCastLight());
    builder.castShadows(light.GetCastShadows());
    builder.falloff(light.GetFalloffRadius());

    builder.spotLightCone(light.GetSpotLightConeInner(),
      light.GetSpotLightConeOuter());

    builder.sunAngularRadius(light.GetSunAngularRadius());
    builder.sunHaloSize(light.GetSunHaloSize());
    builder.sunHaloFalloff(light.GetSunHaloFalloff());

    builder.build(*engine, *light.m_poFilamentEntityLight);
}

////////////////////////////////////////////////////////////////////////////////////
void LightSystem::vRemoveLightFromScene(Light& light) {
    auto filamentSystem =
              ECSystemManager::GetInstance()->poGetSystemAs<FilamentSystem>(
                  FilamentSystem::StaticGetTypeID(), "lightManager::changelight");

    auto scene = filamentSystem->getFilamentScene();

    scene->removeEntities(light.m_poFilamentEntityLight.get(), 1);
}

////////////////////////////////////////////////////////////////////////////////////
void LightSystem::vAddLightToScene(Light& light) {
    auto filamentSystem =
              ECSystemManager::GetInstance()->poGetSystemAs<FilamentSystem>(
                  FilamentSystem::StaticGetTypeID(), "lightManager::changelight");

    auto scene = filamentSystem->getFilamentScene();

    scene->addEntity(*light.m_poFilamentEntityLight);
}

////////////////////////////////////////////////////////////////////////////////////
void LightSystem::vInitSystem() {
  vRegisterMessageHandler(
      ECSMessageType::ChangeSceneLightProperties,
      [this](const ECSMessage& msg) {
        spdlog::debug("ChangeSceneLightProperties");

        const auto colorValue = msg.getData<std::string>(
            ECSMessageType::ChangeSceneLightPropertiesColorValue);

        const auto intensityValue = msg.getData<float>(
            ECSMessageType::ChangeSceneLightPropertiesIntensity);

        //defaultlight_->ChangeColor(colorValue);
        //defaultlight_->ChangeIntensity(intensityValue);
        //changeLight(defaultlight_.get());

        spdlog::debug("ChangeSceneLightProperties Complete");
      });

    // add light

    // remove light
}

////////////////////////////////////////////////////////////////////////////////////
void LightSystem::vUpdate(float /*fElapsedTime*/) {}

////////////////////////////////////////////////////////////////////////////////////
void LightSystem::vShutdownSystem() {
  if(m_poDefaultLight != nullptr) {
    auto component = dynamic_cast<Light*>(m_poDefaultLight->GetComponentByStaticTypeID(Light::StaticGetTypeID()).get());
    vRemoveLightFromScene(*component);

    const auto objectLocatorSystem =
          ECSystemManager::GetInstance()->poGetSystemAs<EntityObjectLocatorSystem>(
              EntityObjectLocatorSystem::StaticGetTypeID(), "vCreateDefaultLight");

    objectLocatorSystem->vUnregisterEntityObject(m_poDefaultLight);
    vUnregisterEntityObject(m_poDefaultLight);
    
    m_poDefaultLight.reset();
  }
}

////////////////////////////////////////////////////////////////////////////////////
void LightSystem::DebugPrint() {
  spdlog::debug("{}::{}", __FILE__, __FUNCTION__);

  // TODO Update print out list of lights
}

////////////////////////////////////////////////////////////////////////////////////
void LightSystem::vRegisterEntityObject(
    const std::shared_ptr<EntityObject>& entity) {
  if (m_mapGuidToEntity.find(entity->GetGlobalGuid()) != m_mapGuidToEntity.end()) {
    spdlog::error("{}::{}: Entity {} already registered", __FILE__,
                  __FUNCTION__, entity->GetGlobalGuid());
    return;
  }

  m_mapGuidToEntity.insert(std::pair(entity->GetGlobalGuid(), entity));
}

////////////////////////////////////////////////////////////////////////////////////
void LightSystem::vUnregisterEntityObject(
    const std::shared_ptr<EntityObject>& entity) {
  m_mapGuidToEntity.erase(entity->GetGlobalGuid());
}
}  // namespace plugin_filament_view