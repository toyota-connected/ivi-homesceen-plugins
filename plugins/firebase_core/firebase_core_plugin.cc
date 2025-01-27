// Copyright 2023, the Chromium project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.
// Copyright 2023, Toyota Connected North America

#include "firebase_core_plugin.h"

#include "firebase/app.h"
#include "firebase_core/plugin_version.h"
#include "messages.g.h"

#include <flutter/plugin_registrar.h>
#include <flutter/standard_method_codec.h>

#include <firebase/app.h>
#include <memory>
#include <string>
#include <vector>

using ::firebase::App;

namespace firebase_core_linux {

static std::string kLibraryName = "flutter-fire-core";

// static
void FirebaseCorePlugin::RegisterWithRegistrar(
    flutter::PluginRegistrar* registrar) {
  auto plugin = std::make_unique<FirebaseCorePlugin>();

  FirebaseCoreHostApi::SetUp(registrar->messenger(), plugin.get());
  FirebaseAppHostApi::SetUp(registrar->messenger(), plugin.get());

  registrar->AddPlugin(std::move(plugin));

  // Register for platform logging
  App::RegisterLibrary(kLibraryName.c_str(), getPluginVersion().c_str(),
                       nullptr);
}

FirebaseCorePlugin::FirebaseCorePlugin() = default;

FirebaseCorePlugin::~FirebaseCorePlugin() = default;

// Convert a Pigeon FirebaseOptions to a Firebase Options.
firebase::AppOptions PigeonFirebaseOptionsToAppOptions(
    const PigeonFirebaseOptions& pigeon_options) {
  firebase::AppOptions options;
  options.set_api_key(pigeon_options.api_key().c_str());
  options.set_app_id(pigeon_options.app_id().c_str());
  if (pigeon_options.database_u_r_l() != nullptr) {
    options.set_database_url(pigeon_options.database_u_r_l()->c_str());
  }
  if (pigeon_options.tracking_id() != nullptr) {
    options.set_ga_tracking_id(pigeon_options.tracking_id()->c_str());
  }
  options.set_messaging_sender_id(pigeon_options.messaging_sender_id().c_str());

  options.set_project_id(pigeon_options.project_id().c_str());

  if (pigeon_options.storage_bucket() != nullptr) {
    options.set_storage_bucket(pigeon_options.storage_bucket()->c_str());
  }
  return options;
}

// Convert a AppOptions to PigeonInitializeOption
PigeonFirebaseOptions optionsFromFIROptions(
    const firebase::AppOptions& options) {
  PigeonFirebaseOptions pigeon_options = PigeonFirebaseOptions();
  pigeon_options.set_api_key(options.api_key());
  pigeon_options.set_app_id(options.app_id());
  if (options.database_url() != nullptr) {
    pigeon_options.set_database_u_r_l(options.database_url());
  }
  pigeon_options.set_tracking_id(nullptr);
  pigeon_options.set_messaging_sender_id(options.messaging_sender_id());
  pigeon_options.set_project_id(options.project_id());
  if (options.storage_bucket() != nullptr) {
    pigeon_options.set_storage_bucket(options.storage_bucket());
  }
  return pigeon_options;
}

// Convert a firebase::App to PigeonInitializeResponse
PigeonInitializeResponse AppToPigeonInitializeResponse(const App& app) {
  PigeonInitializeResponse response = PigeonInitializeResponse();
  response.set_name(app.name());
  response.set_options(optionsFromFIROptions(app.options()));
  return response;
}

void FirebaseCorePlugin::InitializeApp(
    const std::string& app_name,
    const PigeonFirebaseOptions& initialize_app_request,
    std::function<void(ErrorOr<PigeonInitializeResponse> reply)> result) {
  // Create an app
  App* app =
      App::Create(PigeonFirebaseOptionsToAppOptions(initialize_app_request),
                  app_name.c_str());

  // Send back the result to Flutter
  result(AppToPigeonInitializeResponse(*app));
}

void FirebaseCorePlugin::InitializeCore(
    std::function<void(ErrorOr<flutter::EncodableList> reply)> result) {
  // TODO: Missing function to get the list of currently initialized apps
  std::vector<PigeonInitializeResponse> initializedApps;
  std::vector<App*> all_apps = App::GetApps();
  initializedApps.reserve(all_apps.size());
  for (const App* app : all_apps) {
    initializedApps.push_back(AppToPigeonInitializeResponse(*app));
  }

  flutter::EncodableList encodableList;

  for (const auto& item : initializedApps) {
    encodableList.emplace_back(flutter::CustomEncodableValue(item));
  }
  result(encodableList);
}

void FirebaseCorePlugin::OptionsFromResource(
    std::function<void(ErrorOr<PigeonFirebaseOptions> reply)> /* result */) {}

void FirebaseCorePlugin::SetAutomaticDataCollectionEnabled(
    const std::string& app_name,
    bool /* enabled */,
    std::function<void(std::optional<FlutterError> reply)> result) {
  App* firebaseApp = App::GetInstance(app_name.c_str());
  if (firebaseApp != nullptr) {
    // TODO: Missing method
  }
  result(std::nullopt);
}

void FirebaseCorePlugin::SetAutomaticResourceManagementEnabled(
    const std::string& app_name,
    bool /* enabled */,
    std::function<void(std::optional<FlutterError> reply)> result) {
  App* firebaseApp = App::GetInstance(app_name.c_str());
  if (firebaseApp != nullptr) {
    // TODO: Missing method
  }

  result(std::nullopt);
}

void FirebaseCorePlugin::Delete(
    const std::string& app_name,
    std::function<void(std::optional<FlutterError> reply)> result) {
  App* firebaseApp = App::GetInstance(app_name.c_str());
  if (firebaseApp != nullptr) {
    // TODO: Missing method
  }

  result(std::nullopt);
}

}  // namespace firebase_core_linux
