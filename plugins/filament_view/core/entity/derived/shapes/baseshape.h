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

#pragma once

#include "shell/platform/common/client_wrapper/include/flutter/encodable_value.h"

#include <core/components/derived/basetransform.h>
#include <core/components/derived/commonrenderable.h>
#include <core/entity/base/entityobject.h>
#include <core/entity/derived/renderable_entityobject.h>
#include <core/include/shapetypes.h>
#include <core/scene/geometry/direction.h>
#include <core/systems/derived/material_system.h>
#include <filament/IndexBuffer.h>
#include <filament/VertexBuffer.h>
#include <utils/EntityManager.h>

namespace plugin_filament_view {

class CollisionSystem;
using ::utils::Entity;

namespace shapes {

class BaseShape : public RenderableEntityObject {
  friend class plugin_filament_view::CollisionSystem;

 public:
  explicit BaseShape(const flutter::EncodableMap& params);

  BaseShape();

  ~BaseShape() override;

  virtual void DebugPrint(const char* tag) const;

  // Disallow copy and assign.
  BaseShape(const BaseShape&) = delete;
  BaseShape& operator=(const BaseShape&) = delete;

  // will copy over properties, but not 'create' anything.
  // similar to a shallow copy.
  virtual void CloneToOther(BaseShape& other) const;

  virtual bool bInitAndCreateShape(::filament::Engine* engine_,
                                   std::shared_ptr<Entity> entityObject) = 0;

  void vRemoveEntityFromScene() const;
  void vAddEntityToScene() const;

  [[nodiscard]] std::shared_ptr<Entity> poGetEntity() { return m_poEntity; }

 protected:
  ::filament::VertexBuffer* m_poVertexBuffer;
  ::filament::IndexBuffer* m_poIndexBuffer;

  void DebugPrint() const override;

  // uses Vertex and Index buffer to create the material and geometry
  // using all the internal variables.
  void vBuildRenderable(::filament::Engine* engine_);

  ShapeType type_{};

  // Components - saved off here for faster
  // lookup, but they're not owned here, but on EntityObject's list.
  std::weak_ptr<BaseTransform> m_poBaseTransform;
  std::weak_ptr<CommonRenderable> m_poCommonRenderable;

  /// direction of the shape rotation in the world space
  filament::math::float3 m_f3Normal;
  /// material to be used for the shape - instantiated from material definition
  /// This should probably be on the entity level as model would use this in
  /// future as well.
  Resource<filament::MaterialInstance*> m_poMaterialInstance;

  std::shared_ptr<utils::Entity> m_poEntity;

  // Whether we have winding indexes in both directions.
  bool m_bDoubleSided = false;

  // TODO - Note this is backlogged for using value.
  //        For now this is unimplemented, but would be a <small> savings
  //        when building as code currently allocates buffers for UVs
  bool m_bHasTexturedMaterial = true;

  void vChangeMaterialDefinitions(const flutter::EncodableMap& params,
                                  const TextureMap& loadedTextures) override;
  void vChangeMaterialInstanceProperty(
      const MaterialParameter* materialParam,
      const TextureMap& loadedTextures) override;

 private:
  void vDestroyBuffers();

  // This does NOT come over as a property (currently), only used by
  // CollisionManager when created debug wireframe models for seeing collidable
  // shapes.
  bool m_bIsWireframe = false;

  void vLoadMaterialDefinitionsToMaterialInstance();
};

}  // namespace shapes
}  // namespace plugin_filament_view
