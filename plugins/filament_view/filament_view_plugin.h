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
#ifndef FLUTTER_PLUGIN_FILAMENT_VIEW_PLUGIN_H_
#define FLUTTER_PLUGIN_FILAMENT_VIEW_PLUGIN_H_

#include <memory>
#include <string>

#include <core/scene/serialization/scene_text_deserializer.h>
#include <flutter/plugin_registrar.h>
#include <flutter_homescreen.h>
#include <messages.g.h>
#include <platform_views/platform_view.h>
#include <wayland/display.h>

namespace plugin_filament_view {

class FilamentViewPlugin : public flutter::Plugin,
                           public FilamentViewApi,
                           public PlatformView {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrar* registrar,
                                    int32_t id,
                                    std::string viewType,
                                    int32_t direction,
                                    double top,
                                    double left,
                                    double width,
                                    double height,
                                    const std::vector<uint8_t>& params,
                                    const std::string& assetDirectory,
                                    FlutterDesktopEngineRef engine,
                                    PlatformViewAddListener addListener,
                                    PlatformViewRemoveListener removeListener,
                                    void* platform_view_context);

  FilamentViewPlugin(int32_t id,
                     std::string viewType,
                     int32_t direction,
                     double top,
                     double left,
                     double width,
                     double height,
                     const std::vector<uint8_t>& params,
                     const std::string& assetDirectory,
                     PlatformViewAddListener addListener,
                     PlatformViewRemoveListener removeListener,
                     void* platform_view_context);

  ~FilamentViewPlugin() override;

  static void setupMessageChannels(flutter::PluginRegistrar* registrar);
  static void sendReadyEvent();

  std::optional<FlutterError> ChangeMaterialParameter(
      const flutter::EncodableMap& params,
      const std::string& guid) override;
  // Change material definition for the given entity.
  std::optional<FlutterError> ChangeMaterialDefinition(
      const flutter::EncodableMap& params,
      const std::string& guid) override;
  // Toggle shapes visibility in the scene.
  std::optional<FlutterError> ToggleShapesInScene(bool value) override;
  // Toggle debug collidable visuals in the scene.
  std::optional<FlutterError> ToggleDebugCollidableViewsInScene(
      bool value) override;
  // Change the camera mode by name.
  std::optional<FlutterError> ChangeCameraMode(
      const std::string& mode) override;
  std::optional<FlutterError> ChangeCameraOrbitHomePosition(double x,
                                                            double y,
                                                            double z) override;
  std::optional<FlutterError> ChangeCameraTargetPosition(double x,
                                                         double y,
                                                         double z) override;
  std::optional<FlutterError>
  ChangeCameraFlightStartPosition(double x, double y, double z) override;
  // Reset inertia camera to default values.
  std::optional<FlutterError> ResetInertiaCameraToDefaultValues() override;
  // Change view quality settings.
  std::optional<FlutterError> ChangeViewQualitySettings() override;
  // Set camera rotation by a float value.
  std::optional<FlutterError> SetCameraRotation(double value) override;
  std::optional<FlutterError> ChangeLightTransformByGUID(
      const std::string& guid,
      double posx,
      double posy,
      double posz,
      double dirx,
      double diry,
      double dirz) override;
  std::optional<FlutterError> ChangeLightColorByGUID(
      const std::string& guid,
      const std::string& color,
      int64_t intensity) override;
  std::optional<FlutterError> EnqueueAnimation(
      const std::string& guid,
      int64_t animation_index) override;
  std::optional<FlutterError> ClearAnimationQueue(
      const std::string& guid) override;
  std::optional<FlutterError> PlayAnimation(const std::string& guid,
                                            int64_t animation_index) override;
  std::optional<FlutterError> ChangeAnimationSpeed(const std::string& guid,
                                                   double speed) override;
  std::optional<FlutterError> PauseAnimation(const std::string& guid) override;
  std::optional<FlutterError> ResumeAnimation(const std::string& guid) override;
  std::optional<FlutterError> SetAnimationLooping(const std::string& guid,
                                                  bool looping) override;
  std::optional<FlutterError> RequestCollisionCheckFromRay(
      const std::string& query_id,
      double origin_x,
      double origin_y,
      double origin_z,
      double direction_x,
      double direction_y,
      double direction_z,
      double length) override;

  std::optional<FlutterError> ChangeScaleByGUID(const std::string& guid,
                                                double x,
                                                double y,
                                                double z) override;
  std::optional<FlutterError> ChangeTranslationByGUID(const std::string& guid,
                                                      double x,
                                                      double y,
                                                      double z) override;
  std::optional<FlutterError> ChangeRotationByGUID(const std::string& guid,
                                                   double x,
                                                   double y,
                                                   double z,
                                                   double w) override;

  // Disallow copy and assign.
  FilamentViewPlugin(const FilamentViewPlugin&) = delete;

  FilamentViewPlugin& operator=(const FilamentViewPlugin&) = delete;

 private:
  int32_t id_;
  void* platformViewsContext_;
  PlatformViewRemoveListener removeListener_;

  static void on_resize(double width, double height, void* data);
  static void on_set_direction(int32_t direction, void* data);
  static void on_set_offset(double left, double top, void* data);
  static void on_touch(int32_t action,
                       int32_t point_count,
                       size_t point_data_size,
                       const double* point_data,
                       void* data);
  static void on_dispose(bool hybrid, void* data);

  static const struct platform_view_listener platform_view_listener_;
};

}  // namespace plugin_filament_view

#endif  // FLUTTER_PLUGIN_FILAMENT_VIEW_PLUGIN_H_
