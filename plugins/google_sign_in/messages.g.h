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
#include <flutter/standard_message_codec.h>

#include <map>
#include <optional>
#include <string>
#include <utility>

namespace google_sign_in_plugin {

// Generated class from Pigeon.

class FlutterError {
 public:
  explicit FlutterError(std::string code) : code_(std::move(code)) {}
  explicit FlutterError(std::string code, std::string message)
      : code_(std::move(code)), message_(std::move(message)) {}
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
  ErrorOr(const T& rhs) : v_(rhs) {}
  ErrorOr(const T&& rhs) : v_(std::move(rhs)) {}
  ErrorOr(const FlutterError& rhs) : v_(rhs) {}
  ErrorOr(const FlutterError&& rhs) : v_(std::move(rhs)) {}

  [[nodiscard]] bool has_error() const {
    return std::holds_alternative<FlutterError>(v_);
  }
  const T& value() const { return std::get<T>(v_); };
  [[nodiscard]] const FlutterError& error() const {
    return std::get<FlutterError>(v_);
  };

 private:
  friend class GoogleSignInApi;
  ErrorOr() = default;
  T TakeValue() && { return std::get<T>(std::move(v_)); }

  std::variant<T, FlutterError> v_;
};

// Pigeon version of SignInOption.
enum class SignInType {
  // Default configuration.
  kStandard = 0,
  // Recommended configuration for game sign in.
  kGames = 1
};

// Pigeon version of SignInInitParams.
//
// See SignInInitParams for details.
//
// Generated class from Pigeon that represents data sent in messages.
class InitParams {
 public:
  // Constructs an object setting all non-nullable fields.
  explicit InitParams(flutter::EncodableList scopes,
                      const SignInType& sign_in_type,
                      bool force_code_for_refresh_token);

  // Constructs an object setting all fields.
  explicit InitParams(flutter::EncodableList scopes,
                      const SignInType& sign_in_type,
                      const std::string* hosted_domain,
                      const std::string* client_id,
                      const std::string* server_client_id,
                      bool force_code_for_refresh_token);

  [[nodiscard]] const flutter::EncodableList& scopes() const;
  void set_scopes(const flutter::EncodableList& value_arg);

  [[nodiscard]] const SignInType& sign_in_type() const;
  void set_sign_in_type(const SignInType& value_arg);

  [[nodiscard]] const std::string* hosted_domain() const;
  void set_hosted_domain(const std::string_view* value_arg);
  void set_hosted_domain(std::string_view value_arg);

  [[nodiscard]] const std::string* client_id() const;
  void set_client_id(const std::string_view* value_arg);
  void set_client_id(std::string_view value_arg);

  [[nodiscard]] const std::string* server_client_id() const;
  void set_server_client_id(const std::string_view* value_arg);
  void set_server_client_id(std::string_view value_arg);

  [[nodiscard]] bool force_code_for_refresh_token() const;
  void set_force_code_for_refresh_token(bool value_arg);

 private:
  static InitParams FromEncodableList(const flutter::EncodableList& list);
  [[nodiscard]] flutter::EncodableList ToEncodableList() const;
  friend class GoogleSignInApi;
  friend class PigeonInternalCodecSerializer;
  flutter::EncodableList scopes_;
  SignInType sign_in_type_;
  std::optional<std::string> hosted_domain_;
  std::optional<std::string> client_id_;
  std::optional<std::string> server_client_id_;
  bool force_code_for_refresh_token_;
};

// Pigeon version of GoogleSignInUserData.
//
// See GoogleSignInUserData for details.
//
// Generated class from Pigeon that represents data sent in messages.
class UserData {
 public:
  // Constructs an object setting all non-nullable fields.
  explicit UserData(std::string email, std::string id);

  // Constructs an object setting all fields.
  explicit UserData(const std::string* display_name,
                    std::string email,
                    std::string id,
                    const std::string* photo_url,
                    const std::string* id_token,
                    const std::string* server_auth_code);

  [[nodiscard]] const std::string* display_name() const;
  void set_display_name(const std::string_view* value_arg);
  void set_display_name(std::string_view value_arg);

  [[nodiscard]] const std::string& email() const;
  void set_email(std::string_view value_arg);

  [[nodiscard]] const std::string& id() const;
  void set_id(std::string_view value_arg);

  [[nodiscard]] const std::string* photo_url() const;
  void set_photo_url(const std::string_view* value_arg);
  void set_photo_url(std::string_view value_arg);

  [[nodiscard]] const std::string* id_token() const;
  void set_id_token(const std::string_view* value_arg);
  void set_id_token(std::string_view value_arg);

  [[nodiscard]] const std::string* server_auth_code() const;
  void set_server_auth_code(const std::string_view* value_arg);
  void set_server_auth_code(std::string_view value_arg);

 private:
  static UserData FromEncodableList(const flutter::EncodableList& list);
  [[nodiscard]] flutter::EncodableList ToEncodableList() const;
  friend class GoogleSignInApi;
  friend class PigeonInternalCodecSerializer;
  std::optional<std::string> display_name_;
  std::string email_;
  std::string id_;
  std::optional<std::string> photo_url_;
  std::optional<std::string> id_token_;
  std::optional<std::string> server_auth_code_;
};

class PigeonInternalCodecSerializer final
    : public flutter::StandardCodecSerializer {
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
class GoogleSignInApi {
 public:
  GoogleSignInApi(const GoogleSignInApi&) = delete;
  GoogleSignInApi& operator=(const GoogleSignInApi&) = delete;
  virtual ~GoogleSignInApi() = default;
  // Initializes a sign in request with the given parameters.
  virtual std::optional<FlutterError> Init(const InitParams& params) = 0;
  // Starts a silent sign in.
  virtual void SignInSilently(
      std::function<void(ErrorOr<UserData> reply)> result) = 0;
  // Starts a sign in with user interaction.
  virtual void SignIn(std::function<void(ErrorOr<UserData> reply)> result) = 0;
  // Requests the access token for the current sign in.
  virtual void GetAccessToken(
      const std::string& email,
      bool should_recover_auth,
      std::function<void(ErrorOr<std::string> reply)> result) = 0;
  // Signs out the current user.
  virtual void SignOut(
      std::function<void(std::optional<FlutterError> reply)> result) = 0;
  // Revokes scope grants to the application.
  virtual void Disconnect(
      std::function<void(std::optional<FlutterError> reply)> result) = 0;
  // Returns whether the user is currently signed in.
  virtual ErrorOr<bool> IsSignedIn() = 0;
  // Clears the authentication caching for the given token, requiring a
  // new sign in.
  virtual void ClearAuthCache(
      const std::string& token,
      std::function<void(std::optional<FlutterError> reply)> result) = 0;
  // Requests access to the given scopes.
  virtual void RequestScopes(
      const flutter::EncodableList& scopes,
      std::function<void(ErrorOr<bool> reply)> result) = 0;

  // The codec used by GoogleSignInApi.
  static const flutter::StandardMessageCodec& GetCodec();
  // Sets up an instance of `GoogleSignInApi` to handle messages through the
  // `binary_messenger`.
  static void SetUp(flutter::BinaryMessenger* binary_messenger,
                    GoogleSignInApi* api);
  static void SetUp(flutter::BinaryMessenger* binary_messenger,
                    GoogleSignInApi* api,
                    const std::string& message_channel_suffix);
  static flutter::EncodableValue WrapError(std::string_view error_message);
  static flutter::EncodableValue WrapError(const FlutterError& error);

 protected:
  GoogleSignInApi() = default;
};
}  // namespace google_sign_in_plugin
#endif  // PIGEON_MESSAGES_G_H_
