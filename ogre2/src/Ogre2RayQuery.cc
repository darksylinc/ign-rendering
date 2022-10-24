/*
 * Copyright (C) 2018 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <gz/common/Console.hh>
#include <gz/common/Mesh.hh>
#include <gz/common/MeshManager.hh>
#include <gz/common/SubMesh.hh>

#include "gz/rendering/ogre2/Ogre2Camera.hh"
#include "gz/rendering/ogre2/Ogre2Conversions.hh"
#include "gz/rendering/ogre2/Ogre2DepthCamera.hh"
#include "gz/rendering/ogre2/Ogre2ObjectInterface.hh"
#include "gz/rendering/ogre2/Ogre2RayQuery.hh"
#include "gz/rendering/ogre2/Ogre2Scene.hh"
#include "gz/rendering/ogre2/Ogre2SegmentationCamera.hh"
#include "gz/rendering/ogre2/Ogre2SelectionBuffer.hh"
#include "gz/rendering/ogre2/Ogre2ThermalCamera.hh"

#ifdef _MSC_VER
  #pragma warning(push, 0)
#endif
#include <OgreCamera.h>
#include <OgreItem.h>
#include <OgreMesh2.h>
#include <OgreRay.h>
#include <OgreSceneManager.h>
#ifdef _MSC_VER
  #pragma warning(pop)
#endif

/// \brief Private data class for Ogre2RayQuery
class gz::rendering::Ogre2RayQueryPrivate
{
  /// \brief Ogre ray scene query object for computing intersection.
  public: Ogre::RaySceneQuery *rayQuery = nullptr;

  //// \brief Pointer to camera
  public: Ogre2CameraPtr camera{nullptr};

  /// \brief Image pos to cast the ray from
  public: math::Vector2i imgPos = math::Vector2i::Zero;

  /// \brief thread that ray query is created in
  public: std::thread::id threadId;

  //// \brief See RayQuery::SetPreferGpu
  public: bool preferGpu;
};

using namespace gz;
using namespace rendering;

//////////////////////////////////////////////////
Ogre2RayQuery::Ogre2RayQuery()
    : dataPtr(new Ogre2RayQueryPrivate)
{
  this->dataPtr->threadId = std::this_thread::get_id();
}

//////////////////////////////////////////////////
Ogre2RayQuery::~Ogre2RayQuery()
{
}

//////////////////////////////////////////////////
void Ogre2RayQuery::SetFromCamera(const CameraPtr &_camera,
    const math::Vector2d &_coord)
{
  // convert to nomalized screen pos for ogre
  math::Vector2d screenPos((_coord.X() + 1.0) / 2.0, (_coord.Y() - 1.0) / -2.0);

  Ogre2CameraPtr camera = std::dynamic_pointer_cast<Ogre2Camera>(_camera);
  if (camera)
  {
    this->dataPtr->camera = camera;
  }

  Ogre2ObjectInterfacePtr ogre2ObjectInterface =
      std::dynamic_pointer_cast<Ogre2ObjectInterface>(_camera);
  if (!ogre2ObjectInterface)
  {
    gzwarn << "Camera does not support ray query\n";
    return;
  }

  Ogre::Ray ray = ogre2ObjectInterface->OgreCamera()->getCameraToViewportRay(
      screenPos.X(), screenPos.Y());

  auto originMath = Ogre2Conversions::Convert(ray.getOrigin());
  if (originMath.IsFinite())
  {
    this->origin = originMath;
  }
  else
  {
    ignwarn << "Attempted to set non-finite origin from camera ["
            << camera->Name() << "]" << std::endl;
  }

  auto directionMath = Ogre2Conversions::Convert(ray.getDirection());
  if (directionMath.IsFinite())
  {
    this->direction = directionMath;
  }
  else
  {
    ignwarn << "Attempted to set non-finite direction from camera ["
            << camera->Name() << "]" << std::endl;
  }

  this->dataPtr->imgPos.X() = static_cast<int>(
      screenPos.X() * _camera->ImageWidth());
  this->dataPtr->imgPos.Y() = static_cast<int>(
      screenPos.Y() * _camera->ImageHeight());
}

//////////////////////////////////////////////////
void Ogre2RayQuery::SetPreferGpu(bool _preferGpu)
{
  this->dataPtr->preferGpu = _preferGpu;
}

//////////////////////////////////////////////////
bool Ogre2RayQuery::UsesGpu() const
{
#ifdef __APPLE__
  return false;
#endif

  if (!this->dataPtr->preferGpu ||         //
      !this->dataPtr->camera ||            //
      !this->dataPtr->camera->Parent() ||  //
      std::this_thread::get_id() != this->dataPtr->threadId)
  {
    return false;
  }

  return true;
}

//////////////////////////////////////////////////
RayQueryResult Ogre2RayQuery::ClosestPoint(bool _forceSceneUpdate)
{
  if (!this->UsesGpu())
  {
    // use legacy method for backward compatibility if no camera is set or
    // camera is not attached in the scene tree or
    // this function is called from non-rendering thread
    return this->ClosestPointByIntersection(_forceSceneUpdate);
  }
  else
  {
    // the VisualAt function is a hack to force creation of the selection
    // buffer object
    // todo(anyone) Make Camera::SetSelectionBuffer function public?
    if (!this->dataPtr->camera->SelectionBuffer())
      this->dataPtr->camera->VisualAt(math::Vector2i(0, 0));

    return this->ClosestPointBySelectionBuffer();
  }
}

//////////////////////////////////////////////////
RayQueryResult Ogre2RayQuery::ClosestPointBySelectionBuffer()
{
  // update selection buffer dimension in case window is resized
  this->dataPtr->camera->SelectionBuffer()->SetDimensions(
    this->dataPtr->camera->ImageWidth(), this->dataPtr->camera->ImageHeight());

  RayQueryResult result;
  Ogre::Item *ogreItem = nullptr;
  math::Vector3d point;
  bool success = this->dataPtr->camera->SelectionBuffer()->ExecuteQuery(
      this->dataPtr->imgPos.X(), this->dataPtr->imgPos.Y(), ogreItem, point);
  result.distance = -1;

  if (success && ogreItem)
  {
    if (!ogreItem->getUserObjectBindings().getUserAny().isEmpty() &&
        ogreItem->getUserObjectBindings().getUserAny().getType() ==
        typeid(unsigned int))
    {
      auto userAny = ogreItem->getUserObjectBindings().getUserAny();
      double distance = this->dataPtr->camera->WorldPosition().Distance(point)
          - this->dataPtr->camera->NearClipPlane();
      if (!std::isinf(distance))
      {
        result.distance = distance;
        result.point = point;
        result.objectId = Ogre::any_cast<unsigned int>(userAny);
      }
    }
  }
  return result;
}

//////////////////////////////////////////////////
RayQueryResult Ogre2RayQuery::ClosestPointByIntersection(bool _forceSceneUpdate)
{
  RayQueryResult result;
  Ogre2ScenePtr ogreScene =
      std::dynamic_pointer_cast<Ogre2Scene>(this->Scene());
  if (!ogreScene)
    return result;

  if (_forceSceneUpdate)
  {
    ogreScene->OgreSceneManager()->updateSceneGraph();
  }

  const Ogre::Vector3 rayOrigin = Ogre2Conversions::Convert(this->origin);
  const Ogre::Vector3 rayDir = Ogre2Conversions::Convert(this->direction);

  Ogre::Ray mouseRay(rayOrigin, rayDir);

  if (!this->dataPtr->rayQuery)
  {
    this->dataPtr->rayQuery =
        ogreScene->OgreSceneManager()->createRayQuery(mouseRay);
  }
  this->dataPtr->rayQuery->setSortByDistance(true);
  this->dataPtr->rayQuery->setRay(mouseRay);

  // Perform the scene query
  Ogre::RaySceneQueryResult &ogreResult = this->dataPtr->rayQuery->execute();

  double distance = std::numeric_limits<double>::max();

  // Iterate over all the results.
  for (auto iter = ogreResult.begin(); iter != ogreResult.end(); ++iter)
  {
    if (iter->distance <= 0.0)
      continue;

    if (!iter->movable || !iter->movable->getVisible())
      continue;

    auto userAny = iter->movable->getUserObjectBindings().getUserAny();
    if (!userAny.isEmpty() && userAny.getType() == typeid(unsigned int) &&
        iter->movable->getMovableType() == "Item")
    {
      Ogre::Item *ogreItem = static_cast<Ogre::Item *>(iter->movable);

      // mesh factory creates name with ::CENTER or ::ORIGINAL depending on
      // the params passed in the MeshDescriptor when loading the mesh
      // so strip off the suffix
      std::string meshName = ogreItem->getMesh()->getName();
      size_t idx = meshName.find("::");
      if (idx != std::string::npos)
        meshName = meshName.substr(0, idx);

      const common::Mesh *mesh =
           common::MeshManager::Instance()->MeshByName(meshName);

      if (!mesh)
        continue;

      Ogre::Matrix4 transform = ogreItem->_getParentNodeFullTransform();

      const bool bIsAffine = transform.isAffine();

      if (bIsAffine)
      {
        Ogre::Matrix4 invTransform = transform.inverse();
        Ogre::Matrix3 invTransform3x3;
        invTransform.extract3x3Matrix(invTransform3x3);
        mouseRay = Ogre::Ray(invTransform * rayOrigin,
                             (invTransform3x3 * rayDir).normalisedCopy());
      }
      else
      {
        mouseRay = Ogre::Ray(rayOrigin, rayDir);
      }

      // test for hitting individual triangles on the mesh
      for (unsigned int j = 0; j < mesh->SubMeshCount(); ++j)
      {
        auto s = mesh->SubMeshByIndex(j);
        auto submesh = s.lock();
        if (!submesh || submesh->VertexCount() < 3u)
          continue;
        unsigned int indexCount = submesh->IndexCount();

        const gz::math::Vector3d *RESTRICT_ALIAS vertices =
          submesh->VertexPtr();
        const unsigned int *RESTRICT_ALIAS indices = submesh->IndexPtr();

        std::pair<bool, Ogre::Real> bestHit = {
          false, std::numeric_limits<Ogre::Real>::max()
        };

        for (unsigned int k = 0; k < indexCount; k += 3)
        {
          if (indexCount <= k+2)
            continue;

#ifdef SLOW_METHOD
          gz::math::Vector3d vertexA =
            submesh->Vertex(submesh->Index(k));
          gz::math::Vector3d vertexB =
            submesh->Vertex(submesh->Index(k+1));
          gz::math::Vector3d vertexC =
            submesh->Vertex(submesh->Index(k+2));

          Ogre::Vector3 worldVertexA =
              transform * Ogre2Conversions::Convert(vertexA);
          Ogre::Vector3 worldVertexB =
              transform * Ogre2Conversions::Convert(vertexB);
          Ogre::Vector3 worldVertexC =
              transform * Ogre2Conversions::Convert(vertexC);
#else
          Ogre::Vector3 worldVertexA, worldVertexB, worldVertexC;

          if (bIsAffine)
          {
            worldVertexA = Ogre2Conversions::Convert(vertices[indices[k]]);
            worldVertexB = Ogre2Conversions::Convert(vertices[indices[k + 1]]);
            worldVertexC = Ogre2Conversions::Convert(vertices[indices[k + 2]]);
          }
          else
          {
            worldVertexA =
              transform * Ogre2Conversions::Convert(vertices[indices[k]]);
            worldVertexB =
              transform * Ogre2Conversions::Convert(vertices[indices[k + 1]]);
            worldVertexC =
              transform * Ogre2Conversions::Convert(vertices[indices[k + 2]]);
          }
#endif

          // check for a hit against this triangle
          std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(mouseRay,
              worldVertexA, worldVertexB, worldVertexC,
              true, false);

          // if it was a hit check if its the closest
          if (hit.first && hit.second < bestHit.second)
          {
            bestHit = hit;
          }
        }

        if (bestHit.first && distance > bestHit.second)
        {
          // this is the closest so far, save it off
          distance = bestHit.second;
          result.distance = distance;
          if (bIsAffine)
          {
            result.point = Ogre2Conversions::Convert(
              transform * mouseRay.getPoint(distance));
          }
          else
          {
            result.point =
              Ogre2Conversions::Convert(mouseRay.getPoint(distance));
          }
          result.objectId = Ogre::any_cast<unsigned int>(userAny);
        }
      }
    }
  }

  return result;
}
