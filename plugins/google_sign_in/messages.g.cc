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

#undef _HAS_EXCEPTIONS

#include "messages.g.h"

#include <flutter/basic_message_channel.h>
#include <flutter/binary_messenger.h>
#include <flutter/encodable_value.h>
#include <flutter/standard_message_codec.h>

#include <optional>
#include <string>

namespace google_sign_in_plugin {
using flutter::BasicMessageChannel;
using flutter::CustomEncodableValue;
using flutter::EncodableList;
using flutter::EncodableMap;
using flutter::EncodableValue;

FlutterError CreateConnectionError(const std::string& channel_name) {
  return FlutterError(
      "channel-error",
      "Unable to establish connection on channel: '" + channel_name + "'.",
      EncodableValue(""));
}

// InitParams

InitParams::InitParams(EncodableList scopes,
                       const SignInType& sign_in_type,
                       const bool force_code_for_refresh_token)
    : scopes_(std::move(scopes)),
      sign_in_type_(sign_in_type),
      force_code_for_refresh_token_(force_code_for_refresh_token) {}

InitParams::InitParams(EncodableList scopes,
                       const SignInType& sign_in_type,
                       const std::string* hosted_domain,
                       const std::string* client_id,
                       const std::string* server_client_id,
                       bool force_code_for_refresh_token)
    : scopes_(std::move(scopes)),
      sign_in_type_(sign_in_type),
      hosted_domain_(hosted_domain ? std::optional<std::string>(*hosted_domain)
                                   : std::nullopt),
      client_id_(client_id ? std::optional<std::string>(*client_id)
                           : std::nullopt),
      server_client_id_(server_client_id
                            ? std::optional<std::string>(*server_client_id)
                            : std::nullopt),
      force_code_for_refresh_token_(force_code_for_refresh_token) {}

const EncodableList& InitParams::scopes() const {
  return scopes_;
}

void InitParams::set_scopes(const EncodableList& value_arg) {
  scopes_ = value_arg;
}

const SignInType& InitParams::sign_in_type() const {
  return sign_in_type_;
}

void InitParams::set_sign_in_type(const SignInType& value_arg) {
  sign_in_type_ = value_arg;
}

const std::string* InitParams::hosted_domain() const {
  return hosted_domain_ ? &(*hosted_domain_) : nullptr;
}

void InitParams::set_hosted_domain(const std::string_view* value_arg) {
  hosted_domain_ =
      value_arg ? std::optional<std::string>(*value_arg) : std::nullopt;
}

void InitParams::set_hosted_domain(std::string_view value_arg) {
  hosted_domain_ = value_arg;
}

const std::string* InitParams::client_id() const {
  return client_id_ ? &(*client_id_) : nullptr;
}

void InitParams::set_client_id(const std::string_view* value_arg) {
  client_id_ =
      value_arg ? std::optional<std::string>(*value_arg) : std::nullopt;
}

void InitParams::set_client_id(std::string_view value_arg) {
  client_id_ = value_arg;
}

const std::string* InitParams::server_client_id() const {
  return server_client_id_ ? &(*server_client_id_) : nullptr;
}

void InitParams::set_server_client_id(const std::string_view* value_arg) {
  server_client_id_ =
      value_arg ? std::optional<std::string>(*value_arg) : std::nullopt;
}

void InitParams::set_server_client_id(std::string_view value_arg) {
  server_client_id_ = value_arg;
}

bool InitParams::force_code_for_refresh_token() const {
  return force_code_for_refresh_token_;
}

void InitParams::set_force_code_for_refresh_token(bool value_arg) {
  force_code_for_refresh_token_ = value_arg;
}

EncodableList InitParams::ToEncodableList() const {
  EncodableList list;
  list.reserve(6);
  list.emplace_back(scopes_);
  list.emplace_back(CustomEncodableValue(sign_in_type_));
  list.push_back(hosted_domain_ ? EncodableValue(*hosted_domain_)
                                : EncodableValue());
  list.push_back(client_id_ ? EncodableValue(*client_id_) : EncodableValue());
  list.push_back(server_client_id_ ? EncodableValue(*server_client_id_)
                                   : EncodableValue());
  list.emplace_back(force_code_for_refresh_token_);
  return list;
}

InitParams InitParams::FromEncodableList(const EncodableList& list) {
  InitParams decoded(
      std::get<EncodableList>(list[0]),
      std::any_cast<const SignInType&>(std::get<CustomEncodableValue>(list[1])),
      std::get<bool>(list[5]));
  auto& encodable_hosted_domain = list[2];
  if (!encodable_hosted_domain.IsNull()) {
    decoded.set_hosted_domain(std::get<std::string>(encodable_hosted_domain));
  }
  auto& encodable_client_id = list[3];
  if (!encodable_client_id.IsNull()) {
    decoded.set_client_id(std::get<std::string>(encodable_client_id));
  }
  auto& encodable_server_client_id = list[4];
  if (!encodable_server_client_id.IsNull()) {
    decoded.set_server_client_id(
        std::get<std::string>(encodable_server_client_id));
  }
  return decoded;
}

// UserData

UserData::UserData(std::string email, std::string id)
    : email_(std::move(email)), id_(std::move(id)) {}

UserData::UserData(const std::string* display_name,
                   std::string email,
                   std::string id,
                   const std::string* photo_url,
                   const std::string* id_token,
                   const std::string* server_auth_code)
    : display_name_(display_name ? std::optional(*display_name) : std::nullopt),
      email_(std::move(email)),
      id_(std::move(id)),
      photo_url_(photo_url ? std::optional(*photo_url) : std::nullopt),
      id_token_(id_token ? std::optional(*id_token) : std::nullopt),
      server_auth_code_(server_auth_code
                            ? std::optional<std::string>(*server_auth_code)
                            : std::nullopt) {}

const std::string* UserData::display_name() const {
  return display_name_ ? &(*display_name_) : nullptr;
}

void UserData::set_display_name(const std::string_view* value_arg) {
  display_name_ =
      value_arg ? std::optional<std::string>(*value_arg) : std::nullopt;
}

void UserData::set_display_name(std::string_view value_arg) {
  display_name_ = value_arg;
}

const std::string& UserData::email() const {
  return email_;
}

void UserData::set_email(std::string_view value_arg) {
  email_ = value_arg;
}

const std::string& UserData::id() const {
  return id_;
}

void UserData::set_id(const std::string_view value_arg) {
  id_ = value_arg;
}

const std::string* UserData::photo_url() const {
  return photo_url_ ? &(*photo_url_) : nullptr;
}

void UserData::set_photo_url(const std::string_view* value_arg) {
  photo_url_ =
      value_arg ? std::optional<std::string>(*value_arg) : std::nullopt;
}

void UserData::set_photo_url(std::string_view value_arg) {
  photo_url_ = value_arg;
}

const std::string* UserData::id_token() const {
  return id_token_ ? &(*id_token_) : nullptr;
}

void UserData::set_id_token(const std::string_view* value_arg) {
  id_token_ = value_arg ? std::optional<std::string>(*value_arg) : std::nullopt;
}

void UserData::set_id_token(std::string_view value_arg) {
  id_token_ = value_arg;
}

const std::string* UserData::server_auth_code() const {
  return server_auth_code_ ? &(*server_auth_code_) : nullptr;
}

void UserData::set_server_auth_code(const std::string_view* value_arg) {
  server_auth_code_ =
      value_arg ? std::optional<std::string>(*value_arg) : std::nullopt;
}

void UserData::set_server_auth_code(std::string_view value_arg) {
  server_auth_code_ = value_arg;
}

EncodableList UserData::ToEncodableList() const {
  EncodableList list;
  list.reserve(6);
  list.push_back(display_name_ ? EncodableValue(*display_name_)
                               : EncodableValue());
  list.emplace_back(email_);
  list.emplace_back(id_);
  list.push_back(photo_url_ ? EncodableValue(*photo_url_) : EncodableValue());
  list.push_back(id_token_ ? EncodableValue(*id_token_) : EncodableValue());
  list.push_back(server_auth_code_ ? EncodableValue(*server_auth_code_)
                                   : EncodableValue());
  return list;
}

UserData UserData::FromEncodableList(const EncodableList& list) {
  UserData decoded(std::get<std::string>(list[1]),
                   std::get<std::string>(list[2]));
  auto& encodable_display_name = list[0];
  if (!encodable_display_name.IsNull()) {
    decoded.set_display_name(std::get<std::string>(encodable_display_name));
  }
  auto& encodable_photo_url = list[3];
  if (!encodable_photo_url.IsNull()) {
    decoded.set_photo_url(std::get<std::string>(encodable_photo_url));
  }
  auto& encodable_id_token = list[4];
  if (!encodable_id_token.IsNull()) {
    decoded.set_id_token(std::get<std::string>(encodable_id_token));
  }
  auto& encodable_server_auth_code = list[5];
  if (!encodable_server_auth_code.IsNull()) {
    decoded.set_server_auth_code(
        std::get<std::string>(encodable_server_auth_code));
  }
  return decoded;
}

PigeonInternalCodecSerializer::PigeonInternalCodecSerializer() = default;

EncodableValue PigeonInternalCodecSerializer::ReadValueOfType(
    const uint8_t type,
    flutter::ByteStreamReader* stream) const {
  switch (type) {
    case 129: {
      const auto& encodable_enum_arg = ReadValue(stream);
      const int64_t enum_arg_value =
          encodable_enum_arg.IsNull() ? 0 : encodable_enum_arg.LongValue();
      return encodable_enum_arg.IsNull()
                 ? EncodableValue()
                 : CustomEncodableValue(
                       static_cast<SignInType>(enum_arg_value));
    }
    case 130: {
      return CustomEncodableValue(InitParams::FromEncodableList(
          std::get<EncodableList>(ReadValue(stream))));
    }
    case 131: {
      return CustomEncodableValue(UserData::FromEncodableList(
          std::get<EncodableList>(ReadValue(stream))));
    }
    default:
      return flutter::StandardCodecSerializer::ReadValueOfType(type, stream);
  }
}

void PigeonInternalCodecSerializer::WriteValue(
    const EncodableValue& value,
    flutter::ByteStreamWriter* stream) const {
  if (const CustomEncodableValue* custom_value =
          std::get_if<CustomEncodableValue>(&value)) {
    if (custom_value->type() == typeid(SignInType)) {
      stream->WriteByte(129);
      WriteValue(EncodableValue(static_cast<int>(
                     std::any_cast<SignInType>(*custom_value))),
                 stream);
      return;
    }
    if (custom_value->type() == typeid(InitParams)) {
      stream->WriteByte(130);
      WriteValue(
          EncodableValue(
              std::any_cast<InitParams>(*custom_value).ToEncodableList()),
          stream);
      return;
    }
    if (custom_value->type() == typeid(UserData)) {
      stream->WriteByte(131);
      WriteValue(EncodableValue(
                     std::any_cast<UserData>(*custom_value).ToEncodableList()),
                 stream);
      return;
    }
  }
  flutter::StandardCodecSerializer::WriteValue(value, stream);
}

/// The codec used by GoogleSignInApi.
const flutter::StandardMessageCodec& GoogleSignInApi::GetCodec() {
  return flutter::StandardMessageCodec::GetInstance(
      &PigeonInternalCodecSerializer::GetInstance());
}

// Sets up an instance of `GoogleSignInApi` to handle messages through the
// `binary_messenger`.
void GoogleSignInApi::SetUp(flutter::BinaryMessenger* binary_messenger,
                            GoogleSignInApi* api) {
  GoogleSignInApi::SetUp(binary_messenger, api, "");
}

void GoogleSignInApi::SetUp(flutter::BinaryMessenger* binary_messenger,
                            GoogleSignInApi* api,
                            const std::string& message_channel_suffix) {
  const std::string prepended_suffix =
      !message_channel_suffix.empty()
          ? std::string(".") + message_channel_suffix
          : "";
  {
    BasicMessageChannel<> channel(
        binary_messenger,
        "dev.flutter.pigeon.google_sign_in_android.GoogleSignInApi.init" +
            prepended_suffix,
        &GetCodec());
    if (api != nullptr) {
      channel.SetMessageHandler(
          [api](const EncodableValue& message,
                const flutter::MessageReply<EncodableValue>& reply) {
            try {
              const auto& args = std::get<EncodableList>(message);
              const auto& encodable_params_arg = args.at(0);
              if (encodable_params_arg.IsNull()) {
                reply(WrapError("params_arg unexpectedly null."));
                return;
              }
              const auto& params_arg = std::any_cast<const InitParams&>(
                  std::get<CustomEncodableValue>(encodable_params_arg));
              std::optional<FlutterError> output = api->Init(params_arg);
              if (output.has_value()) {
                reply(WrapError(output.value()));
                return;
              }
              EncodableList wrapped;
              wrapped.emplace_back();
              reply(EncodableValue(std::move(wrapped)));
            } catch (const std::exception& exception) {
              reply(WrapError(exception.what()));
            }
          });
    } else {
      channel.SetMessageHandler(nullptr);
    }
  }
  {
    BasicMessageChannel<> channel(binary_messenger,
                                  "dev.flutter.pigeon.google_sign_in_android."
                                  "GoogleSignInApi.signInSilently" +
                                      prepended_suffix,
                                  &GetCodec());
    if (api != nullptr) {
      channel.SetMessageHandler(
          [api](const EncodableValue& /* message */,
                const flutter::MessageReply<EncodableValue>& reply) {
            try {
              api->SignInSilently([reply](ErrorOr<UserData>&& output) {
                if (output.has_error()) {
                  reply(WrapError(output.error()));
                  return;
                }
                EncodableList wrapped;
                wrapped.emplace_back(
                    CustomEncodableValue(std::move(output).TakeValue()));
                reply(EncodableValue(std::move(wrapped)));
              });
            } catch (const std::exception& exception) {
              reply(WrapError(exception.what()));
            }
          });
    } else {
      channel.SetMessageHandler(nullptr);
    }
  }
  {
    BasicMessageChannel<> channel(
        binary_messenger,
        "dev.flutter.pigeon.google_sign_in_android.GoogleSignInApi.signIn" +
            prepended_suffix,
        &GetCodec());
    if (api != nullptr) {
      channel.SetMessageHandler(
          [api](const EncodableValue& /* message */,
                const flutter::MessageReply<EncodableValue>& reply) {
            try {
              api->SignIn([reply](ErrorOr<UserData>&& output) {
                if (output.has_error()) {
                  reply(WrapError(output.error()));
                  return;
                }
                EncodableList wrapped;
                wrapped.emplace_back(
                    CustomEncodableValue(std::move(output).TakeValue()));
                reply(EncodableValue(std::move(wrapped)));
              });
            } catch (const std::exception& exception) {
              reply(WrapError(exception.what()));
            }
          });
    } else {
      channel.SetMessageHandler(nullptr);
    }
  }
  {
    BasicMessageChannel<> channel(binary_messenger,
                                  "dev.flutter.pigeon.google_sign_in_android."
                                  "GoogleSignInApi.getAccessToken" +
                                      prepended_suffix,
                                  &GetCodec());
    if (api != nullptr) {
      channel.SetMessageHandler(
          [api](const EncodableValue& message,
                const flutter::MessageReply<EncodableValue>& reply) {
            try {
              const auto& args = std::get<EncodableList>(message);
              const auto& encodable_email_arg = args.at(0);
              if (encodable_email_arg.IsNull()) {
                reply(WrapError("email_arg unexpectedly null."));
                return;
              }
              const auto& email_arg =
                  std::get<std::string>(encodable_email_arg);
              const auto& encodable_should_recover_auth_arg = args.at(1);
              if (encodable_should_recover_auth_arg.IsNull()) {
                reply(WrapError("should_recover_auth_arg unexpectedly null."));
                return;
              }
              const auto& should_recover_auth_arg =
                  std::get<bool>(encodable_should_recover_auth_arg);
              api->GetAccessToken(
                  email_arg, should_recover_auth_arg,
                  [reply](ErrorOr<std::string>&& output) {
                    if (output.has_error()) {
                      reply(WrapError(output.error()));
                      return;
                    }
                    EncodableList wrapped;
                    wrapped.emplace_back(std::move(output).TakeValue());
                    reply(EncodableValue(std::move(wrapped)));
                  });
            } catch (const std::exception& exception) {
              reply(WrapError(exception.what()));
            }
          });
    } else {
      channel.SetMessageHandler(nullptr);
    }
  }
  {
    BasicMessageChannel<> channel(
        binary_messenger,
        "dev.flutter.pigeon.google_sign_in_android.GoogleSignInApi.signOut" +
            prepended_suffix,
        &GetCodec());
    if (api != nullptr) {
      channel.SetMessageHandler(
          [api](const EncodableValue& /* message */,
                const flutter::MessageReply<EncodableValue>& reply) {
            try {
              api->SignOut([reply](std::optional<FlutterError>&& output) {
                if (output.has_value()) {
                  reply(WrapError(output.value()));
                  return;
                }
                EncodableList wrapped;
                wrapped.emplace_back();
                reply(EncodableValue(std::move(wrapped)));
              });
            } catch (const std::exception& exception) {
              reply(WrapError(exception.what()));
            }
          });
    } else {
      channel.SetMessageHandler(nullptr);
    }
  }
  {
    BasicMessageChannel<> channel(
        binary_messenger,
        "dev.flutter.pigeon.google_sign_in_android.GoogleSignInApi.disconnect" +
            prepended_suffix,
        &GetCodec());
    if (api != nullptr) {
      channel.SetMessageHandler(
          [api](const EncodableValue& /* message */,
                const flutter::MessageReply<EncodableValue>& reply) {
            try {
              api->Disconnect([reply](std::optional<FlutterError>&& output) {
                if (output.has_value()) {
                  reply(WrapError(output.value()));
                  return;
                }
                EncodableList wrapped;
                wrapped.emplace_back();
                reply(EncodableValue(std::move(wrapped)));
              });
            } catch (const std::exception& exception) {
              reply(WrapError(exception.what()));
            }
          });
    } else {
      channel.SetMessageHandler(nullptr);
    }
  }
  {
    BasicMessageChannel<> channel(
        binary_messenger,
        "dev.flutter.pigeon.google_sign_in_android.GoogleSignInApi.isSignedIn" +
            prepended_suffix,
        &GetCodec());
    if (api != nullptr) {
      channel.SetMessageHandler(
          [api](const EncodableValue& /* message */,
                const flutter::MessageReply<EncodableValue>& reply) {
            try {
              ErrorOr<bool> output = api->IsSignedIn();
              if (output.has_error()) {
                reply(WrapError(output.error()));
                return;
              }
              EncodableList wrapped;
              wrapped.emplace_back(std::move(output).TakeValue());
              reply(EncodableValue(std::move(wrapped)));
            } catch (const std::exception& exception) {
              reply(WrapError(exception.what()));
            }
          });
    } else {
      channel.SetMessageHandler(nullptr);
    }
  }
  {
    BasicMessageChannel<> channel(binary_messenger,
                                  "dev.flutter.pigeon.google_sign_in_android."
                                  "GoogleSignInApi.clearAuthCache" +
                                      prepended_suffix,
                                  &GetCodec());
    if (api != nullptr) {
      channel.SetMessageHandler(
          [api](const EncodableValue& message,
                const flutter::MessageReply<EncodableValue>& reply) {
            try {
              const auto& args = std::get<EncodableList>(message);
              const auto& encodable_token_arg = args.at(0);
              if (encodable_token_arg.IsNull()) {
                reply(WrapError("token_arg unexpectedly null."));
                return;
              }
              const auto& token_arg =
                  std::get<std::string>(encodable_token_arg);
              api->ClearAuthCache(
                  token_arg, [reply](std::optional<FlutterError>&& output) {
                    if (output.has_value()) {
                      reply(WrapError(output.value()));
                      return;
                    }
                    EncodableList wrapped;
                    wrapped.emplace_back();
                    reply(EncodableValue(std::move(wrapped)));
                  });
            } catch (const std::exception& exception) {
              reply(WrapError(exception.what()));
            }
          });
    } else {
      channel.SetMessageHandler(nullptr);
    }
  }
  {
    BasicMessageChannel<> channel(binary_messenger,
                                  "dev.flutter.pigeon.google_sign_in_android."
                                  "GoogleSignInApi.requestScopes" +
                                      prepended_suffix,
                                  &GetCodec());
    if (api != nullptr) {
      channel.SetMessageHandler(
          [api](const EncodableValue& message,
                const flutter::MessageReply<EncodableValue>& reply) {
            try {
              const auto& args = std::get<EncodableList>(message);
              const auto& encodable_scopes_arg = args.at(0);
              if (encodable_scopes_arg.IsNull()) {
                reply(WrapError("scopes_arg unexpectedly null."));
                return;
              }
              const auto& scopes_arg =
                  std::get<EncodableList>(encodable_scopes_arg);
              api->RequestScopes(scopes_arg, [reply](ErrorOr<bool>&& output) {
                if (output.has_error()) {
                  reply(WrapError(output.error()));
                  return;
                }
                EncodableList wrapped;
                wrapped.emplace_back(std::move(output).TakeValue());
                reply(EncodableValue(std::move(wrapped)));
              });
            } catch (const std::exception& exception) {
              reply(WrapError(exception.what()));
            }
          });
    } else {
      channel.SetMessageHandler(nullptr);
    }
  }
}

EncodableValue GoogleSignInApi::WrapError(std::string_view error_message) {
  return EncodableValue(
      EncodableList{EncodableValue(std::string(error_message)),
                    EncodableValue("Error"), EncodableValue()});
}

EncodableValue GoogleSignInApi::WrapError(const FlutterError& error) {
  return EncodableValue(EncodableList{EncodableValue(error.code()),
                                      EncodableValue(error.message()),
                                      error.details()});
}

}  // namespace google_sign_in_plugin
