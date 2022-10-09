/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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
#ifndef GZ_RENDERING_OGRE2_WIDEANGLECAMERA_HH_
#define GZ_RENDERING_OGRE2_WIDEANGLECAMERA_HH_

#include <memory>
#include <string>

#include <gz/utils/ImplPtr.hh>

#include "gz/rendering/base/BaseWideAngleCamera.hh"
#include "gz/rendering/ogre2/Export.hh"
#include "gz/rendering/ogre2/Ogre2RenderTarget.hh"
#include "gz/rendering/ogre2/Ogre2Sensor.hh"

namespace gz
{
  namespace rendering
  {
    inline namespace GZ_RENDERING_VERSION_NAMESPACE {
    //
    /// \brief Ogre implementation of WideAngleCamera
    class GZ_RENDERING_OGRE2_VISIBLE Ogre2WideAngleCamera :
        public BaseWideAngleCamera<Ogre2Sensor>
    {
      /// \brief Constructor
      protected: Ogre2WideAngleCamera();

      /// \brief Destructor
      public: virtual ~Ogre2WideAngleCamera() override;

      // Documentation inherited
      public: virtual void Init() override;

      /// \brief Create a texture
      public: virtual void CreateRenderTexture();

      /// \brief Render the camera
      public: virtual void PostRender() override;

      // Documentation inherited
      public: virtual void Destroy() override;

      /// \brief Gets the environment texture size
      /// \return Texture size
      public: uint32_t EnvTextureSize() const;

      /// \brief Sets environment texture size
      /// \param[in] _size Texture size
      public: void SetEnvTextureSize(uint32_t _size);

      /// \brief Project 3D world coordinates to screen coordinates
      /// \param[in] _pt 3D world coodinates
      /// \return Screen coordinates. Z is the distance of point from camera
      /// optical center.
      public: math::Vector3d Project3d(const math::Vector3d &_pt) const
          override;

      // Documentation inherited.
      public: virtual void PreRender() override;

      /// \brief Implementation of the render call
      public: virtual void Render() override;

      // Documentation inherited
      public: common::ConnectionPtr ConnectNewWideAngleFrame(
          std::function<void(const unsigned char *, unsigned int, unsigned int,
          unsigned int, const std::string &)>  _subscriber) override;

      /// \brief Set the camera's render target
      protected: void CreateWideAngleTexture() override;

      /// \brief Create the camera.
      protected: void CreateCamera();

      /// \brief Get a pointer to the render target.
      /// \return Pointer to the render target
      protected: virtual RenderTargetPtr RenderTarget() const override;

      /// \brief Changes the Compositor Definition to use the MSAA
      /// settings we need. Do not call this if not using MSAA.
      /// \param[in] _ogreCompMgr Ogre's Compositor Manager
      /// \param[in] _msaa Value in range [2; 256)
      protected: void SetupMSAA(Ogre::CompositorManager2 *_ogreCompMgr,
                                uint8_t _msaa);

      /// \brief Saves the CompositorPassSceneDef of each of the 6 passes
      /// defined in WideAngleCamera.compositor data file for later
      /// manipulation.
      /// \param[in] _ogreCompMgr Ogre's Compositor Manager
      /// \param[in] _withMsaa Whether the version we're retrieving is the MSAA
      /// one
      private: void RetrieveCubePassSceneDefs(
            Ogre::CompositorManager2 *_ogreCompMgr, bool _withMsaa);

      /// \brief Called before each cubemap face is about to be rendered
      /// \param[in] _pass Compositor pass to set its clear colour
      private: void PrepareForCubemapFacePass(Ogre::CompositorPass *_pass);

      /// \brief Called before the final pass is about to be rendered
      /// \param[in] _pass Material Pass to setup
      private: void PrepareForFinalPass(Ogre::Pass *_pass);

      /// \cond warning
      /// \brief Private data pointer
      GZ_UTILS_UNIQUE_IMPL_PTR(dataPtr)
      /// \endcond

      private: friend class Ogre2Scene;
      private: friend class Ogre2WideAngleCameraWorkspaceListenerPrivate;
    };
    }
  }
}
#endif
