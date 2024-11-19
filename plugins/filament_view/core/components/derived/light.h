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
#include <filament/LightManager.h>

namespace plugin_filament_view {

class Light : public Component {
  friend class LightSystem;

 public:
  // Constructors
  Light()
      : Component(std::string(__FUNCTION__)),
        m_Type(filament::LightManager::Type::DIRECTIONAL),
        m_fColorTemperature(6500000.0f),
        m_fIntensity(100000.0f),
        m_f3Position(0.0f, 1.0f, 0.0f),
        m_f3Direction(0.0f, -1.0f, 0.0f),
        m_bCastLight(true),
        m_bCastShadows(true),
        m_fFalloffRadius(1000.0f),
        m_fSpotLightConeInner(0.0f),
        m_fSpotLightConeOuter(0.0f),
        m_fSunAngularRadius(0.0f),
        m_fSunHaloSize(0.0f),
        m_fSunHaloFalloff(0.0f) {}

  explicit Light(const flutter::EncodableMap& params);

  [[nodiscard]] filament::LightManager::Type GetLightType() const {
    return m_Type;
  }
  [[nodiscard]] const std::string& GetColor() const { return m_szColor; }
  [[nodiscard]] float GetColorTemperature() const {
    return m_fColorTemperature;
  }
  [[nodiscard]] float GetIntensity() const { return m_fIntensity; }
  [[nodiscard]] const filament::math::float3& GetPosition() const {
    return m_f3Position;
  }
  [[nodiscard]] const filament::math::float3& GetDirection() const {
    return m_f3Direction;
  }
  [[nodiscard]] bool GetCastLight() const { return m_bCastLight; }
  [[nodiscard]] bool GetCastShadows() const { return m_bCastShadows; }
  [[nodiscard]] float GetFalloffRadius() const { return m_fFalloffRadius; }
  [[nodiscard]] float GetSpotLightConeInner() const {
    return m_fSpotLightConeInner;
  }
  [[nodiscard]] float GetSpotLightConeOuter() const {
    return m_fSpotLightConeOuter;
  }
  [[nodiscard]] float GetSunAngularRadius() const {
    return m_fSunAngularRadius;
  }
  [[nodiscard]] float GetSunHaloSize() const { return m_fSunHaloSize; }
  [[nodiscard]] float GetSunHaloFalloff() const { return m_fSunHaloFalloff; }

  void SetLightType(filament::LightManager::Type type) { m_Type = type; }
  void SetColor(const std::string& color) { m_szColor = color; }
  void SetColorTemperature(float temperature) {
    m_fColorTemperature = temperature;
  }
  void SetIntensity(float intensity) { m_fIntensity = intensity; }
  void SetPosition(const filament::math::float3& position) {
    m_f3Position = position;
  }
  void SetDirection(const filament::math::float3& direction) {
    m_f3Direction = direction;
  }
  void SetCastLight(bool castLight) { m_bCastLight = castLight; }
  void SetCastShadows(bool castShadows) { m_bCastShadows = castShadows; }
  void SetFalloffRadius(float radius) { m_fFalloffRadius = radius; }
  void SetSpotLightConeInner(float angle) { m_fSpotLightConeInner = angle; }
  void SetSpotLightConeOuter(float angle) { m_fSpotLightConeOuter = angle; }
  void SetSunAngularRadius(float radius) { m_fSunAngularRadius = radius; }
  void SetSunHaloSize(float size) { m_fSunHaloSize = size; }
  void SetSunHaloFalloff(float falloff) { m_fSunHaloFalloff = falloff; }

  void DebugPrint(const std::string& tabPrefix) const override;

  static size_t StaticGetTypeID() { return typeid(Light).hash_code(); }

  [[nodiscard]] size_t GetTypeID() const override { return StaticGetTypeID(); }

  [[nodiscard]] Component* Clone() const override { return new Light(*this); }

  static ::filament::LightManager::Type textToLightType(
      const std::string& type);

  static const char* lightTypeToText(::filament::LightManager::Type type);

 private:
  std::shared_ptr<utils::Entity> m_poFilamentEntityLight;

  filament::LightManager::Type m_Type;
  std::string m_szColor;
  float m_fColorTemperature;
  float m_fIntensity;
  filament::math::float3 m_f3Position;
  filament::math::float3 m_f3Direction;
  bool m_bCastLight;
  bool m_bCastShadows;
  float m_fFalloffRadius;
  float m_fSpotLightConeInner;
  float m_fSpotLightConeOuter;
  float m_fSunAngularRadius;
  float m_fSunHaloSize;
  float m_fSunHaloFalloff;
};

}  // namespace plugin_filament_view
