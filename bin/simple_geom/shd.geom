/* FB1 GLSL shader programm */
#version 460 core

/* Input/putput primitives layout */
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

/* Inputs color */
layout(location = 0) in vec4 InColors[];

/* Outputs fragment color */
layout(location = 0) out vec4 OutColor;

void main( void )
{
  OutColor = InColors[0];

  gl_Position = vec4(-0.5, -0.5, 0.5, 1);
  EmitVertex();

  gl_Position = vec4(-0.5, 0.5, 0.5, 1);
  EmitVertex();

  gl_Position = vec4(0.5, 0.5, 0.5, 1);
  EmitVertex();

  EndPrimitive();
}
