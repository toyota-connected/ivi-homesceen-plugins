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

#ifndef PIGEON_MESSAGES_G_H_
#define PIGEON_MESSAGES_G_H_
#include <flutter/basic_message_channel.h>
#include <flutter/binary_messenger.h>
#include <flutter/encodable_value.h>
#include <flutter/shell/platform/common/json_method_codec.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <optional>
#include <string>

namespace nav_render_view_plugin {

// Generated class from Pigeon.

class FlutterError {
 public:
  explicit FlutterError(const std::string& code) : code_(code) {}
  explicit FlutterError(const std::string& code, const std::string& message)
      : code_(code), message_(message) {}
  explicit FlutterError(const std::string& code,
                        const std::string& message,
                        const flutter::EncodableValue& details)
      : code_(code), message_(message), details_(details) {}

  const std::string& code() const { return code_; }
  const std::string& message() const { return message_; }
  const flutter::EncodableValue& details() const { return details_; }

 private:
  std::string code_;
  std::string message_;
  flutter::EncodableValue details_;
};

template <class T>
class ErrorOr {
 public:
  ErrorOr(const T& rhs) : v_(rhs) {}
  ErrorOr(const T&& rhs) : v_(std::move(rhs)) {}
  ErrorOr(const FlutterError& rhs) : v_(rhs) {}
  ErrorOr(const FlutterError&& rhs) : v_(std::move(rhs)) {}

  bool has_error() const { return std::holds_alternative<FlutterError>(v_); }
  const T& value() const { return std::get<T>(v_); };
  const FlutterError& error() const { return std::get<FlutterError>(v_); };

 private:
  friend class NavRenderApi;
  ErrorOr() = default;
  T TakeValue() && { return std::get<T>(std::move(v_)); }

  std::variant<T, FlutterError> v_;
};

// Generated interface from Pigeon that represents a handler of messages from
// Flutter.
class NavRenderViewApi {
 public:
  NavRenderViewApi(const NavRenderViewApi&) = delete;
  NavRenderViewApi& operator=(const NavRenderViewApi&) = delete;
  virtual ~NavRenderViewApi() = default;

  // The codec used by NavRenderViewApi.
  static const flutter::StandardMethodCodec& GetCodec();
  // Sets up an instance of `NavRenderViewApi` to handle messages through the
  // `binary_messenger`.
  static void SetUp(flutter::BinaryMessenger* binary_messenger,
                    NavRenderViewApi* api);
  static flutter::EncodableValue WrapError(std::string_view error_message);
  static flutter::EncodableValue WrapError(const FlutterError& error);

 protected:
  NavRenderViewApi() = default;
};

}  // namespace nav_render_view_plugin
#endif  // PIGEON_MESSAGES_G_H_
