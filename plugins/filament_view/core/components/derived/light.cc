#include "light.h"

#include <core/include/literals.h>
#include <core/utils/deserialize.h>
#include <spdlog/spdlog.h>

namespace plugin_filament_view {

////////////////////////////////////////////////////////////////////////////
Light::Light(const flutter::EncodableMap& params)
    : Component(std::string(__FUNCTION__)),
      m_Type(filament::LightManager::Type::DIRECTIONAL),
      m_fColorTemperature(6500.0f),
      m_fIntensity(100000.0f),
      m_f3Position(0.0f, 0.0f, 0.0f),
      m_f3Direction(0.0f, -1.0f, 0.0f),
      m_bCastLight(true),
      m_bCastShadows(true),
      m_fFalloffRadius(100.0f),
      m_fSpotLightConeInner(0.0f),
      m_fSpotLightConeOuter(0.0f),
      m_fSunAngularRadius(0.0f),
      m_fSunHaloSize(0.0f),
      m_fSunHaloFalloff(0.0f) {
  if (auto it = params.find(flutter::EncodableValue(kType));
      it != params.end()) {
    const auto& snd = it->second;
    if (std::holds_alternative<std::string>(snd)) {
      m_Type = textToLightType(std::get<std::string>(snd));
    }
  }

  Deserialize::DecodeParameterWithDefault(kColor, &m_szColor, params,
                                          std::string(""));
  Deserialize::DecodeParameterWithDefault(
      kColorTemperature, &m_fColorTemperature, params, 6500.0f);
  Deserialize::DecodeParameterWithDefault(kIntensity, &m_fIntensity, params,
                                          100000.0f);
  Deserialize::DecodeParameterWithDefault(kPosition, &m_f3Position, params,
                                          filament::math::float3(0, 0, 0));
  Deserialize::DecodeParameterWithDefault(kDirection, &m_f3Direction, params,
                                          filament::math::float3(0, -1, 0));
  Deserialize::DecodeParameterWithDefault(kCastLight, &m_bCastLight, params,
                                          true);
  Deserialize::DecodeParameterWithDefault(kCastShadows, &m_bCastShadows, params,
                                          true);
  Deserialize::DecodeParameterWithDefault(kFalloffRadius, &m_fFalloffRadius,
                                          params, 0.0f);
  Deserialize::DecodeParameterWithDefault(kSpotLightConeInner,
                                          &m_fSpotLightConeInner, params, 0.0f);
  Deserialize::DecodeParameterWithDefault(kSpotLightConeOuter,
                                          &m_fSpotLightConeOuter, params, 0.0f);
  Deserialize::DecodeParameterWithDefault(kSunAngularRadius,
                                          &m_fSunAngularRadius, params, 0.0f);
  Deserialize::DecodeParameterWithDefault(kSunHaloSize, &m_fSunHaloSize, params,
                                          0.0f);
  Deserialize::DecodeParameterWithDefault(kSunHaloFalloff, &m_fSunHaloFalloff,
                                          params, 0.0f);
}

////////////////////////////////////////////////////////////////////////////
void Light::DebugPrint(const std::string& tabPrefix) const {
  spdlog::debug(tabPrefix + "Type: {}", static_cast<int>(m_Type));
  spdlog::debug(tabPrefix + "Color: {}", m_szColor);
  spdlog::debug(tabPrefix + "Color Temperature: {}", m_fColorTemperature);
  spdlog::debug(tabPrefix + "Intensity: {}", m_fIntensity);
  spdlog::debug(tabPrefix + "Position: x={}, y={}, z={}", m_f3Position.x,
                m_f3Position.y, m_f3Position.z);
  spdlog::debug(tabPrefix + "Direction: x={}, y={}, z={}", m_f3Direction.x,
                m_f3Direction.y, m_f3Direction.z);
  spdlog::debug(tabPrefix + "Casts Light: {}", m_bCastLight);
  spdlog::debug(tabPrefix + "Casts Shadows: {}", m_bCastShadows);
  spdlog::debug(tabPrefix + "Falloff Radius: {}", m_fFalloffRadius);
  spdlog::debug(tabPrefix + "Spotlight Cone Inner: {}", m_fSpotLightConeInner);
  spdlog::debug(tabPrefix + "Spotlight Cone Outer: {}", m_fSpotLightConeOuter);
  spdlog::debug(tabPrefix + "Sun Angular Radius: {}", m_fSunAngularRadius);
  spdlog::debug(tabPrefix + "Sun Halo Size: {}", m_fSunHaloSize);
  spdlog::debug(tabPrefix + "Sun Halo Falloff: {}", m_fSunHaloFalloff);
}

////////////////////////////////////////////////////////////////////////////
filament::LightManager::Type Light::textToLightType(const std::string& type) {
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
const char* Light::lightTypeToText(const filament::LightManager::Type type) {
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
