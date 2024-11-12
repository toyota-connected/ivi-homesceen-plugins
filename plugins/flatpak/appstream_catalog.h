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

#ifndef PLUGINS_FLATPAK_APPSTREAM_CATALOG_H
#define PLUGINS_FLATPAK_APPSTREAM_CATALOG_H

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>
#include "component.h"

class AppstreamCatalog {
 public:
  explicit AppstreamCatalog(const std::string& filePath, std::string language);

  ~AppstreamCatalog();

  [[nodiscard]] const std::vector<Component>& getComponents() const;

  [[nodiscard]] std::vector<Component> searchByCategory(
      const std::string& category,
      bool sorted = false,
      const std::string& key = "") const;

  [[nodiscard]] std::vector<Component> searchByKeyword(
      const std::string& keyword,
      bool sorted = false,
      const std::string& key = "") const;

  [[nodiscard]] std::optional<Component> searchById(
      const std::string& id) const;

  [[nodiscard]] size_t getTotalComponentCount() const;

  [[nodiscard]] std::unordered_set<std::string> getUniqueCategories() const;

  [[nodiscard]] std::unordered_set<std::string> getUniqueKeywords() const;

 private:
  std::string language_;

  void parseXmlFile(const std::string& filePath);

  static void decompressGzFile(const std::string& gzPath,
                               const std::string& xmlPath);

  std::vector<Component> components_;
  std::unordered_set<std::string> uniqueCategories_;
  std::unordered_set<std::string> uniqueKeywords_;
};

#endif  // APPSTREAM_CATALOG_H
