/*
 * Copyright 2020-2023 Toyota Connected North America
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

#include "webrtc_plugin.h"

namespace flutter_webrtc_plugin {

// static
void WebrtcPlugin::RegisterWithRegistrar(flutter::PluginRegistrar* registrar) {
  auto channel = std::make_unique<MethodChannel>(
      registrar->messenger(), "FlutterWebRTC.Method",
      &flutter::StandardMethodCodec::GetInstance());

  auto* channel_pointer = channel.get();

  // Uses new instead of make_unique due to private constructor.
  std::unique_ptr<WebrtcPlugin> plugin(
      new WebrtcPlugin(registrar, std::move(channel)));

  channel_pointer->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto& call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

WebrtcPlugin::WebrtcPlugin(PluginRegistrar* registrar,
                           std::unique_ptr<MethodChannel> channel)
    : channel_(std::move(channel)),
      messenger_(registrar->messenger()),
      textures_(registrar->texture_registrar()) {
  webrtc_ = std::make_unique<FlutterWebRTC>(this);
}

// Called when a method is called on |channel_|;
void WebrtcPlugin::HandleMethodCall(const MethodCall& method_call,
                                    std::unique_ptr<MethodResult> result) {
  // handle method call and forward to webrtc native sdk.
  auto method_call_proxy = MethodCallProxy::Create(method_call);
  webrtc_->HandleMethodCall(*method_call_proxy,
                            MethodResultProxy::Create(std::move(result)));
}

}  // namespace flutter_webrtc_plugin
