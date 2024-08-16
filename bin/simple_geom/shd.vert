/* Copyright 2024 Fedor Borodulin

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
 */

/* "simple_geom/shd.vert" - GLSL shader with system testing purpose, vertex stage file */

#version 460 core

/* Inputs coordinate and color as input */
layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec4 InColor;

/* Outputs color for interpolation */
layout(location = 0) out vec4 OutColor;

/* Shader entry point */
void main( void )
{
  gl_Position = vec4(InPosition, 1);
  OutColor = InColor;
} /* End of 'main' function */

/* END OF 'shd.vert' FILE */
