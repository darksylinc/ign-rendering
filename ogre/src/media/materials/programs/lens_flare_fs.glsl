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

// Viewport's aspect ratio
uniform float vpAspectRatio;

// light pos in clip space
uniform vec3 lightPos;

// lens flare scale
uniform float scale;

// lens flare color
uniform vec3 color;

uniform sampler2D srcTex;

vec3 lensflare(vec2 uv,vec2 pos)
{
  vec2 main = uv-pos;

  vec2 uvd = uv*length(uv);

  float dist = length(main); dist = pow(dist,.1);

  float f0 = 1.0/(length(uv-pos)*16.0/scale+1.0);

  float f2 = max(1.0/(1.0+32.0*pow(length(uvd+0.8*pos),2.0)),.0)*00.25;
  float f22 = max(1.0/(1.0+32.0*pow(length(uvd+0.85*pos),2.0)),.0)*00.23;
  float f23 = max(1.0/(1.0+32.0*pow(length(uvd+0.9*pos),2.0)),.0)*00.21;

  vec2 uvx = mix(uv,uvd,-0.5);

  float f4 = max(0.01-pow(length(uvx+0.4*pos),2.4),.0)*6.0;
  float f42 = max(0.01-pow(length(uvx+0.45*pos),2.4),.0)*5.0;
  float f43 = max(0.01-pow(length(uvx+0.5*pos),2.4),.0)*3.0;

  uvx = mix(uv,uvd,-.4);

  float f5 = max(0.01-pow(length(uvx+0.2*pos),5.5),.0)*2.0;
  float f52 = max(0.01-pow(length(uvx+0.4*pos),5.5),.0)*2.0;
  float f53 = max(0.01-pow(length(uvx+0.6*pos),5.5),.0)*2.0;

  uvx = mix(uv,uvd,-0.5);

  float f6 = max(0.01-pow(length(uvx-0.3*pos),1.6),.0)*6.0;
  float f62 = max(0.01-pow(length(uvx-0.325*pos),1.6),.0)*3.0;
  float f63 = max(0.01-pow(length(uvx-0.35*pos),1.6),.0)*5.0;

  vec3 c = vec3(.0);

  c.r+=f2+f4+f5+f6; c.g+=f22+f42+f52+f62; c.b+=f23+f43+f53+f63;
  c *= min(scale, 0.2)/0.2;
  c+=vec3(f0);

  return c;
}

void main()
{
  if (lightPos.z < 0.0)
  {
    // light is behind the view
    gl_FragColor = texture2D(srcTex, gl_TexCoord[0].xy);
  }
  else
  {
    vec3 pos = lightPos;

    // flip y
    pos.y *= -1.0;

    vec2 uv = gl_TexCoord[0].xy - 0.5;
    // scale lightPos to be same range as uv
    pos *= 0.5;
    // fix aspect ratio
    uv.x *= vpAspectRatio;
    pos.x *= vpAspectRatio;

    // compute lens flare
    vec3 finalColor = color * lensflare(uv, pos.xy);

    // apply lens flare
    gl_FragColor = texture2D(srcTex, gl_TexCoord[0].xy) +
                   vec4(finalColor, 1.0);
  }
}
