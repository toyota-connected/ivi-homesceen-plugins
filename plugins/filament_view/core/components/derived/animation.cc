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
#include "animation.h"

#include <core/include/literals.h>
#include <core/utils/deserialize.h>
#include <plugins/common/common.h>
#include <filesystem>
#include <core/systems/ecsystems_manager.h>

namespace plugin_filament_view {

////////////////////////////////////////////////////////////////////////////
Animation::Animation(const flutter::EncodableMap& params)
    : Component(std::string(__FUNCTION__)) {
    Deserialize::DecodeParameterWithDefault(kAutoPlay, &auto_play_, params, false);
    Deserialize::DecodeParameterWithDefault(kIndex, &index_, params, 0);
    Deserialize::DecodeParameterWithDefault(kName, &name_, params, "not_set");
    Deserialize::DecodeParameterWithDefault(kAssetPath, &asset_path_, params, "");

    // animation had centerPosition as a variable, not using this atm.
    /*for (const auto& [fst, snd] : params) {
        if (snd.IsNull())
            continue;

        auto key = std::get<std::string>(fst);
        if (key == "centerPosition" &&
                   std::holds_alternative<flutter::EncodableMap>(snd)) {
            center_position_ = std::make_unique<filament::math::float3>(
                Deserialize::Format3(std::get<flutter::EncodableMap>(snd)));
                   } else if (!snd.IsNull()) {
                       spdlog::debug("[Animation] Unhandled Parameter");
                       plugin_common::Encodable::PrintFlutterEncodableValue(key.c_str(), snd);
                   }
    }*/
}

////////////////////////////////////////////////////////////////////////////
void Animation::DebugPrint(const std::string& tabPrefix) const {
    spdlog::debug(tabPrefix + "name: [{}]", name_);
    spdlog::debug(tabPrefix + "index_: {}", index_);
    spdlog::debug(tabPrefix + "autoPlay: {}", auto_play_);
    spdlog::debug(tabPrefix + "asset_path: [{}]", asset_path_);

    const auto flutterAssetsPath =
        ECSystemManager::GetInstance()->getConfigValue<std::string>(kAssetPath);

    const std::filesystem::path asset_folder(flutterAssetsPath);
    spdlog::debug(tabPrefix + "asset_path {} valid",
                  exists(asset_folder / asset_path_) ? "is" : "is not");

}

}  // namespace plugin_filament_view