/*
 * Copyright (C) 2019 Open Source Robotics Foundation
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

#version ogre_glsl_ver_330

vulkan_layout( OGRE_POSITION ) in vec4 vertex;

vulkan( layout( ogre_P0 ) uniform Params { )
  uniform mat4 worldViewProj;
  uniform mat4 worldView;
  uniform float ignMinClipDistance;
vulkan( }; )

out gl_PerVertex
{
  vec4 gl_Position;
  float gl_ClipDistance[1];
};

void main()
{
  // Calculate output position
  gl_Position = worldViewProj * vertex;

  if( ignMinClipDistance > 0.0f )
  {
	// Frustum is rectangular; but the minimum lidar distance is spherical,
	// so we can't rely on near plane to clip geometry that is too close,
	// we have to do it by hand
	vec3 viewSpacePos = (worldView * vertex).xyz;
	gl_ClipDistance[0] = length( viewSpacePos.xyz ) - ignMinClipDistance;
  }
  else
  {
	gl_ClipDistance[0] = 1.0f;
  }
}
