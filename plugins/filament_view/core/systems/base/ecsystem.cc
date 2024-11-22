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
#include "ecsystem.h"

#include <event_stream_handler_functions.h>
#include <functional>
#include <iostream>
#include <plugin_registrar.h>
#include <queue>
#include <standard_method_codec.h>
#include <unordered_map>
#include <vector>

#include <core/systems/messages/ecs_message.h>
#include <core/systems/messages/ecs_message_types.h>
#include <plugins/common/common.h>

namespace plugin_filament_view {

////////////////////////////////////////////////////////////////////////////
// Send a message to the system
void ECSystem::vSendMessage(const ECSMessage& msg) {
  SPDLOG_TRACE("[vSendMessage] Attempting to acquire messagesMutex");
  std::unique_lock lock(messagesMutex);
  SPDLOG_TRACE("[vSendMessage] messagesMutex acquired");
  messageQueue_.push(msg);
  SPDLOG_TRACE("[vSendMessage] Message pushed to queue. Queue size: {}",
               messageQueue_.size());
}

////////////////////////////////////////////////////////////////////////////
// Register a message handler for a specific message type
void ECSystem::vRegisterMessageHandler(ECSMessageType type,
                                       const ECSMessageHandler& handler) {
  SPDLOG_TRACE("[vRegisterMessageHandler] Attempting to acquire handlersMutex");
  std::unique_lock lock(handlersMutex);
  SPDLOG_TRACE("[vRegisterMessageHandler] handlersMutex acquired");
  handlers_[type].push_back(handler);
  SPDLOG_TRACE(
      "[vRegisterMessageHandler] Handler registered for message type {}",
      static_cast<int>(type));
}

////////////////////////////////////////////////////////////////////////////
// Unregister all handlers for a specific message type
void ECSystem::vUnregisterMessageHandler(ECSMessageType type) {
  SPDLOG_TRACE(
      "[vUnregisterMessageHandler] Attempting to acquire handlersMutex");
  std::unique_lock lock(handlersMutex);
  SPDLOG_TRACE("[vUnregisterMessageHandler] handlersMutex acquired");
  handlers_.erase(type);
  SPDLOG_TRACE(
      "[vUnregisterMessageHandler] Handlers unregistered for message type {}",
      static_cast<int>(type));
}

////////////////////////////////////////////////////////////////////////////
// Clear all message handlers
void ECSystem::vClearMessageHandlers() {
  SPDLOG_TRACE("[vClearMessageHandlers] Attempting to acquire handlersMutex");
  std::unique_lock lock(handlersMutex);
  SPDLOG_TRACE("[vClearMessageHandlers] handlersMutex acquired");
  handlers_.clear();
  SPDLOG_TRACE("[vClearMessageHandlers] All handlers cleared");
}

////////////////////////////////////////////////////////////////////////////
// Process incoming messages
void ECSystem::vProcessMessages() {
  SPDLOG_TRACE("[vProcessMessages] Attempting to acquire messagesMutex");
  std::queue<ECSMessage> messagesToProcess;

  {
    std::unique_lock lock(messagesMutex);
    SPDLOG_TRACE("[vProcessMessages] messagesMutex acquired");
    std::swap(messageQueue_, messagesToProcess);
    SPDLOG_TRACE(
        "[vProcessMessages] Swapped message queues. Messages to process: {}",
        messagesToProcess.size());
  }  // messagesMutex is unlocked here

  while (!messagesToProcess.empty()) {
    const ECSMessage& msg = messagesToProcess.front();
    SPDLOG_TRACE("[vProcessMessages] Processing message");
    vHandleMessage(msg);
    messagesToProcess.pop();
    SPDLOG_TRACE("[vProcessMessages] Message processed. Remaining messages: {}",
                 messagesToProcess.size());
  }

  SPDLOG_TRACE("[vProcessMessages] done");
}

////////////////////////////////////////////////////////////////////////////
// Handle a specific message type by invoking the registered handlers
void ECSystem::vHandleMessage(const ECSMessage& msg) {
  SPDLOG_TRACE("[vHandleMessage] Attempting to acquire handlersMutex");
  std::vector<ECSMessageHandler> handlersToInvoke;
  {
    std::unique_lock lock(handlersMutex);
    SPDLOG_TRACE("[vHandleMessage] handlersMutex acquired");
    for (const auto& [type, handlerList] : handlers_) {
      if (msg.hasData(type)) {
        SPDLOG_TRACE("[vHandleMessage] Message has data for type {}",
                     static_cast<int>(type));
        handlersToInvoke.insert(handlersToInvoke.end(), handlerList.begin(),
                                handlerList.end());
      }
    }
  }  // handlersMutex is unlocked here
  SPDLOG_TRACE("[vHandleMessage] Handlers to invoke: {}",
               handlersToInvoke.size());

  for (const auto& handler : handlersToInvoke) {
    SPDLOG_TRACE("[vHandleMessage] Invoking handler");
    try {
      handler(msg);
    } catch (const std::exception& e) {
      spdlog::error("[vHandleMessage] Exception in handler: {}", e.what());
    }
  }
  SPDLOG_TRACE("[vHandleMessage] Handlers invocation completed");
}

////////////////////////////////////////////////////////////////////////////////////
void ECSystem::vSendDataToEventChannel(const flutter::EncodableMap& oDataMap) const {
  if (!event_sink_ || !event_channel_) {
    return;
  }

  event_sink_->Success(flutter::EncodableValue(oDataMap));
}

////////////////////////////////////////////////////////////////////////////////////
void ECSystem::vSetupMessageChannels(
      flutter::PluginRegistrar* poPluginRegistrar,
      const std::string& szChannelName) {
  if(event_channel_ != nullptr) {
    return;
  }

#if 0
  animationInfoCallback_ = std::make_unique<flutter::MethodChannel<>>(
      plugin_registrar->messenger(), channel_name,
      &flutter::StandardMethodCodec::GetInstance());
#else

  spdlog::debug("Creating Event Channel {}::{}}", __FUNCTION__, szChannelName);

  event_channel_ = std::make_unique<flutter::EventChannel<>>(
      poPluginRegistrar->messenger(),
      /*std::string("flutter.io/videoPlayer/videoEvents") +
          std::to_string(m_texture_id),*/
      szChannelName, &flutter::StandardMethodCodec::GetInstance());

  event_channel_->SetStreamHandler(
      std::make_unique<flutter::StreamHandlerFunctions<>>(
          [&](const flutter::EncodableValue* /* arguments */,
                 std::unique_ptr<flutter::EventSink<>>&& events)
              -> std::unique_ptr<flutter::StreamHandlerError<>> {
            event_sink_ = std::move(events);
            return nullptr;
          },
          [&](const flutter::EncodableValue* /* arguments */)
              -> std::unique_ptr<flutter::StreamHandlerError<>> {
            event_sink_ = nullptr;
            return nullptr;
          }));

  spdlog::debug("Event Channel creation Complete for {}", szChannelName);

#endif
}


}  // namespace plugin_filament_view
