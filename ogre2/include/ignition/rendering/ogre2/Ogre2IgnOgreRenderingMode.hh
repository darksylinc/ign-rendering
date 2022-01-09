/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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
#ifndef IGNITION_RENDERING_OGRE2_OGRE2IGNOGRERENDERINGMODE_HH_
#define IGNITION_RENDERING_OGRE2_OGRE2IGNOGRERENDERINGMODE_HH_

#include "ignition/rendering/config.hh"
#include "ignition/rendering/ogre2/Export.hh"

namespace ignition
{
  namespace rendering
  {
    inline namespace IGNITION_RENDERING_VERSION_NAMESPACE
    {
      /// \brief Rendering modes so that IgnHlms implementations
      /// follow alternate code paths or extra customizations
      /// when they're enabled
      enum IgnOgreRenderingMode
      {
        /// \brief Normal rendering
        IORM_NORMAL,

        /// \brief Render a solid color explicit per object.
        /// Used by e.g. Segmentation camera mode
        IORM_SOLID_COLOR,

        /// \brief Render a solid color explicit per object.
        /// But Unlit won't do that.
        /// Used by thermal camera
        IORM_SOLID_COLOR_NOT_UNLIT,

        /// \brief Total number of rendering modes
        IORM_COUNT,
      };
    }  // namespace IGNITION_RENDERING_VERSION_NAMESPACE
  }    // namespace rendering
}  // namespace ignition

#endif
