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

#include "common.h"

#include <sstream>

#include <libxml/tree.h>
#include <libxml/xmlstring.h>

#include "common/common.h"
#include "component.h"

std::optional<std::string> getOptionalAttribute(const xmlNode* node,
                                                const char* attrName) {
  if (xmlChar* xmlValue = xmlGetProp(node, BAD_CAST attrName)) {
    std::string value(reinterpret_cast<const char*>(xmlValue));
    xmlFree(xmlValue);
    return value;
  }
  return std::nullopt;
}

std::string getAttribute(const xmlNode* node, const char* attrName) {
  if (xmlChar* xmlValue = xmlGetProp(node, BAD_CAST attrName)) {
    std::string value(reinterpret_cast<const char*>(xmlValue));
    xmlFree(xmlValue);
    return value;
  }
  return "";
}

void PrintComponent(const Component& component) {
  spdlog::info("[FlatpakPlugin] Component [{}]", component.getId());
  spdlog::info("[FlatpakPlugin] \tName: {}", component.getName());
  spdlog::info("[FlatpakPlugin] \tPackage Name: {}", component.getPkgName());
  spdlog::info("[FlatpakPlugin] \tSummary: {}", component.getSummary());

  if (component.getReleases()) {
    spdlog::info("[FlatpakPlugin] \tReleases: ");
    for (const auto& release : component.getReleases().value()) {
      spdlog::info("[FlatpakPlugin] \t\tVersion: {}", release.getVersion());
      spdlog::info("[FlatpakPlugin] \t\tTimestamp: {}", release.getTimestamp());
      if (release.getDescription()) {
        spdlog::info("[FlatpakPlugin] \t\tDescription: {}",
                     release.getDescription().value());
      }
      if (release.getSize()) {
        spdlog::info("[FlatpakPlugin] \t\tSize: {}", release.getSize().value());
      }
    }
  }

  // Checking and printing optional fields
  if (component.getVersion()) {
    spdlog::info("[FlatpakPlugin] \tVersion: {}",
                 component.getVersion().value());
  }
  if (component.getOrigin()) {
    spdlog::info("[FlatpakPlugin] \tOrigin: {}", component.getOrigin().value());
  }
  if (component.getMediaBaseurl()) {
    spdlog::info("[FlatpakPlugin] \tMedia Base URL: {}",
                 component.getMediaBaseurl().value());
  }
  if (component.getArchitecture()) {
    spdlog::info("[FlatpakPlugin] \tArchitecture: {}",
                 component.getArchitecture().value());
  }
  if (component.getProjectLicense()) {
    spdlog::info("[FlatpakPlugin] \tProject License: {}",
                 component.getProjectLicense().value());
  }
  if (component.getDescription()) {
    spdlog::info("[FlatpakPlugin] \tDescription: {}",
                 component.getDescription().value());
  }
  if (component.getUrl()) {
    spdlog::info("[FlatpakPlugin] \tURL: {}", component.getUrl().value());
  }
  if (component.getProjectGroup()) {
    spdlog::info("[FlatpakPlugin] \tProject Group: {}",
                 component.getProjectGroup().value());
  }
  if (const auto& icons = component.getIcons(); icons.has_value()) {
    for (const auto& icon : icons.value()) {
      icon.printIconDetails();
    }
  }
  if (component.getCategories()) {
    spdlog::info("[FlatpakPlugin] \tCategories:");
    for (const auto& category : component.getCategories().value()) {
      spdlog::info("[FlatpakPlugin] \t\t{}", category);
    }
  }
  if (component.getScreenshots()) {
    for (const auto& screenshot : component.getScreenshots().value()) {
      screenshot.printScreenshotDetails();
    }
  }
  if (component.getKeywords()) {
    spdlog::info("[FlatpakPlugin] \tKeywords:");
    for (const auto& keyword : component.getKeywords().value()) {
      spdlog::info("[FlatpakPlugin] \t\t{}", keyword);
    }
  }
  // Additional optional fields
  if (component.getSourcePkgname()) {
    spdlog::info("[FlatpakPlugin] \tSource Pkgname: {}",
                 component.getSourcePkgname().value());
  }
  if (component.getBundle()) {
    spdlog::info("[FlatpakPlugin] \tBundle: {}", component.getBundle().value());
  }
  if (component.getContentRatingType()) {
    if (component.getContentRatingType().has_value()) {
      spdlog::info("[FlatpakPlugin] \tContent Rating Type: [{}]",
                   component.getContentRatingType().value());
    }
  }
  if (component.getContentRating()) {
    if (component.getContentRating().has_value()) {
      if (!component.getContentRating().value().empty()) {
        spdlog::info("[FlatpakPlugin] \tContent Rating:");
        for (const auto& [key, value] : component.getContentRating().value()) {
          spdlog::info("[FlatpakPlugin] \t\t{} = {}", key,
                       Component::RatingValueToString(value));
        }
      }
    }
  }
  if (component.getAgreement()) {
    spdlog::info("[FlatpakPlugin] \tAgreement: {}",
                 component.getAgreement().value());
  }
}
