// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Autogenerated from Pigeon (v21.2.0), do not edit directly.
// See also: https://pub.dev/packages/pigeon

#undef _HAS_EXCEPTIONS

#include "messages.g.h"

#include <flutter/basic_message_channel.h>
#include <flutter/binary_messenger.h>
#include <flutter/encodable_value.h>
#include <flutter/standard_message_codec.h>

#include <optional>
#include <string>

namespace url_launcher_linux {
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

PigeonInternalCodecSerializer::PigeonInternalCodecSerializer() = default;

EncodableValue PigeonInternalCodecSerializer::ReadValueOfType(
    const uint8_t type,
    flutter::ByteStreamReader* stream) const {
  return flutter::StandardCodecSerializer::ReadValueOfType(type, stream);
}

void PigeonInternalCodecSerializer::WriteValue(
    const EncodableValue& value,
    flutter::ByteStreamWriter* stream) const {
  flutter::StandardCodecSerializer::WriteValue(value, stream);
}

/// The codec used by UrlLauncherApi.
const flutter::StandardMessageCodec& UrlLauncherApi::GetCodec() {
  return flutter::StandardMessageCodec::GetInstance(
      &PigeonInternalCodecSerializer::GetInstance());
}

// Sets up an instance of `UrlLauncherApi` to handle messages through the
// `binary_messenger`.
void UrlLauncherApi::SetUp(flutter::BinaryMessenger* binary_messenger,
                           UrlLauncherApi* api) {
  UrlLauncherApi::SetUp(binary_messenger, api, "");
}

void UrlLauncherApi::SetUp(flutter::BinaryMessenger* binary_messenger,
                           UrlLauncherApi* api,
                           const std::string& message_channel_suffix) {
  const std::string prepended_suffix =
      !message_channel_suffix.empty()
          ? std::string(".") + message_channel_suffix
          : "";
  {
    BasicMessageChannel<> channel(
        binary_messenger,
        "dev.flutter.pigeon.url_launcher_linux.UrlLauncherApi.canLaunchUrl" +
            prepended_suffix,
        &GetCodec());
    if (api != nullptr) {
      channel.SetMessageHandler(
          [api](const EncodableValue& message,
                const flutter::MessageReply<EncodableValue>& reply) {
            try {
              const auto& args = std::get<EncodableList>(message);
              const auto& encodable_url_arg = args.at(0);
              if (encodable_url_arg.IsNull()) {
                reply(WrapError("url_arg unexpectedly null."));
                return;
              }
              const auto& url_arg = std::get<std::string>(encodable_url_arg);
              ErrorOr<bool> output = api->CanLaunchUrl(url_arg);
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
    BasicMessageChannel<> channel(
        binary_messenger,
        "dev.flutter.pigeon.url_launcher_linux.UrlLauncherApi.launchUrl" +
            prepended_suffix,
        &GetCodec());
    if (api != nullptr) {
      channel.SetMessageHandler(
          [api](const EncodableValue& message,
                const flutter::MessageReply<EncodableValue>& reply) {
            try {
              const auto& args = std::get<EncodableList>(message);
              const auto& encodable_url_arg = args.at(0);
              if (encodable_url_arg.IsNull()) {
                reply(WrapError("url_arg unexpectedly null."));
                return;
              }
              const auto& url_arg = std::get<std::string>(encodable_url_arg);
              ErrorOr<std::optional<std::string>> output =
                  api->LaunchUrl(url_arg);
              if (output.has_error()) {
                reply(WrapError(output.error()));
                return;
              }
              EncodableList wrapped;
              if (auto output_optional = std::move(output).TakeValue()) {
                wrapped.emplace_back(std::move(output_optional).value());
              } else {
                wrapped.emplace_back();
              }
              reply(EncodableValue(std::move(wrapped)));
            } catch (const std::exception& exception) {
              reply(WrapError(exception.what()));
            }
          });
    } else {
      channel.SetMessageHandler(nullptr);
    }
  }
}

EncodableValue UrlLauncherApi::WrapError(std::string_view error_message) {
  return EncodableValue(
      EncodableList{EncodableValue(std::string(error_message)),
                    EncodableValue("Error"), EncodableValue()});
}

EncodableValue UrlLauncherApi::WrapError(const FlutterError& error) {
  return EncodableValue(EncodableList{EncodableValue(error.code()),
                                      EncodableValue(error.message()),
                                      error.details()});
}

}  // namespace url_launcher_linux
