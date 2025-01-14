// /*
//  * Copyright 2024 Toyota Connected North America
//  *
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  *      http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  */
// Autogenerated from Pigeon (v22.7.0), do not edit directly.
// See also: https://pub.dev/packages/pigeon

#ifndef PIGEON_MESSAGES_G_H_
#define PIGEON_MESSAGES_G_H_
#include <flutter/basic_message_channel.h>
#include <flutter/binary_messenger.h>
#include <flutter/encodable_value.h>
#include <flutter/standard_message_codec.h>

#include <map>
#include <optional>
#include <string>
#include <utility>

namespace plugin_filament_view {

// Generated class from Pigeon.

class FlutterError {
 public:
  explicit FlutterError(std::string code)
      : code_(std::move(code)), details_() {}

  explicit FlutterError(std::string code, std::string message)
      : code_(std::move(code)), message_(std::move(message)), details_() {}

  explicit FlutterError(std::string code,
                        std::string message,
                        flutter::EncodableValue details)
      : code_(std::move(code)),
        message_(std::move(message)),
        details_(std::move(details)) {}

  [[nodiscard]] const std::string& code() const { return code_; }
  [[nodiscard]] const std::string& message() const { return message_; }
  [[nodiscard]] const flutter::EncodableValue& details() const {
    return details_;
  }

 private:
  std::string code_;
  std::string message_;
  flutter::EncodableValue details_;
};

template <class T>
class ErrorOr {
 public:
  explicit ErrorOr(const T& rhs) : v_(rhs) {}
  explicit ErrorOr(const T&& rhs) : v_(std::move(rhs)) {}
  explicit ErrorOr(const FlutterError& rhs) : v_(rhs) {}
  explicit ErrorOr(const FlutterError&& rhs) : v_(std::move(rhs)) {}

  [[nodiscard]] bool has_error() const {
    return std::holds_alternative<FlutterError>(v_);
  }
  const T& value() const { return std::get<T>(v_); };
  [[nodiscard]] const FlutterError& error() const {
    return std::get<FlutterError>(v_);
  };

 private:
  friend class FilamentViewApi;
  ErrorOr() = default;
  T TakeValue() && { return std::get<T>(std::move(v_)); }

  std::variant<T, FlutterError> v_;
};

class PigeonInternalCodecSerializer : public flutter::StandardCodecSerializer {
 public:
  PigeonInternalCodecSerializer();
  inline static PigeonInternalCodecSerializer& GetInstance() {
    static PigeonInternalCodecSerializer sInstance;
    return sInstance;
  }

  void WriteValue(const flutter::EncodableValue& value,
                  flutter::ByteStreamWriter* stream) const override;

 protected:
  flutter::EncodableValue ReadValueOfType(
      uint8_t type,
      flutter::ByteStreamReader* stream) const override;
};

// Generated interface from Pigeon that represents a handler of messages from
// Flutter.
class FilamentViewApi {
 public:
  FilamentViewApi(const FilamentViewApi&) = delete;
  FilamentViewApi& operator=(const FilamentViewApi&) = delete;
  virtual ~FilamentViewApi() = default;
  // Change material parameters for the given entity.
  virtual std::optional<FlutterError> ChangeMaterialParameter(
      const flutter::EncodableMap& params,
      const std::string& guid) = 0;
  // Change material definition for the given entity.
  virtual std::optional<FlutterError> ChangeMaterialDefinition(
      const flutter::EncodableMap& params,
      const std::string& guid) = 0;
  // Toggle shapes visibility in the scene.
  virtual std::optional<FlutterError> ToggleShapesInScene(bool value) = 0;
  // Toggle debug collidable visuals in the scene.
  virtual std::optional<FlutterError> ToggleDebugCollidableViewsInScene(
      bool value) = 0;
  // Change the camera mode by name.
  virtual std::optional<FlutterError> ChangeCameraMode(
      const std::string& mode) = 0;
  virtual std::optional<FlutterError>
  ChangeCameraOrbitHomePosition(double x, double y, double z) = 0;
  virtual std::optional<FlutterError> ChangeCameraTargetPosition(double x,
                                                                 double y,
                                                                 double z) = 0;
  virtual std::optional<FlutterError>
  ChangeCameraFlightStartPosition(double x, double y, double z) = 0;
  // Reset inertia camera to default values.
  virtual std::optional<FlutterError> ResetInertiaCameraToDefaultValues() = 0;
  // Change view quality settings.
  virtual std::optional<FlutterError> ChangeViewQualitySettings() = 0;
  // Set camera rotation by a float value.
  virtual std::optional<FlutterError> SetCameraRotation(double value) = 0;
  virtual std::optional<FlutterError> ChangeLightTransformByGUID(
      const std::string& guid,
      double posx,
      double posy,
      double posz,
      double dirx,
      double diry,
      double dirz) = 0;
  virtual std::optional<FlutterError> ChangeLightColorByGUID(
      const std::string& guid,
      const std::string& color,
      int64_t intensity) = 0;
  virtual std::optional<FlutterError> EnqueueAnimation(
      const std::string& guid,
      int64_t animation_index) = 0;
  virtual std::optional<FlutterError> ClearAnimationQueue(
      const std::string& guid) = 0;
  virtual std::optional<FlutterError> PlayAnimation(
      const std::string& guid,
      int64_t animation_index) = 0;
  virtual std::optional<FlutterError> ChangeAnimationSpeed(
      const std::string& guid,
      double speed) = 0;
  virtual std::optional<FlutterError> PauseAnimation(
      const std::string& guid) = 0;
  virtual std::optional<FlutterError> ResumeAnimation(
      const std::string& guid) = 0;
  virtual std::optional<FlutterError> SetAnimationLooping(
      const std::string& guid,
      bool looping) = 0;
  virtual std::optional<FlutterError> RequestCollisionCheckFromRay(
      const std::string& query_i_d,
      double origin_x,
      double origin_y,
      double origin_z,
      double direction_x,
      double direction_y,
      double direction_z,
      double length) = 0;

  virtual std::optional<FlutterError> ChangeScaleByGUID(const std::string& guid,
                                                        double x,
                                                        double y,
                                                        double z) = 0;
  virtual std::optional<FlutterError> ChangeTranslationByGUID(
      const std::string& guid,
      double x,
      double y,
      double z) = 0;
  virtual std::optional<FlutterError> ChangeRotationByGUID(
      const std::string& guid,
      double x,
      double y,
      double z,
      double w) = 0;
  virtual std::optional<FlutterError> TurnOffVisualForEntity(
      const std::string& guid) = 0;
  virtual std::optional<FlutterError> TurnOnVisualForEntity(
      const std::string& guid) = 0;
  virtual std::optional<FlutterError> TurnOffCollisionChecksForEntity(
      const std::string& guid) = 0;
  virtual std::optional<FlutterError> TurnOnCollisionChecksForEntity(
      const std::string& guid) = 0;

  // The codec used by FilamentViewApi.
  static const flutter::StandardMessageCodec& GetCodec();
  // Sets up an instance of `FilamentViewApi` to handle messages through the
  // `binary_messenger`.
  static void SetUp(flutter::BinaryMessenger* binary_messenger,
                    FilamentViewApi* api);
  static void SetUp(flutter::BinaryMessenger* binary_messenger,
                    FilamentViewApi* api,
                    const std::string& message_channel_suffix);
  static flutter::EncodableValue WrapError(std::string_view error_message);
  static flutter::EncodableValue WrapError(const FlutterError& error);

 protected:
  FilamentViewApi() = default;
};
}  // namespace plugin_filament_view
#endif  // PIGEON_MESSAGES_G_H_
