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

#include "model.h"

#include <core/components/derived/collidable.h>
#include <core/include/literals.h>
#include <core/systems/derived/filament_system.h>
#include <core/systems/derived/material_system.h>
#include <core/systems/ecsystems_manager.h>
#include <core/utils/deserialize.h>
#include <filament/RenderableManager.h>
#include <plugins/common/common.h>
#include <utils/Slice.h>
#include <utility>

namespace plugin_filament_view {

////////////////////////////////////////////////////////////////////////////
Model::Model(std::string assetPath,
             std::string url,
             Model* fallback,
             Animation* animation,
             std::shared_ptr<BaseTransform> poTransform,
             std::shared_ptr<CommonRenderable> poCommonRenderable,
             const flutter::EncodableMap& params)
    : EntityObject(assetPath),
      assetPath_(std::move(assetPath)),
      url_(std::move(url)),
      fallback_(fallback),
      animation_(animation),
      m_poAsset(nullptr) {
  m_poBaseTransform = std::weak_ptr<BaseTransform>(poTransform);
  m_poCommonRenderable = std::weak_ptr<CommonRenderable>(poCommonRenderable);

  DeserializeNameAndGlobalGuid(params);

  vAddComponent(std::move(poTransform));
  vAddComponent(std::move(poCommonRenderable));

  // if we have collidable data request, we need to build that component, as its
  // optional
  if (const auto it = params.find(flutter::EncodableValue(kCollidable));
      it != params.end() && !it->second.IsNull()) {
    // They're requesting a collidable on this object. Make one.
    auto collidableComp = std::make_shared<Collidable>(params);
    vAddComponent(std::move(collidableComp));
  }
}

////////////////////////////////////////////////////////////////////////////
GlbModel::GlbModel(std::string assetPath,
                   std::string url,
                   Model* fallback,
                   Animation* animation,
                   std::shared_ptr<BaseTransform> poTransform,
                   std::shared_ptr<CommonRenderable> poCommonRenderable,
                   const flutter::EncodableMap& params)
    : Model(std::move(assetPath),
            std::move(url),
            fallback,
            animation,
            std::move(poTransform),
            std::move(poCommonRenderable),
            params) {}

////////////////////////////////////////////////////////////////////////////
GltfModel::GltfModel(std::string assetPath,
                     std::string url,
                     std::string pathPrefix,
                     std::string pathPostfix,
                     Model* fallback,
                     Animation* animation,
                     std::shared_ptr<BaseTransform> poTransform,
                     std::shared_ptr<CommonRenderable> poCommonRenderable,
                     const flutter::EncodableMap& params)
    : Model(std::move(assetPath),
            std::move(url),
            fallback,
            animation,
            std::move(poTransform),
            std::move(poCommonRenderable),
            params),
      pathPrefix_(std::move(pathPrefix)),
      pathPostfix_(std::move(pathPostfix)) {}

////////////////////////////////////////////////////////////////////////////
std::unique_ptr<Model> Model::Deserialize(const std::string& flutterAssetsPath,
                                          const flutter::EncodableMap& params) {
  SPDLOG_TRACE("++Model::Model");
  std::unique_ptr<Animation> animation;
  std::unique_ptr<Model> fallback;
  std::optional<std::string> assetPath;
  std::optional<std::string> pathPrefix;
  std::optional<std::string> pathPostfix;
  std::optional<std::string> url;
  bool is_glb = false;

  auto oTransform = std::make_shared<BaseTransform>(params);
  auto oCommonRenderable = std::make_shared<CommonRenderable>(params);

  for (const auto& [fst, snd] : params) {
    if (snd.IsNull())
      continue;

    if (auto key = std::get<std::string>(fst);
        key == "animation" &&
        std::holds_alternative<flutter::EncodableMap>(snd)) {
      animation = std::make_unique<Animation>(
          flutterAssetsPath, std::get<flutter::EncodableMap>(snd));
    } else if (key == "assetPath" && std::holds_alternative<std::string>(snd)) {
      assetPath = std::get<std::string>(snd);
    } else if (key == "isGlb" && std::holds_alternative<bool>(snd)) {
      is_glb = std::get<bool>(snd);
    } else if (key == "url" && std::holds_alternative<std::string>(snd)) {
      url = std::get<std::string>(snd);
    } else if (key == "pathPrefix" &&
               std::holds_alternative<std::string>(snd)) {
      pathPrefix = std::get<std::string>(snd);
    } else if (key == "pathPostfix" &&
               std::holds_alternative<std::string>(snd)) {
      pathPostfix = std::get<std::string>(snd);
    } else if (key == "scene" &&
               std::holds_alternative<flutter::EncodableMap>(snd)) {
      spdlog::warn("Scenes are no longer valid off of a model node.");
    } /*else if (!it.second.IsNull()) {
      spdlog::debug("[Model] Unhandled Parameter");
      plugin_common::Encodable::PrintFlutterEncodableValue(key.c_str(),
                                                           it.second);
    }*/
  }

  if (is_glb) {
    return std::make_unique<GlbModel>(
        assetPath.has_value() ? std::move(assetPath.value()) : "",
        url.has_value() ? std::move(url.value()) : "", nullptr,
        animation ? animation.release() : nullptr, oTransform,
        oCommonRenderable, params);
  }

  return std::make_unique<GltfModel>(
      assetPath.has_value() ? std::move(assetPath.value()) : "",
      url.has_value() ? std::move(url.value()) : "",
      pathPrefix.has_value() ? std::move(pathPrefix.value()) : "",
      pathPostfix.has_value() ? std::move(pathPostfix.value()) : "", nullptr,
      animation ? animation.release() : nullptr, oTransform, oCommonRenderable,
      params);
}

////////////////////////////////////////////////////////////////////////////
void Model::DebugPrint() const {
  vDebugPrintComponents();
}

////////////////////////////////////////////////////////////////////////////
void Model::vLoadMaterialDefinitionsToMaterialInstance() {
  const auto materialSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<MaterialSystem>(
          MaterialSystem::StaticGetTypeID(), "BaseShape::vBuildRenderable");

  if (materialSystem == nullptr) {
    spdlog::error("Failed to get material system.");
  } else {
    // this will also set all the default values of the material instance from
    // the material param list
    const auto materialDefinitions =
        GetComponentByStaticTypeID(MaterialDefinitions::StaticGetTypeID());
    if (materialDefinitions != nullptr) {
      m_poMaterialInstance = materialSystem->getMaterialInstance(
          dynamic_cast<const MaterialDefinitions*>(materialDefinitions.get()));
    }

    if (m_poMaterialInstance.getStatus() != Status::Success) {
      spdlog::error("Failed to get material instance.");
    }
  }
}

////////////////////////////////////////////////////////////////////////////
void Model::vChangeMaterialDefinitions(const flutter::EncodableMap& params,
                                       const TextureMap& /*loadedTextures*/) {
  // if we have a materialdefinitions component, we need to remove it
  // and remake / add a new one.
  if (HasComponentByStaticTypeID(MaterialDefinitions::StaticGetTypeID())) {
    vRemoveComponent(MaterialDefinitions::StaticGetTypeID());
  }

  // If you want to inspect the params coming in.
  /*for (const auto& [fst, snd] : params) {
      auto key = std::get<std::string>(fst);
      plugin_common::Encodable::PrintFlutterEncodableValue(key.c_str(), snd);
  }*/

  auto materialDefinitions = std::make_shared<MaterialDefinitions>(params);
  vAddComponent(std::move(materialDefinitions));

  m_poMaterialInstance.vReset();

  // then tell material system to load us the correct way once
  // we're deserialized.
  vLoadMaterialDefinitionsToMaterialInstance();

  if (m_poMaterialInstance.getStatus() != Status::Success) {
    spdlog::error(
        "Unable to load material definition to instance, bailing out.");
    return;
  }

  // now, reload / rebuild the material?
  const auto filamentSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<FilamentSystem>(
          FilamentSystem::StaticGetTypeID(),
          "BaseShape::vChangeMaterialDefinitions");

  // If your entity has multiple primitives, you’ll need to call
  // setMaterialInstanceAt for each primitive you want to update.
  auto& renderManager =
      filamentSystem->getFilamentEngine()->getRenderableManager();

  utils::Slice const listOfRenderables{getAsset()->getRenderableEntities(),
                                       getAsset()->getRenderableEntityCount()};

  // Note this will apply to EVERYTHING currently. You might want a custom <only
  // effect these pieces> type functionality.
  for (const auto entity : listOfRenderables) {
    const auto ri = renderManager.getInstance(entity);

    // I dont know about primitive index being non zero if our tree has multiple
    // nodes getting from the asset.
    renderManager.setMaterialInstanceAt(ri, 0, *m_poMaterialInstance.getData());
  }
}

////////////////////////////////////////////////////////////////////////////
void Model::vChangeMaterialInstanceProperty(
    const MaterialParameter* materialParam,
    const TextureMap& loadedTextures) {
  if (m_poMaterialInstance.getStatus() != Status::Success) {
    spdlog::error(
        "No material definition set for model, set one first that's not the "
        "uber shader.");
    return;
  }

  const auto data = m_poMaterialInstance.getData().value();

  const auto matDefs = dynamic_cast<MaterialDefinitions*>(
      GetComponentByStaticTypeID(MaterialDefinitions::StaticGetTypeID()).get());
  if (matDefs == nullptr) {
    return;
  }

  MaterialDefinitions::vApplyMaterialParameterToInstance(data, materialParam,
                                                         loadedTextures);
}

}  // namespace plugin_filament_view
