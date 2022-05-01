/*
 * Copyright (C) 2015 Open Source Robotics Foundation
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
#ifndef IGNITION_RENDERING_BASE_BASENATIVEWINDOW_HH_
#define IGNITION_RENDERING_BASE_BASENATIVEWINDOW_HH_

#include <map>
#include <string>
#include <vector>
#include <ignition/common/SuppressWarning.hh>
#include "ignition/rendering/NativeWindow.hh"
#include "ignition/rendering/Storage.hh"

namespace ignition
{
  namespace rendering
  {
    inline namespace IGNITION_RENDERING_VERSION_NAMESPACE {
    //
    class IGNITION_RENDERING_VISIBLE BaseNativeWindow :
      public virtual NativeWindow
    {
      protected: BaseNativeWindow() {}

      public: virtual ~BaseNativeWindow() {}

      // Documentation Inherited.
      public: virtual void NotifyFocused(bool /*_focused*/) override {}

      // Documentation Inherited.
      public: virtual void NotifyVisible(bool /*_visible*/) override {}

      // Documentation Inherited.
      public: virtual void NotifyWindowMovedOrResized() override {}

      // Documentation Inherited.
      public: virtual void RequestResolution(
            uint32_t /*_width*/, uint32_t /*_height*/) override {}

      // Documentation Inherited.
      public: virtual void Draw(CameraPtr /*_camera*/) override {}
    };
    }
  }
}
#endif
