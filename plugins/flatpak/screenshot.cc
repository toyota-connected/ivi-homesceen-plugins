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

#include "screenshot.h"

#include <libxml/xmlstring.h>

#include "common.h"
#include "common/common.h"

Image::Image(xmlNode* node) {
  parseXmlNode(node);
}

void Image::parseXmlNode(xmlNode* node) {
  type_ = getAttribute(node, "type");
  if (const auto widthAttr = getOptionalAttribute(node, "width")) {
    width_ = std::stoi(*widthAttr);
  }
  if (const auto heightAttr = getOptionalAttribute(node, "height")) {
    height_ = std::stoi(*heightAttr);
  }
  url_ = std::string(reinterpret_cast<const char*>(xmlNodeGetContent(node)));
}

void Image::printImageDetails() const {
  spdlog::info("\tImage:");
  if (type_.has_value())
    spdlog::info("\t\tType: {}", type_.value());
  if (width_.has_value())
    spdlog::info("\t\tWidth: {}", width_.value());
  if (height_.has_value())
    spdlog::info("\t\tHeight: {}", height_.value());
  if (url_.has_value())
    spdlog::info("\t\tURL: {}", url_.value());
}

Video::Video(xmlNode* node) {
  parseXmlNode(node);
}

void Video::parseXmlNode(xmlNode* node) {
  container_ = getAttribute(node, "container");
  codec_ = getAttribute(node, "codec");
  if (const auto widthAttr = getOptionalAttribute(node, "width")) {
    width_ = std::stoi(*widthAttr);
  }
  if (const auto heightAttr = getOptionalAttribute(node, "height")) {
    height_ = std::stoi(*heightAttr);
  }
  url_ = std::string(reinterpret_cast<const char*>(xmlNodeGetContent(node)));
}

void Video::printVideoDetails() const {
  spdlog::info("\tVideo:");
  if (container_.has_value())
    spdlog::info("\t\tContainer: {}", container_.value());
  if (codec_.has_value())
    spdlog::info("\t\tCodec: {}", codec_.value());
  if (width_.has_value())
    spdlog::info("\t\tWidth: {}", width_.value());
  if (height_.has_value())
    spdlog::info("\t\tHeight: {}", height_.value());
  if (url_.has_value())
    spdlog::info("\t\tURL: {}", url_.value());
}

Screenshot::Screenshot(const xmlNode* node) {
  parseXmlNode(node);
}

void Screenshot::parseXmlNode(const xmlNode* node) {
  std::vector<Image> images;
  for (xmlNode* current = node->children; current; current = current->next) {
    if (xmlStrEqual(current->name, BAD_CAST "screenshot")) {
      type_ = getAttribute(current, "type");
    } else if (xmlStrEqual(current->name, BAD_CAST "caption")) {
      captions_.emplace_back(
          reinterpret_cast<const char*>(xmlNodeGetContent(current)));
    } else if (xmlStrEqual(current->name, BAD_CAST "image")) {
      images.emplace_back(current);
    } else if (xmlStrEqual(current->name, BAD_CAST "video")) {
      video_ = Video(current);
    }
  }

  if (!images.empty()) {
    images_ = std::move(images);
  }
}

void Screenshot::printScreenshotDetails() const {
  if (type_.has_value()) {
    spdlog::info("\tScreenshot:");
    spdlog::info("\t\tType: {}", type_.value());
  } else {
    return;
  }

  for (const auto& caption : captions_) {
    spdlog::info("\t\tCaption: {}", caption);
  }

  if (images_.has_value()) {
    for (const auto& image : images_.value()) {
      image.printImageDetails();
    }
  }

  if (video_.has_value()) {
    video_->printVideoDetails();
  }
}
