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

#include "sphere.h"

#include <filament/RenderableManager.h>
#include <filament/IndexBuffer.h>
#include <filament/VertexBuffer.h>
#include <math/mat3.h>
#include <math/norm.h>
#include <math/vec3.h>
#include <vector>

#include "core/utils/deserialize.h"
#include "plugins/common/common.h"

namespace plugin_filament_view {
namespace shapes {

using ::filament::IndexBuffer;
using ::filament::RenderableManager;
using ::filament::VertexBuffer;
using ::filament::VertexAttribute;
using ::filament::math::float3;
using ::filament::math::mat3f;
using ::utils::Entity;

Sphere::Sphere(const std::string& flutter_assets_path,
               const flutter::EncodableMap& params) 
    : BaseShape(flutter_assets_path, params) {
  SPDLOG_TRACE("+-{} {}", __FILE__, __FUNCTION__);

    for (auto& it : params) {
        auto key = std::get<std::string>(it.first);
        
    if (key == "stacks" && std::holds_alternative<int>(it.second)) {
        stacks_ = std::get<int>(it.second);
        } else if (key == "slices" &&
                std::holds_alternative<int32_t>(it.second)) {
        slices_ = std::get<int>(it.second);
        }
    }
}

bool Sphere::bInitAndCreateShape(::filament::Engine* engine_,
                                 std::shared_ptr<Entity> entityObject,
                                 MaterialManager* material_manager) {
  m_poEntity = std::move(entityObject);

  m_poVertexBuffer = nullptr;
  m_poIndexBuffer = nullptr;

  if (m_bDoubleSided) {
    createDoubleSidedSphere(engine_, material_manager);
  } else {
    createSingleSidedSphere(engine_, material_manager);
  }

  return true;
}

void Sphere::createSingleSidedSphere(::filament::Engine* engine_,
                                     MaterialManager* material_manager) {

    const int sectors = slices_;  // Longitude, or number of vertical slices
    const int stacks = stacks_;   // Latitude, or number of horizontal slices

    float sectorStep = 2 * M_PI / sectors;
    float stackStep = M_PI / stacks;
    float sectorAngle, stackAngle;

    // Generate vertices and normals for the outer surface
    for (int i = 0; i <= stacks; ++i) {
        stackAngle = M_PI / 2.0f - (float)i * stackStep;  // from pi/2 to -pi/2
        float xy = cosf(stackAngle);                // r * cos(u)
        float z = sinf(stackAngle);                 // r * sin(u)

        for (int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep;     // from 0 to 2pi

            float  x = xy * cosf(sectorAngle);             // x = r * cos(u) * cos(v)
            float y = xy * sinf(sectorAngle);             // y = r * cos(u) * sin(v)

            vertices.push_back(float3{x, y, z});

            float length = sqrt(x * x + y * y + z * z);
            if(length == 0) length = 0.01f;

            normals.push_back(float3{x/length, y/length, z/length});
        }
    }

    // Generate indices for the outer surface
    for (int i = 0; i < stacks; ++i) {
      int k1 = i * (sectors + 1);  // Beginning of current stack
      int k2 = k1 + sectors + 1;   // Beginning of next stack

      for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            // Middle area triangles
            indices.push_back(static_cast<uint16_t>(k1));
            indices.push_back(static_cast<uint16_t>(k2));
            indices.push_back(static_cast<uint16_t>(k1 + 1));

            indices.push_back(static_cast<uint16_t>(k1 + 1));
            indices.push_back(static_cast<uint16_t>(k2));
            indices.push_back(static_cast<uint16_t>(k2 + 1));
      } 
    }
  
    // Create the vertex buffer
    m_poVertexBuffer = VertexBuffer::Builder()
                           .vertexCount(vertices.size())
                           .bufferCount(1)
                           .attribute(VertexAttribute::POSITION, 0,
                                      VertexBuffer::AttributeType::FLOAT3)
                           .attribute(VertexAttribute::TANGENTS, 1,
                                      VertexBuffer::AttributeType::FLOAT3)
                           //.normalized(VertexAttribute::TANGENTS)
                           .build(*engine_);

    // Set buffer data
    m_poVertexBuffer->setBufferAt(
        *engine_, 0,
        VertexBuffer::BufferDescriptor(vertices.data(), vertices.size() * sizeof(float) * 3), 0);
    m_poVertexBuffer->setBufferAt(
         *engine_, 1,
         VertexBuffer::BufferDescriptor(normals.data(), normals.size() * sizeof(float3)));

    // Create the index buffer
    int indexCount = indices.size();
    m_poIndexBuffer = IndexBuffer::Builder()
                          .indexCount(indexCount)
                          .bufferType(IndexBuffer::IndexType::USHORT)
                          .build(*engine_);

    m_poIndexBuffer->setBuffer(
        *engine_, IndexBuffer::BufferDescriptor(indices.data(), indices.size() * sizeof(unsigned short)));

    vBuildRenderable(engine_, material_manager);
}


void Sphere::createDoubleSidedSphere(::filament::Engine* engine_,
                                     MaterialManager* material_manager) {
// createDoubleSidedSphere - Same geometry, but do stack winding opposite and positive on indice creation.
    spdlog::warn("createDoubleSidedSphere not implemented.");
}

void Sphere::Print(const char* tag) const {
  BaseShape::Print(tag);
}

}  // namespace shapes
}  // namespace plugin_filament_view