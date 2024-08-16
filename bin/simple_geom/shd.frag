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

/* "simple_geom/shd.frag" - GLSL shader with system testing purpose, fragment stage file */

#version 460 core

/* Inputs interpolated color */
layout(location = 0) in vec4 InColor;

/* Outputs fragment color */
layout(location = 0) out vec4 OutColor;

/* Shader entry point */
void main( void )
{
  OutColor = InColor;
} /* End of 'main' function */

/* END OF 'shd.frag' FILE */
