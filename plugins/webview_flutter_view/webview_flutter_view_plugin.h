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

#ifndef FLUTTER_PLUGIN_WEBVIEW_FLUTTER_PLUGIN_H_
#define FLUTTER_PLUGIN_WEBVIEW_FLUTTER_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>

#include "messages.g.h"

#include <capi/cef_app_capi.h>
#include <cef_app.h>
#include <cef_client.h>
#include <cef_render_handler.h>

#include "flutter_desktop_engine_state.h"
#include "flutter_homescreen.h"
#include "platform_views/platform_view.h"
#include "view/flutter_view.h"
#include "wayland/display.h"

namespace plugin_webview_flutter {

class RenderHandler : public CefRenderHandler {
 public:
  RenderHandler();
  ~RenderHandler();

  // CefRenderHandler interface
 public:
  void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;

  void OnPaint(CefRefPtr<CefBrowser> browser,
               PaintElementType type,
               const RectList& dirtyRects,
               const void* buffer,
               int width,
               int height) override;

  void OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                          PaintElementType type,
                          const RectList& dirtyRects,
                          const CefAcceleratedPaintInfo& info) override;
  // CefBase interface
 public:
  IMPLEMENT_REFCOUNTING(RenderHandler);
};

// for manual render handler
class BrowserClient : public CefClient {
 public:
  BrowserClient(RenderHandler* renderHandler) : m_renderHandler(renderHandler) {
    ;
  }

  CefRefPtr<CefRenderHandler> GetRenderHandler() override {
    return m_renderHandler;
  }

  CefRefPtr<CefRenderHandler> m_renderHandler;

  IMPLEMENT_REFCOUNTING(BrowserClient);
};

class WebviewPlatformView final : public PlatformView {
 public:
  WebviewPlatformView(int32_t id,
                      std::string viewType,
                      int32_t direction,
                      double top,
                      double left,
                      double width,
                      double height,
                      const std::vector<uint8_t>& params,
                      std::string assetDirectory,
                      FlutterDesktopEngineState* state,
                      PlatformViewAddListener addListener,
                      PlatformViewRemoveListener removeListener,
                      void* platform_view_context);

  ~WebviewPlatformView() override = default;

 private:
  MAYBE_UNUSED int32_t id_;
  void* platformViewsContext_;
  MAYBE_UNUSED PlatformViewRemoveListener removeListener_;
  const std::string flutterAssetsPath_;

  wl_display* display_;
  wl_surface* surface_;
  wl_surface* parent_surface_;
  wl_callback* callback_;
  wl_subsurface* subsurface_;

  static void on_frame(void* data, wl_callback* callback, uint32_t time);
  static const wl_callback_listener frame_listener;

  static void on_resize(double width, double height, void* data);
  static void on_set_direction(int32_t direction, void* data);
  static void on_set_offset(double left, double top, void* data);
  static void on_touch(int32_t action,
                       int32_t point_count,
                       size_t point_data_size,
                       const double* point_data,
                       void* data);
  static void on_dispose(bool hybrid, void* data);

  static const platform_view_listener platform_view_listener_;
};

