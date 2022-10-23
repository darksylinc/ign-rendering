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
#ifndef GZ_RENDERING_OGRE2_OGRE2LENSFLAREPASS_HH_
#define GZ_RENDERING_OGRE2_OGRE2LENSFLAREPASS_HH_

#include <memory>

#include <gz/utils/ImplPtr.hh>

#include "gz/rendering/base/BaseLensFlarePass.hh"
#include "gz/rendering/ogre2/Ogre2RenderPass.hh"
#include "gz/rendering/ogre2/Export.hh"

namespace gz
{
  namespace rendering
  {
    inline namespace GZ_RENDERING_VERSION_NAMESPACE {
    //
    // forward declaration
    class Ogre2LensFlarePassPrivate;

    /// \brief Ogre2 Implementation of a Lens Flare render pass.
    class GZ_RENDERING_OGRE2_VISIBLE Ogre2LensFlarePass :
      public BaseLensFlarePass<Ogre2RenderPass>
    {
      /// \brief Constructor
      public: Ogre2LensFlarePass();

      /// \brief Destructor
      public: ~Ogre2LensFlarePass() override;

      // Documentation inherited
      public: void PreRender() override;

      // Documentation inherited
      public: void WorkspaceAdded(
            Ogre::CompositorWorkspace *_workspace) override;

      /// \brief Notifies Ogre2RenderPass that a workspace will be destroyed
      /// \param[in] _workspace workspace about to be destroyed
      public: void WorkspaceRemoved(
            Ogre::CompositorWorkspace *_workspace) override;

      /// \cond warning
      /// \brief Private data pointer
      GZ_UTILS_UNIQUE_IMPL_PTR(dataPtr)
      /// \endcond

      private: friend class Ogre2LensFlarePassWorkspaceListenerPrivate;
    };
    }
  }
}
#endif
