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

#ifndef PLUGINS_FLATPAK_ICON_H
#define PLUGINS_FLATPAK_ICON_H

#include <libxml/tree.h>
#include <optional>
#include <string>

class Icon {
 public:
  explicit Icon(const xmlNode* node);

  [[nodiscard]] const std::optional<std::string>& getType() const;

  [[nodiscard]] const std::optional<int>& getWidth() const;

  [[nodiscard]] const std::optional<int>& getHeight() const;

  [[nodiscard]] const std::optional<int>& getScale() const;

  [[nodiscard]] const std::optional<std::string>& getPath() const;

  void printIconDetails() const;

 private:
  std::optional<std::string> type_;
  std::optional<int> width_;
  std::optional<int> height_;
  std::optional<int> scale_;
  std::optional<std::string> path_;

  void parseXmlNode(const xmlNode* node);
};

#endif  // PLUGINS_FLATPAK_ICON_H
