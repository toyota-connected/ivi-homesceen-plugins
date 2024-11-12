/*
 * Copyright 2024 Toyota Connected North America
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

#ifndef PLUGINS_FLATPAK_RELEASE_H
#define PLUGINS_FLATPAK_RELEASE_H

#include <libxml/tree.h>
#include <optional>
#include <string>

class Release {
 public:
  explicit Release(xmlNode* node);

  [[nodiscard]] const std::string& getVersion() const;

  [[nodiscard]] const std::string& getTimestamp() const;  // ISO 8601 format

  [[nodiscard]] const std::optional<std::string>& getDescription() const;

  [[nodiscard]] const std::optional<std::string>& getSize() const;

 private:
  std::string version_;
  std::string timestamp_;
  std::optional<std::string> description_;
  std::optional<std::string> downloadSize_;
};

#endif  // PLUGINS_FLATPAK_RELEASE_H
