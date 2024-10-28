//
// Copyright 2024 Toyota Connected North America
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// Autogenerated from Pigeon (v22.6.0), do not edit directly.
// See also: https://pub.dev/packages/pigeon

#ifndef PIGEON_MESSAGES_G_H_
#define PIGEON_MESSAGES_G_H_
#include <flutter/basic_message_channel.h>
#include <flutter/binary_messenger.h>
#include <flutter/encodable_value.h>
#include <flutter/standard_message_codec.h>

#include <map>
#include <optional>
#include <string>

namespace flatpak_plugin {

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
  friend class FlatpakApi;
  ErrorOr() = default;
  T TakeValue() && { return std::get<T>(std::move(v_)); }

  std::variant<T, FlutterError> v_;
};

// Generated class from Pigeon that represents data sent in messages.
class Remote {
 public:
  // Constructs an object setting all fields.
  explicit Remote(std::string name,
                  std::string url,
                  std::string collection_id,
                  std::string title,
                  std::string comment,
                  std::string description,
                  std::string homepage,
                  std::string icon,
                  std::string default_branch,
                  std::string main_ref,
                  std::string remote_type,
                  std::string filter,
                  std::string appstream_timestamp,
                  std::string appstream_dir,
                  bool gpg_verify,
                  bool no_enumerate,
                  bool no_deps,
                  bool disabled,
                  int64_t prio);

  const std::string& name() const;
  void set_name(std::string_view value_arg);

  const std::string& url() const;
  void set_url(std::string_view value_arg);

  const std::string& collection_id() const;
  void set_collection_id(std::string_view value_arg);

  const std::string& title() const;
  void set_title(std::string_view value_arg);

  const std::string& comment() const;
  void set_comment(std::string_view value_arg);

  const std::string& description() const;
  void set_description(std::string_view value_arg);

  const std::string& homepage() const;
  void set_homepage(std::string_view value_arg);

  const std::string& icon() const;
  void set_icon(std::string_view value_arg);

  const std::string& default_branch() const;
  void set_default_branch(std::string_view value_arg);

  const std::string& main_ref() const;
  void set_main_ref(std::string_view value_arg);

  const std::string& remote_type() const;
  void set_remote_type(std::string_view value_arg);

  const std::string& filter() const;
  void set_filter(std::string_view value_arg);

  const std::string& appstream_timestamp() const;
  void set_appstream_timestamp(std::string_view value_arg);

  const std::string& appstream_dir() const;
  void set_appstream_dir(std::string_view value_arg);

  bool gpg_verify() const;
  void set_gpg_verify(bool value_arg);

  bool no_enumerate() const;
  void set_no_enumerate(bool value_arg);

  bool no_deps() const;
  void set_no_deps(bool value_arg);

  bool disabled() const;
  void set_disabled(bool value_arg);

  int64_t prio() const;
  void set_prio(int64_t value_arg);

 private:
  static Remote FromEncodableList(const flutter::EncodableList& list);
  flutter::EncodableList ToEncodableList() const;
  friend class FlatpakApi;
  friend class PigeonInternalCodecSerializer;
  std::string name_;
  std::string url_;
  std::string collection_id_;
  std::string title_;
  std::string comment_;
  std::string description_;
  std::string homepage_;
  std::string icon_;
  std::string default_branch_;
  std::string main_ref_;
  std::string remote_type_;
  std::string filter_;
  std::string appstream_timestamp_;
  std::string appstream_dir_;
  bool gpg_verify_;
  bool no_enumerate_;
  bool no_deps_;
  bool disabled_;
  int64_t prio_;
};

// Generated class from Pigeon that represents data sent in messages.
class Application {
 public:
  // Constructs an object setting all fields.
  explicit Application(std::string name,
                       std::string id,
                       std::string summary,
                       std::string version,
                       std::string origin,
                       std::string license,
                       int64_t installed_size,
                       std::string deploy_dir,
                       bool is_current,
                       std::string content_rating_type,
                       std::string latest_commit,
                       std::string eol,
                       std::string eol_rebase,
                       flutter::EncodableList subpaths);

  const std::string& name() const;
  void set_name(std::string_view value_arg);

  const std::string& id() const;
  void set_id(std::string_view value_arg);

  const std::string& summary() const;
  void set_summary(std::string_view value_arg);

  const std::string& version() const;
  void set_version(std::string_view value_arg);

  const std::string& origin() const;
  void set_origin(std::string_view value_arg);

  const std::string& license() const;
  void set_license(std::string_view value_arg);

  int64_t installed_size() const;
  void set_installed_size(int64_t value_arg);

  const std::string& deploy_dir() const;
  void set_deploy_dir(std::string_view value_arg);

  bool is_current() const;
  void set_is_current(bool value_arg);

  const std::string& content_rating_type() const;
  void set_content_rating_type(std::string_view value_arg);

  const std::string& latest_commit() const;
  void set_latest_commit(std::string_view value_arg);

  const std::string& eol() const;
  void set_eol(std::string_view value_arg);

  const std::string& eol_rebase() const;
  void set_eol_rebase(std::string_view value_arg);

