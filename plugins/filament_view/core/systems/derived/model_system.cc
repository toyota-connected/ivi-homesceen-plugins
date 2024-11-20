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
#include "model_system.h"
#include "collision_system.h"
#include "filament_system.h"

#include <core/components/derived/collidable.h>
#include <core/include/file_utils.h>
#include <core/systems/ecsystems_manager.h>
#include <core/utils/entitytransforms.h>
#include <curl_client/curl_client.h>
#include <filament/Scene.h>
#include <filament/filament/RenderableManager.h>
#include <filament/filament/TransformManager.h>
#include <filament/gltfio/ResourceLoader.h>
#include <filament/gltfio/TextureProvider.h>
#include <filament/gltfio/materials/uberarchive.h>
#include <filament/utils/Slice.h>
#include <algorithm>  // for max
#include <asio/post.hpp>

#include "animation_system.h"

namespace plugin_filament_view {

using filament::gltfio::AssetConfiguration;
using filament::gltfio::AssetLoader;
using filament::gltfio::ResourceConfiguration;
using filament::gltfio::ResourceLoader;

////////////////////////////////////////////////////////////////////////////////////
void ModelSystem::destroyAllAssetsOnModels() {
  for (const auto& [fst, snd] : m_mapszoAssets) {
    destroyAsset(snd->getAsset());  // NOLINT
  }
  m_mapszoAssets.clear();
}

////////////////////////////////////////////////////////////////////////////////////
void ModelSystem::destroyAsset(
    const filament::gltfio::FilamentAsset* asset) const {
  if (!asset) {
    return;
  }

  const auto filamentSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<FilamentSystem>(
          FilamentSystem::StaticGetTypeID(), __FUNCTION__);

  filamentSystem->getFilamentScene()->removeEntities(asset->getEntities(),
                                                     asset->getEntityCount());
  assetLoader_->destroyAsset(asset);
}

////////////////////////////////////////////////////////////////////////////////////
filament::gltfio::FilamentAsset* ModelSystem::poFindAssetByGuid(
    const std::string& szGUID) {
  const auto iter = m_mapszoAssets.find(szGUID);
  if (iter == m_mapszoAssets.end()) {
    return nullptr;
  }

  return iter->second->getAsset();
}

////////////////////////////////////////////////////////////////////////////////////
void ModelSystem::loadModelGlb(std::shared_ptr<Model> oOurModel,
                               const std::vector<uint8_t>& buffer,
                               const std::string& /*assetName*/) {
  if (assetLoader_ == nullptr) {
    // NOTE, this should only be temporary until CustomModelViewer isn't
    // necessary in implementation.
    vInitSystem();

    if (assetLoader_ == nullptr) {
      spdlog::error("unable to initialize model system");
      return;
    }
  }

  auto* asset = assetLoader_->createAsset(buffer.data(),
                                          static_cast<uint32_t>(buffer.size()));
  if (!asset) {
    spdlog::error("Failed to loadModelGlb->createasset from buffered data.");
    return;
  }

  const auto filamentSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<FilamentSystem>(
          FilamentSystem::StaticGetTypeID(), "loadModelGlb");
  const auto engine = filamentSystem->getFilamentEngine();

  resourceLoader_->asyncBeginLoad(asset);

  // TODO
  // This will move to be on the model itself.
  // modelViewer->setAnimator(asset->getInstance()->getAnimator());

  // NOTE if this is a prefab/instance you will NOT Want to do this.
  asset->releaseSourceData();

  auto& rcm = engine->getRenderableManager();

  utils::Slice const listOfRenderables{asset->getRenderableEntities(),
                                       asset->getRenderableEntityCount()};

  for (const auto entity : listOfRenderables) {
    const auto ri = rcm.getInstance(entity);
    rcm.setCastShadows(
        ri, oOurModel->GetCommonRenderable()->IsCastShadowsEnabled());
    rcm.setReceiveShadows(
        ri, oOurModel->GetCommonRenderable()->IsReceiveShadowsEnabled());
    // Investigate this more before making it a property on common renderable
    // component.
    rcm.setScreenSpaceContactShadows(ri, false);
  }

  oOurModel->setAsset(asset);

  EntityTransforms::vApplyTransform(oOurModel->getAsset(),
                                    *oOurModel->GetBaseTransform());

  std::shared_ptr<Model> sharedPtr = std::move(oOurModel);

  vSetupAssetThroughoutECS(sharedPtr, asset);
}

////////////////////////////////////////////////////////////////////////////////////
void ModelSystem::loadModelGltf(
    std::shared_ptr<Model> oOurModel,
    const std::vector<uint8_t>& buffer,
    std::function<const filament::backend::BufferDescriptor&(
        std::string uri)>& /* callback */) {
  auto* asset = assetLoader_->createAsset(buffer.data(),
                                          static_cast<uint32_t>(buffer.size()));
  if (!asset) {
    spdlog::error("Failed to loadModelGltf->createasset from buffered data.");
    return;
  }

  const auto uri_data = asset->getResourceUris();
  const auto uris =
      std::vector(uri_data, uri_data + asset->getResourceUriCount());
  for (const auto uri : uris) {
    (void)uri;
    SPDLOG_DEBUG("resource uri: {}", uri);
#if 0   // TODO
              auto resourceBuffer = callback(uri);
              if (!resourceBuffer) {
                  this->asset_ = nullptr;
                  return;
              }
              resourceLoader_->addResourceData(uri, resourceBuffer);
#endif  // TODO
  }
  resourceLoader_->asyncBeginLoad(asset);
  // modelViewer->setAnimator(asset->getInstance()->getAnimator());
  asset->releaseSourceData();

  const auto filamentSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<FilamentSystem>(
          FilamentSystem::StaticGetTypeID(), "loadModelGltf");
  const auto engine = filamentSystem->getFilamentEngine();

  auto& rcm = engine->getRenderableManager();

  utils::Slice const listOfRenderables{asset->getRenderableEntities(),
                                       asset->getRenderableEntityCount()};

  for (const auto entity : listOfRenderables) {
    const auto ri = rcm.getInstance(entity);
    rcm.setCastShadows(
        ri, oOurModel->GetCommonRenderable()->IsCastShadowsEnabled());
    rcm.setReceiveShadows(
        ri, oOurModel->GetCommonRenderable()->IsReceiveShadowsEnabled());
    // Investigate this more before making it a property on common renderable
    // component.
    rcm.setScreenSpaceContactShadows(ri, false);
  }

  oOurModel->setAsset(asset);

  EntityTransforms::vApplyTransform(oOurModel->getAsset(),
                                    *oOurModel->GetBaseTransform());

  std::shared_ptr<Model> sharedPtr = std::move(oOurModel);

  vSetupAssetThroughoutECS(sharedPtr, asset);
}

////////////////////////////////////////////////////////////////////////////////////
void ModelSystem::vSetupAssetThroughoutECS(
    std::shared_ptr<Model>& sharedPtr,
    filament::gltfio::FilamentAsset* filamentAsset) {
  m_mapszoAssets.insert(std::pair(sharedPtr->GetGlobalGuid(), sharedPtr));

  if (const auto animatorInstance = filamentAsset->getInstance()->getAnimator();
      animatorInstance != nullptr &&
      sharedPtr->HasComponentByStaticTypeID(Animation::StaticGetTypeID())) {
    const auto animatorComponent =
        sharedPtr->GetComponentByStaticTypeID(Animation::StaticGetTypeID());
    const auto animator = dynamic_cast<Animation*>(animatorComponent.get());
    animator->vSetAnimator(*animatorInstance);

    // Great if you need help with your animation information!
    // animationPtr->DebugPrint("From ModelSystem::vSetupAssetThroughoutECS\t");
  } else if (animatorInstance != nullptr &&
             animatorInstance->getAnimationCount() > 0) {
    SPDLOG_DEBUG(
        "For asset - {} you have a valid set of animations [{}] you can play "
        "on this, but you didn't load an animation component, load one if you "
        "want that "
        "functionality",
        sharedPtr->szGetAssetPath(), animatorInstance->getAnimationCount());
  }

  sharedPtr->vRegisterEntity();
}

////////////////////////////////////////////////////////////////////////////////////
void ModelSystem::populateSceneWithAsyncLoadedAssets(const Model* model) {
  const auto filamentSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<FilamentSystem>(
          FilamentSystem::StaticGetTypeID(), __FUNCTION__);
  const auto engine = filamentSystem->getFilamentEngine();

  auto& rcm = engine->getRenderableManager();

  auto* asset = model->getAsset();

  size_t count = asset->popRenderables(nullptr, 0);
  while (count) {
    constexpr size_t maxToPopAtOnce = 128;
    auto maxToPop = std::min(count, maxToPopAtOnce);

    SPDLOG_DEBUG(
        "ModelSystem::populateSceneWithAsyncLoadedAssets async load count "
        "available[{}] - working on [{}]",
        count, maxToPop);
    // Note for high amounts, we should probably do a small amount; break out;
    // and let it come back do more on another frame.

    asset->popRenderables(readyRenderables_, maxToPop);

    utils::Slice const listOfRenderables{asset->getRenderableEntities(),
                                         asset->getRenderableEntityCount()};

    for (const auto entity : listOfRenderables) {
      const auto ri = rcm.getInstance(entity);
      rcm.setCastShadows(ri,
                         model->GetCommonRenderable()->IsCastShadowsEnabled());
      rcm.setReceiveShadows(
          ri, model->GetCommonRenderable()->IsReceiveShadowsEnabled());
      // Investigate this more before making it a property on common renderable
      // component.
      rcm.setScreenSpaceContactShadows(ri, false);
    }
    filamentSystem->getFilamentScene()->addEntities(readyRenderables_,
                                                    maxToPop);
    count = asset->popRenderables(nullptr, 0);
  }

  if ([[maybe_unused]] auto lightEntities = asset->getLightEntities()) {
    spdlog::info(
        "Note: Light entities have come in from asset model load;"
        "these are not attached to our entities and will be un changeable");
    filamentSystem->getFilamentScene()->addEntities(asset->getLightEntities(),
                                                    sizeof(*lightEntities));
  }
}

////////////////////////////////////////////////////////////////////////////////////
void ModelSystem::updateAsyncAssetLoading() {
  resourceLoader_->asyncUpdateLoad();

  // This does not specify per resource, but a global, best we can do with this
  // information is if we're done loading <everything> that was marked as async
  // load, then load that physics data onto a collidable if required. This gives
  // us visuals without collidables in a scene with <tons> of objects; but would
  // eventually settle
  const float percentComplete = resourceLoader_->asyncGetLoadProgress();

  for (const auto& [fst, snd] : m_mapszoAssets) {
    populateSceneWithAsyncLoadedAssets(snd.get());

    if (percentComplete != 1.0f) {
      continue;
    }

    auto collisionSystem =
        ECSystemManager::GetInstance()->poGetSystemAs<CollisionSystem>(
            CollisionSystem::StaticGetTypeID(), "updateAsyncAssetLoading");
    if (collisionSystem == nullptr) {
      spdlog::warn("Failed to get collision system when loading model");
      continue;
    }

    // if its 'done' loading, we need to create our large AABB collision
    // object if this model it's referencing required one.
    //
    // Also need to make sure it hasn't already created one for this model.
    if (snd->HasComponentByStaticTypeID(Collidable::StaticGetTypeID()) &&
        !collisionSystem->bHasEntityObjectRepresentation(fst)) {
      // I don't think this needs to become a message; as an async load
      // gives us un-deterministic throughput; it can't be replicated with a
      // messaging structure, and we have to wait till the load is done.
      collisionSystem->vAddCollidable(snd.get());
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////
std::future<Resource<std::string_view>> ModelSystem::loadGlbFromAsset(
    std::shared_ptr<Model> oOurModel,
    const std::string& path,
    bool isFallback) {
  const auto promise(
      std::make_shared<std::promise<Resource<std::string_view>>>());
  auto promise_future(promise->get_future());

  try {
    const asio::io_context::strand& strand_(
        *ECSystemManager::GetInstance()->GetStrand());
    const auto assetPath =
        ECSystemManager::GetInstance()->getConfigValue<std::string>(kAssetPath);

    post(strand_, [&, model = std::move(oOurModel), promise, path, isFallback,
                   assetPath]() mutable {
      try {
        const auto buffer = readBinaryFile(path, assetPath);
        handleFile(std::move(model), buffer, path, isFallback, promise);
      } catch (const std::exception& e) {
        std::cerr << "Lambda Exception " << e.what() << '\n';
        promise->set_exception(std::make_exception_ptr(e));
      } catch (...) {
        std::cerr << "Unknown Exception in lambda" << '\n';
      }
    });
  } catch (const std::exception& e) {
    std::cerr << "Total Exception: " << e.what() << '\n';
    promise->set_exception(std::make_exception_ptr(e));
  }
  return promise_future;
}

////////////////////////////////////////////////////////////////////////////////////
std::future<Resource<std::string_view>> ModelSystem::loadGlbFromUrl(
    std::shared_ptr<Model> oOurModel,
    std::string url,
    bool isFallback) {
  const auto promise(
      std::make_shared<std::promise<Resource<std::string_view>>>());
  auto promise_future(promise->get_future());
  post(*ECSystemManager::GetInstance()->GetStrand(),
       [&, model = std::move(oOurModel), promise, url = std::move(url),
        isFallback]() mutable {
         plugin_common_curl::CurlClient client;
         const auto buffer = client.RetrieveContentAsVector();
         if (client.GetCode() != CURLE_OK) {
           promise->set_value(Resource<std::string_view>::Error(
               "Couldn't load Glb from " + url));
         }
         handleFile(std::move(model), buffer, url, isFallback, promise);
       });
  return promise_future;
}

////////////////////////////////////////////////////////////////////////////////////
void ModelSystem::handleFile(std::shared_ptr<Model>&& oOurModel,
                             const std::vector<uint8_t>& buffer,
                             const std::string& fileSource,
                             bool /*isFallback*/,
                             const PromisePtr& promise) {
  if (!buffer.empty()) {
    loadModelGlb(std::move(oOurModel), buffer, fileSource);
    promise->set_value(Resource<std::string_view>::Success(
        "Loaded glb model successfully from " + fileSource));
  } else {
    promise->set_value(Resource<std::string_view>::Error(
        "Couldn't load glb model from " + fileSource));
  }
}

////////////////////////////////////////////////////////////////////////////////////
std::future<Resource<std::string_view>> ModelSystem::loadGltfFromAsset(
    const std::shared_ptr<Model>& /*oOurModel*/,
    const std::string& /* path */,
    const std::string& /* pre_path */,
    const std::string& /* post_path */,
    bool /* isFallback */) {
  const auto promise(
      std::make_shared<std::promise<Resource<std::string_view>>>());
  auto future(promise->get_future());
  promise->set_value(Resource<std::string_view>::Error("Not implemented yet"));
  return future;
}

////////////////////////////////////////////////////////////////////////////////////
std::future<Resource<std::string_view>> ModelSystem::loadGltfFromUrl(
    const std::shared_ptr<Model>& /*oOurModel*/,
    const std::string& /* url */,
    bool /* isFallback */) {
  const auto promise(
      std::make_shared<std::promise<Resource<std::string_view>>>());
  auto future(promise->get_future());
  promise->set_value(Resource<std::string_view>::Error("Not implemented yet"));
  return future;
}

////////////////////////////////////////////////////////////////////////////////////
void ModelSystem::vInitSystem() {
  if (materialProvider_ != nullptr) {
    return;
  }

  const auto filamentSystem =
      ECSystemManager::GetInstance()->poGetSystemAs<FilamentSystem>(
          FilamentSystem::StaticGetTypeID(), "ModelSystem::vInitSystem");
  const auto engine = filamentSystem->getFilamentEngine();

  if (engine == nullptr) {
    spdlog::error("Engine is null, delaying vInitSystem");
    return;
  }

  materialProvider_ = filament::gltfio::createUbershaderProvider(
      engine, UBERARCHIVE_DEFAULT_DATA,
      static_cast<size_t>(UBERARCHIVE_DEFAULT_SIZE));

  SPDLOG_DEBUG("UbershaderProvider MaterialsCount: {}",
               materialProvider_->getMaterialsCount());

  AssetConfiguration assetConfiguration{};
  assetConfiguration.engine = engine;
  assetConfiguration.materials = materialProvider_;
  assetLoader_ = AssetLoader::create(assetConfiguration);

  ResourceConfiguration resourceConfiguration{};
  resourceConfiguration.engine = engine;
  resourceConfiguration.normalizeSkinningWeights = true;
  resourceLoader_ = new ResourceLoader(resourceConfiguration);

  const auto decoder = filament::gltfio::createStbProvider(engine);
  resourceLoader_->addTextureProvider("image/png", decoder);
  resourceLoader_->addTextureProvider("image/jpeg", decoder);
}

////////////////////////////////////////////////////////////////////////////////////
void ModelSystem::vUpdate(float /*fElapsedTime*/) {
  updateAsyncAssetLoading();
}

////////////////////////////////////////////////////////////////////////////////////
void ModelSystem::vShutdownSystem() {
  destroyAllAssetsOnModels();
  delete resourceLoader_;
  resourceLoader_ = nullptr;

  if (assetLoader_) {
    AssetLoader::destroy(&assetLoader_);
    assetLoader_ = nullptr;
  }
}

////////////////////////////////////////////////////////////////////////////////////
void ModelSystem::DebugPrint() {
  SPDLOG_DEBUG("{} {}", __FILE__, __FUNCTION__);
}

}  // namespace plugin_filament_view
