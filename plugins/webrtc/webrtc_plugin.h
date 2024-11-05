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

#ifndef FLUTTER_PLUGIN_WEBRTC_PLUGIN_H
#define FLUTTER_PLUGIN_WEBRTC_PLUGIN_H

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>

#include "flutter_common.h"
#include "flutter_webrtc.h"

#include "messages.g.h"

namespace flutter_webrtc_plugin {

using namespace libwebrtc;

class WebrtcPlugin final : public FlutterWebRTCPlugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrar* registrar);

  ~WebrtcPlugin() override {}

  BinaryMessenger* messenger() override { return messenger_; }

  TextureRegistrar* textures() override { return textures_; }

  // Disallow copy and assign.
  WebrtcPlugin(const WebrtcPlugin&) = delete;
  WebrtcPlugin& operator=(const WebrtcPlugin&) = delete;

 private:
  std::unique_ptr<MethodChannel> channel_;
  std::unique_ptr<FlutterWebRTC> webrtc_;
  BinaryMessenger* messenger_;
  TextureRegistrar* textures_;

  // Creates a plugin that communicates on the given channel.
  WebrtcPlugin(PluginRegistrar* registrar,
               std::unique_ptr<MethodChannel> channel);

  void HandleMethodCall(const MethodCall& method_call,
                        std::unique_ptr<MethodResult> result);
};
}  // namespace flutter_webrtc_plugin

#endif  // FLUTTER_PLUGIN_WEBRTC_PLUGIN_H