  const flutter::EncodableList& subpaths() const;
  void set_subpaths(const flutter::EncodableList& value_arg);

 private:
  static Application FromEncodableList(const flutter::EncodableList& list);
  flutter::EncodableList ToEncodableList() const;
  friend class FlatpakApi;
  friend class PigeonInternalCodecSerializer;
  std::string name_;
  std::string id_;
  std::string summary_;
  std::string version_;
  std::string origin_;
  std::string license_;
  int64_t installed_size_;
  std::string deploy_dir_;
  bool is_current_;
  std::string content_rating_type_;
  std::string latest_commit_;
  std::string eol_;
  std::string eol_rebase_;
  flutter::EncodableList subpaths_;
};

// Generated class from Pigeon that represents data sent in messages.
class Installation {
 public:
  // Constructs an object setting all fields.
  explicit Installation(std::string id,
                        std::string display_name,
                        std::string path,
                        bool no_interaction,
                        bool is_user,
                        int64_t priority,
                        flutter::EncodableList default_lanaguages,
                        flutter::EncodableList default_locale,
                        flutter::EncodableList remotes);

  const std::string& id() const;
  void set_id(std::string_view value_arg);

  const std::string& display_name() const;
  void set_display_name(std::string_view value_arg);

  const std::string& path() const;
  void set_path(std::string_view value_arg);

  bool no_interaction() const;
  void set_no_interaction(bool value_arg);

  bool is_user() const;
  void set_is_user(bool value_arg);

  int64_t priority() const;
  void set_priority(int64_t value_arg);

  const flutter::EncodableList& default_lanaguages() const;
  void set_default_lanaguages(const flutter::EncodableList& value_arg);

  const flutter::EncodableList& default_locale() const;
  void set_default_locale(const flutter::EncodableList& value_arg);

  const flutter::EncodableList& remotes() const;
  void set_remotes(const flutter::EncodableList& value_arg);

 private:
  static Installation FromEncodableList(const flutter::EncodableList& list);
  flutter::EncodableList ToEncodableList() const;
  friend class FlatpakApi;
  friend class PigeonInternalCodecSerializer;
  std::string id_;
  std::string display_name_;
  std::string path_;
  bool no_interaction_;
  bool is_user_;
  int64_t priority_;
  flutter::EncodableList default_lanaguages_;
  flutter::EncodableList default_locale_;
  flutter::EncodableList remotes_;
};

class PigeonInternalCodecSerializer : public flutter::StandardCodecSerializer {
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
class FlatpakApi {
 public:
  FlatpakApi(const FlatpakApi&) = delete;
  FlatpakApi& operator=(const FlatpakApi&) = delete;
  virtual ~FlatpakApi() {}
  // Get Flatpak version.
  virtual ErrorOr<std::string> GetVersion() = 0;
  // Get the default flatpak arch
  virtual ErrorOr<std::string> GetDefaultArch() = 0;
  // Get all arches supported by flatpak
  virtual ErrorOr<flutter::EncodableList> GetSupportedArches() = 0;
  // Returns a list of Flatpak system installations.
  virtual ErrorOr<flutter::EncodableList> GetSystemInstallations() = 0;
  // Returns user flatpak installation.
  virtual ErrorOr<Installation> GetUserInstallation() = 0;
  // Add a remote repository.
  virtual ErrorOr<bool> RemoteAdd(const Remote& configuration) = 0;
  // Remove Remote configuration.
  virtual ErrorOr<bool> RemoteRemove(const std::string& id) = 0;
  // Get a list of applications installed on machine.
  virtual ErrorOr<flutter::EncodableList> GetApplicationsInstalled() = 0;
  // Get list of applications hosted on a remote.
  virtual ErrorOr<flutter::EncodableList> GetApplicationsRemote(
      const std::string& id) = 0;
  // Install application of given id.
  virtual ErrorOr<bool> ApplicationInstall(const std::string& id) = 0;
  // Uninstall application with specified id.
  virtual ErrorOr<bool> ApplicationUninstall(const std::string& id) = 0;
  // Start application using specified configuration.
  virtual ErrorOr<bool> ApplicationStart(
      const std::string& id,
      const flutter::EncodableMap* configuration) = 0;
  // Stop application with given id.
  virtual ErrorOr<bool> ApplicationStop(const std::string& id) = 0;

  // The codec used by FlatpakApi.
  static const flutter::StandardMessageCodec& GetCodec();
  // Sets up an instance of `FlatpakApi` to handle messages through the
  // `binary_messenger`.
  static void SetUp(flutter::BinaryMessenger* binary_messenger,
                    FlatpakApi* api);
  static void SetUp(flutter::BinaryMessenger* binary_messenger,
                    FlatpakApi* api,
                    const std::string& message_channel_suffix);
  static flutter::EncodableValue WrapError(std::string_view error_message);
  static flutter::EncodableValue WrapError(const FlutterError& error);

 protected:
  FlatpakApi() = default;
};
}  // namespace flatpak_plugin
#endif  // PIGEON_MESSAGES_G_H_
