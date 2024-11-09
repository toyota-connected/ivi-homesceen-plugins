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

#ifndef FLUTTER_PLUGIN_GOOGLE_SIGN_IN_PLUGIN_H_
#define FLUTTER_PLUGIN_GOOGLE_SIGN_IN_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>

#include "messages.g.h"

namespace google_sign_in_plugin {

static constexpr auto kPeopleUrl =
    "https://people.googleapis.com/v1/people/"
    "me?personFields=photos,names,emailAddresses";

// Key Constants
static constexpr auto kKeyAccessToken = "access_token";
static constexpr auto kKeyAuthCode = "auth_code";
static constexpr auto kKeyAuthProviderX509CertUrl =
    "auth_provider_x509_cert_url";
static constexpr auto kKeyAuthUri = "auth_uri";
static constexpr auto kKeyClientId = "client_id";
static constexpr auto kKeyClientSecret = "client_secret";
static constexpr auto kKeyCode = "code";
static constexpr auto kKeyExpiresAt = "expires_at";
static constexpr auto kKeyExpiresIn = "expires_in";
static constexpr auto kKeyGrantType = "grant_type";
static constexpr auto kKeyIdToken = "id_token";
static constexpr auto kKeyInstalled = "installed";
static constexpr auto kKeyProjectId = "project_id";
static constexpr auto kKeyRefreshToken = "refresh_token";
static constexpr auto kKeyRedirectUri = "redirect_uri";
static constexpr auto kKeyRedirectUris = "redirect_uris";

static constexpr auto kKeyScope = "scope";
static constexpr auto kKeyTokenType = "token_type";
static constexpr auto kKeyTokenUri = "token_uri";

/// Value Constants
static constexpr auto kValueAuthorizationCode = "authorization_code";
static constexpr auto kValueRedirectUri = "urn:ietf:wg:oauth:2.0:oob";
static constexpr auto kValueRefreshToken = "refresh_token";

/// People Response Constants
static constexpr auto kKeyDisplayName = "displayName";
static constexpr auto kKeyEmailAddresses = "emailAddresses";
static constexpr auto kKeyMetadata = "metadata";
static constexpr auto kKeyNames = "names";
static constexpr auto kKeyPhotos = "photos";
static constexpr auto kKeyPrimary = "primary";
static constexpr auto kKeyResourceName = "resourceName";
static constexpr auto kKeySourcePrimary = "sourcePrimary";
static constexpr auto kKeyUrl = "url";
static constexpr auto kKeyValue = "value";

static constexpr auto kClientCredentialsPathEnvironmentVariable =
    "GOOGLE_API_OAUTH2_CLIENT_CREDENTIALS";
static constexpr auto kClientSecretPathEnvironmentVariable =
    "GOOGLE_API_OAUTH2_CLIENT_SECRET_JSON";

class GoogleSignInPlugin final : public flutter::Plugin,
                                 public GoogleSignInApi {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrar* registrar);

  GoogleSignInPlugin() = default;

  ~GoogleSignInPlugin() override = default;

  // Initializes a sign in request with the given parameters.
  std::optional<FlutterError> Init(const InitParams& params) override;

  // Starts a silent sign in.
  void SignInSilently(
      std::function<void(ErrorOr<UserData> reply)> result) override;

  // Starts a sign in with user interaction.
  void SignIn(std::function<void(ErrorOr<UserData> reply)> result) override;

  // Requests the access token for the current sign in.
  void GetAccessToken(
      const std::string& email,
      bool should_recover_auth,
      std::function<void(ErrorOr<std::string> reply)> result) override;

  // Signs out the current user.
  void SignOut(
      std::function<void(std::optional<FlutterError> reply)> result) override;

  // Revokes scope grants to the application.
  void Disconnect(
      std::function<void(std::optional<FlutterError> reply)> result) override;

  // Returns whether the user is currently signed in.
  ErrorOr<bool> IsSignedIn() override;

  // Clears the authentication caching for the given token, requiring a
  // new sign in.
  void ClearAuthCache(
      const std::string& token,
      std::function<void(std::optional<FlutterError> reply)> result) override;

  // Requests access to the given scopes.
  void RequestScopes(const flutter::EncodableList& scopes,
                     std::function<void(ErrorOr<bool> reply)> result) override;

  // Disallow copy and assign.
  GoogleSignInPlugin(const GoogleSignInPlugin&) = delete;
  GoogleSignInPlugin& operator=(const GoogleSignInPlugin&) = delete;

 private:
  // Method Response Constants
  static constexpr auto kMethodResponseKeyAccessToken = "accessToken";
  static constexpr auto kMethodResponseKeyEmail = "email";
  static constexpr auto kMethodResponseKeyId = "id";
  static constexpr auto kMethodResponseKeyIdToken = "idToken";
  static constexpr auto kMethodResponseKeyPhotoUrl = "photoUrl";
  static constexpr auto kMethodResponseKeyServerAuthCode = "serverAuthCode";
};
}  // namespace google_sign_in_plugin

#endif  // FLUTTER_PLUGIN_GOOGLE_SIGN_IN_PLUGIN_H_