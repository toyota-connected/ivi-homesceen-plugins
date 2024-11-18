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

#include "light.h"

#include <core/include/literals.h>
#include <core/utils/deserialize.h>
#include <plugins/common/common.h>

namespace plugin_filament_view {

////////////////////////////////////////////////////////////////////////////
Light2::Light2(float colorTemperature,
             float intensity,
             filament::math::float3 direction,
             bool castShadows) {
  type_ = filament::LightManager::Type::DIRECTIONAL;
  colorTemperature_ = colorTemperature;
  intensity_ = intensity;
  direction_ = std::make_unique<filament::math::float3>(direction);
  castShadows_ = castShadows;
}

////////////////////////////////////////////////////////////////////////////
Light2::Light2(const flutter::EncodableMap& params) {
  SPDLOG_TRACE("++{}::{}", __FILE__, __FUNCTION__);
  for (const auto& [fst, snd] : params) {
    auto key = std::get<std::string>(fst);
    if (snd.IsNull()) {
      SPDLOG_WARN("Light Param ITER is null key:{} file:{} function:{}", key,
                  __FILE__, __FUNCTION__);
      continue;
    }

    if (key == kType && std::holds_alternative<std::string>(snd)) {
      type_ = textToLightType(std::get<std::string>(snd));
    } else if (key == kColor && std::holds_alternative<std::string>(snd)) {
      color_ = std::get<std::string>(snd);
    } else if (key == kColorTemperature &&
               std::holds_alternative<double>(snd)) {
      colorTemperature_ = std::get<double>(snd);
    } else if (key == kIntensity && std::holds_alternative<double>(snd)) {
      intensity_ = std::get<double>(snd);
    } else if (key == kPosition &&
               std::holds_alternative<flutter::EncodableMap>(snd)) {
      position_ = std::make_unique<filament::math::float3>(
          Deserialize::Format3(std::get<flutter::EncodableMap>(snd)));
    } else if (key == kDirection &&
               std::holds_alternative<flutter::EncodableMap>(snd)) {
      direction_ = std::make_unique<filament::math::float3>(
          Deserialize::Format3(std::get<flutter::EncodableMap>(snd)));
    } else if (key == kCastLight && std::holds_alternative<bool>(snd)) {
      castLight_ = std::get<bool>(snd);
    } else if (key == kCastShadows && std::holds_alternative<bool>(snd)) {
      castShadows_ = std::get<bool>(snd);
    } else if (key == kFalloffRadius && std::holds_alternative<double>(snd)) {
      falloffRadius_ = std::get<double>(snd);
    } else if (key == kSpotLightConeInner &&
               std::holds_alternative<double>(snd)) {
      spotLightConeInner_ = std::get<double>(snd);
    } else if (key == kSpotLightConeOuter &&
               std::holds_alternative<double>(snd)) {
      spotLightConeOuter_ = std::get<double>(snd);
    } else if (key == kSunAngularRadius &&
               std::holds_alternative<double>(snd)) {
      sunAngularRadius_ = std::get<double>(snd);
    } else if (key == kSunHaloSize &&
               std::holds_alternative<double>(snd)) {
      sunHaloSize_ = std::get<double>(snd);
    } else if (key == kSunHaloFalloff &&
               std::holds_alternative<double>(snd)) {
      sunHaloFalloff_ = std::get<double>(snd);
    } /*else if (!it.second.IsNull()) {
          spdlog::debug("[Light] Unhandled Parameter {}", key.c_str());
          plugin_common::Encodable::PrintFlutterEncodableValue(key.c_str(),
                                                               it.second);
        }*/
  }

  // Print("Setup Light");
  SPDLOG_TRACE("--{}::{}", __FILE__, __FUNCTION__);
}

////////////////////////////////////////////////////////////////////////////
void Light2::DebugPrint(const std::string& tabPrefix) const {
  spdlog::debug(tabPrefix + "Light Debug Info:");
  spdlog::debug(tabPrefix + "Type: {}", lightTypeToText(type_));

  if (color_.has_value()) {
    spdlog::debug(tabPrefix + "Color: {}", color_.value());
  }

  if (colorTemperature_.has_value()) {
    spdlog::debug(tabPrefix + "Color Temperature: {}",
                  colorTemperature_.value());
  }

  if (intensity_.has_value()) {
    spdlog::debug(tabPrefix + "Intensity: {}", intensity_.value());
  }

  if (position_) {
    spdlog::debug(tabPrefix + "Position: x={}, y={}, z={}", position_->x,
                  position_->y, position_->z);
  }

  if (direction_) {
    spdlog::debug(tabPrefix + "Direction: x={}, y={}, z={}", direction_->x,
                  direction_->y, direction_->z);
  }

  if (castLight_.has_value()) {
    spdlog::debug(tabPrefix + "Casts Light: {}", castLight_.value());
  }

  if (castShadows_.has_value()) {
    spdlog::debug(tabPrefix + "Casts Shadows: {}", castShadows_.value());
  }

  if (falloffRadius_.has_value()) {
    spdlog::debug(tabPrefix + "Falloff Radius: {}", falloffRadius_.value());
  }

  if (spotLightConeInner_.has_value()) {
    spdlog::debug(tabPrefix + "Spotlight Cone Inner Angle: {}",
                  spotLightConeInner_.value());
  }

  if (spotLightConeOuter_.has_value()) {
    spdlog::debug(tabPrefix + "Spotlight Cone Outer Angle: {}",
                  spotLightConeOuter_.value());
  }

  if (sunAngularRadius_.has_value()) {
    spdlog::debug(tabPrefix + "Sun Angular Radius: {}",
                  sunAngularRadius_.value());
  }

  if (sunHaloSize_.has_value()) {
    spdlog::debug(tabPrefix + "Sun Halo Size: {}", sunHaloSize_.value());
  }

  if (sunHaloFalloff_.has_value()) {
    spdlog::debug(tabPrefix + "Sun Halo Falloff: {}", sunHaloFalloff_.value());
  }
}

////////////////////////////////////////////////////////////////////////////
filament::LightManager::Type Light2::textToLightType(const std::string& type) {
  static constexpr std::pair<const char*, filament::LightManager::Type>
      typeMap[] = {
          {"SUN", filament::LightManager::Type::SUN},
          {"DIRECTIONAL", filament::LightManager::Type::DIRECTIONAL},
          {"POINT", filament::LightManager::Type::POINT},
          {"FOCUSED_SPOT", filament::LightManager::Type::FOCUSED_SPOT},
          {"SPOT", filament::LightManager::Type::SPOT},
      };

  for (const auto& [text, lightType] : typeMap) {
    if (type == text) {
      return lightType;
    }
  }

  return filament::LightManager::Type::DIRECTIONAL;  // Default fallback
}

////////////////////////////////////////////////////////////////////////////
const char* Light2::lightTypeToText(const filament::LightManager::Type type) {
  static constexpr std::pair<filament::LightManager::Type, const char*>
      typeMap[] = {
          {filament::LightManager::Type::SUN, "SUN"},
          {filament::LightManager::Type::DIRECTIONAL, "DIRECTIONAL"},
          {filament::LightManager::Type::POINT, "POINT"},
          {filament::LightManager::Type::FOCUSED_SPOT, "FOCUSED_SPOT"},
          {filament::LightManager::Type::SPOT, "SPOT"},
      };

  for (const auto& [lightType, text] : typeMap) {
    if (type == lightType) {
      return text;
    }
  }

  return "DIRECTIONAL";  // Default fallback
}

}  // namespace plugin_filament_view