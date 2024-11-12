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

#ifndef PLUGINS_FLATPAK_COMPONENT_H
#define PLUGINS_FLATPAK_COMPONENT_H

#include <libxml/tree.h>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>
#include "icon.h"
#include "release.h"
#include "screenshot.h"

class Component {
 public:
  enum ContentRatingValue { none = 0, mild, moderate, intense };

  explicit Component(const xmlNode* node, std::string language);

  // Required fields
  [[nodiscard]] const std::string& getId() const;

  [[nodiscard]] const std::string& getName() const;

  [[nodiscard]] const std::string& getSummary() const;

  [[nodiscard]] const std::string& getPkgName() const;

  // Optional fields with checks
  [[nodiscard]] const std::optional<std::string>& getVersion() const;

  [[nodiscard]] const std::optional<std::string>& getOrigin() const;

  [[nodiscard]] const std::optional<std::string>& getMediaBaseurl() const;

  [[nodiscard]] const std::optional<std::string>& getArchitecture() const;

  [[nodiscard]] const std::optional<std::string>& getProjectLicense() const;

  [[nodiscard]] const std::optional<std::string>& getDescription() const;

  [[nodiscard]] const std::optional<std::string>& getUrl() const;

  [[nodiscard]] const std::optional<std::string>& getProjectGroup() const;

  [[nodiscard]] const std::optional<std::unordered_set<std::string>>&
  getCategories() const;

  [[nodiscard]] const std::optional<std::unordered_set<std::string>>&
  getKeywords() const;

  [[nodiscard]] const std::optional<std::vector<Screenshot>>& getScreenshots()
      const;

  [[nodiscard]] const std::optional<std::vector<Release>>& getReleases() const;

  [[nodiscard]] const std::optional<std::vector<Icon>>& getIcons() const;

  [[nodiscard]] const std::optional<std::unordered_set<std::string>>&
  getLanguages() const;

  [[nodiscard]] const std::optional<std::unordered_set<std::string>>&
  getSuggests() const;

  [[nodiscard]] const std::optional<std::unordered_set<std::string>>&
  getProvides() const;

  [[nodiscard]] const std::optional<std::unordered_set<std::string>>&
  getCompulsoryForDesktop() const;

  [[nodiscard]] const std::optional<std::unordered_set<std::string>>&
  getDeveloper() const;

  [[nodiscard]] const std::optional<std::unordered_set<std::string>>&
  getLaunchable() const;

  // Optional fields
  [[nodiscard]] const std::optional<std::string>& getSourcePkgname() const;

  [[nodiscard]] const std::optional<std::string>& getBundle() const;

  [[nodiscard]] const std::optional<std::string>& getContentRatingType() const;

  [[nodiscard]] const std::optional<std::map<std::string, ContentRatingValue>>&
  getContentRating() const;

  [[nodiscard]] const std::optional<std::string>& getAgreement() const;

  static std::string RatingValueToString(ContentRatingValue value);
  static ContentRatingValue CharToRatingValue(const char* value);

 private:
  void parseCategories(const xmlNode* node);

  void parseIcons(xmlNode* node);

  void parseKeywords(const xmlNode* node);

  void parseReleases(xmlNode* node);

  void parseScreenshots(xmlNode* node);

  void parseContentRating(const xmlNode* node);

  std::string language_;

  // Required fields
  std::string id_;
  std::string name_;
  std::string summary_;
  std::string pkgname_;

  // Optional fields
  std::optional<std::string> version_;
  std::optional<std::string> origin_;
  std::optional<std::string> mediaBaseurl_;
  std::optional<std::string> architecture_;
  std::optional<std::string> projectLicense_;
  std::optional<std::string> description_;
  std::optional<std::string> url_;
  std::optional<std::string> projectGroup_;
  std::optional<std::string> icon_;
  std::optional<std::unordered_set<std::string>> categories_;
  std::optional<std::unordered_set<std::string>> keywords_;
  std::optional<std::vector<Icon>> icons_;
  std::optional<std::vector<Release>> releases_;
  std::optional<std::vector<Screenshot>> screenshots_;
  std::optional<std::unordered_set<std::string>> languages_;
  std::optional<std::unordered_set<std::string>> suggests_;
  std::optional<std::unordered_set<std::string>> provides_;
  std::optional<std::unordered_set<std::string>> compulsoryForDesktop_;
  std::optional<std::unordered_set<std::string>> developer_;
  std::optional<std::unordered_set<std::string>> launchable_;

  // Optional fields
  std::optional<std::string> sourcePkgname_;
  std::optional<std::string> bundle_;
  std::optional<std::map<std::string, ContentRatingValue>> contentRating_;
  std::optional<std::string> contentRatingType_;
  std::optional<std::string> agreement_;
};

#endif  // COMPONENT_H