class WebviewFlutterPlugin final : public flutter::Plugin,
                                   public InstanceManagerHostApi,
                                   public WebStorageHostApi,
                                   public WebViewHostApi,
                                   public WebSettingsHostApi,
                                   public WebChromeClientHostApi,
                                   public WebViewClientHostApi,
                                   public DownloadListenerHostApi,
                                   public JavaScriptChannelHostApi,
                                   public CookieManagerHostApi {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrar* registrar);

  static void PlatformViewCreate(int32_t id,
                                 std::string viewType,
                                 int32_t direction,
                                 double top,
                                 double left,
                                 double width,
                                 double height,
                                 const std::vector<uint8_t>& params,
                                 std::string assetDirectory,
                                 FlutterDesktopEngineRef engine,
                                 PlatformViewAddListener addListener,
                                 PlatformViewRemoveListener removeListener,
                                 void* platform_view_context);

  WebviewFlutterPlugin();

  ~WebviewFlutterPlugin() override;

 private:
  _cef_browser_t* browser_;
  _cef_client_t* browserClient_;
  std::unique_ptr<_cef_render_handler_t> renderHandler_;

 public:
  std::optional<FlutterError> Clear() override;

  std::optional<FlutterError> Create(int64_t instance_id) override;

  std::optional<FlutterError> DeleteAllData(int64_t instance_id) override;

  std::optional<FlutterError> LoadData(int64_t instance_id,
                                       const std::string& data,
                                       const std::string* mime_type,
                                       const std::string* encoding) override;

  std::optional<FlutterError> LoadDataWithBaseUrl(
      int64_t instance_id,
      const std::string* base_url,
      const std::string& data,
      const std::string* mime_type,
      const std::string* encoding,
      const std::string* history_url) override;

  std::optional<FlutterError> LoadUrl(
      int64_t instance_id,
      const std::string& url,
      const flutter::EncodableMap& headers) override;

  std::optional<FlutterError> PostUrl(
      int64_t instance_id,
      const std::string& url,
      const std::vector<uint8_t>& data) override;

  ErrorOr<std::optional<std::string>> GetUrl(int64_t instance_id) override;

  ErrorOr<bool> CanGoBack(int64_t instance_id) override;

  ErrorOr<bool> CanGoForward(int64_t instance_id) override;

  std::optional<FlutterError> GoBack(int64_t instance_id) override;

  std::optional<FlutterError> GoForward(int64_t instance_id) override;

  std::optional<FlutterError> Reload(int64_t instance_id) override;

  std::optional<FlutterError> ClearCache(int64_t instance_id,
                                         bool include_disk_files) override;

  void EvaluateJavascript(
      int64_t instance_id,
      const std::string& javascript_string,
      std::function<void(ErrorOr<std::optional<std::string>> reply)> result)
      override;

  std::optional<FlutterError> Create(int64_t instance_id,
                                     const std::string& channel_name) override;

  ErrorOr<std::optional<std::string>> GetTitle(int64_t instance_id) override;

  std::optional<FlutterError> ScrollTo(int64_t instance_id,
                                       int64_t x,
                                       int64_t y) override;

  std::optional<FlutterError> ScrollBy(int64_t instance_id,
                                       int64_t x,
                                       int64_t y) override;

  ErrorOr<int64_t> GetScrollX(int64_t instance_id) override;

  ErrorOr<int64_t> GetScrollY(int64_t instance_id) override;

  ErrorOr<WebViewPoint> GetScrollPosition(int64_t instance_id) override;

  std::optional<FlutterError> SetWebContentsDebuggingEnabled(
      bool enabled) override;

  std::optional<FlutterError> SetWebViewClient(
      int64_t instance_id,
      int64_t web_view_client_instance_id) override;

  std::optional<FlutterError> AddJavaScriptChannel(
      int64_t instance_id,
      int64_t java_script_channel_instance_id) override;

  std::optional<FlutterError> RemoveJavaScriptChannel(
      int64_t instance_id,
      int64_t java_script_channel_instance_id) override;

  std::optional<FlutterError> SetDownloadListener(
      int64_t instance_id,
      const int64_t* listener_instance_id) override;

  std::optional<FlutterError> SetWebChromeClient(
      int64_t instance_id,
      const int64_t* client_instance_id) override;

  std::optional<FlutterError> SetBackgroundColor(int64_t instance_id,
                                                 int64_t color) override;

  std::optional<FlutterError> Create(int64_t instance_id,
                                     int64_t web_view_instance_id) override;

  std::optional<FlutterError> SetDomStorageEnabled(int64_t instance_id,
                                                   bool flag) override;
  std::optional<FlutterError> SetJavaScriptCanOpenWindowsAutomatically(
      int64_t instance_id,
      bool flag) override;
  std::optional<FlutterError> SetSupportMultipleWindows(int64_t instance_id,
                                                        bool support) override;
  std::optional<FlutterError> SetJavaScriptEnabled(int64_t instance_id,
                                                   bool flag) override;
  std::optional<FlutterError> SetUserAgentString(
      int64_t instance_id,
      const std::string* user_agent_string) override;
  std::optional<FlutterError> SetMediaPlaybackRequiresUserGesture(
      int64_t instance_id,
      bool require) override;
  std::optional<FlutterError> SetSupportZoom(int64_t instance_id,
                                             bool support) override;
  std::optional<FlutterError> SetLoadWithOverviewMode(int64_t instance_id,
                                                      bool overview) override;
  std::optional<FlutterError> SetUseWideViewPort(int64_t instance_id,
                                                 bool use) override;
  std::optional<FlutterError> SetDisplayZoomControls(int64_t instance_id,
                                                     bool enabled) override;
  std::optional<FlutterError> SetBuiltInZoomControls(int64_t instance_id,
                                                     bool enabled) override;
  std::optional<FlutterError> SetAllowFileAccess(int64_t instance_id,
                                                 bool enabled) override;
  std::optional<FlutterError> SetTextZoom(int64_t instance_id,
                                          int64_t text_zoom) override;
  ErrorOr<std::string> GetUserAgentString(int64_t instance_id) override;

  std::optional<FlutterError> SetSynchronousReturnValueForOnShowFileChooser(
      int64_t instance_id,
      bool value) override;
  std::optional<FlutterError> SetSynchronousReturnValueForOnConsoleMessage(
      int64_t instance_id,
      bool value) override;

  std::optional<FlutterError>
  SetSynchronousReturnValueForShouldOverrideUrlLoading(int64_t instance_id,
                                                       bool value) override;

  std::optional<FlutterError> SetSynchronousReturnValueForOnJsAlert(
      int64_t instance_id,
      bool value) override;
  std::optional<FlutterError> SetSynchronousReturnValueForOnJsConfirm(
      int64_t instance_id,
      bool value) override;
  std::optional<FlutterError> SetSynchronousReturnValueForOnJsPrompt(
      int64_t instance_id,
      bool value) override;

  std::optional<FlutterError> AttachInstance(
      int64_t instance_identifier) override;

  std::optional<FlutterError> SetCookie(int64_t identifier,
                                        const std::string& url,
                                        const std::string& value) override;

  void RemoveAllCookies(
      int64_t identifier,
      std::function<void(ErrorOr<bool> reply)> result) override;

  std::optional<FlutterError> SetAcceptThirdPartyCookies(
      int64_t identifier,
      int64_t web_view_identifier,
      bool accept) override;

  // Disallow copy and assign.
  WebviewFlutterPlugin(const WebviewFlutterPlugin&) = delete;
  WebviewFlutterPlugin& operator=(const WebviewFlutterPlugin&) = delete;
};

}  // namespace plugin_webview_flutter

#endif  // FLUTTER_PLUGIN_WEBVIEW_FLUTTER_PLUGIN_H_
