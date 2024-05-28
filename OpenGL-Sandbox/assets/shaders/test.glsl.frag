#version 450 core

layout (location = 0) out vec4 o_Color;

in vec4 v_Color;
in vec2 v_TexCoord;
in vec2 v_TexIndex;

void main()
{
	o_Color = vec4(v_TexCoord, 0.0, 1.0);
}