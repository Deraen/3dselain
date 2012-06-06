#version 130

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 location;

in vec3 in_Position;
in vec3 in_Normal;
in  vec3 in_Color;
out vec3 ex_Color;

void main(void)
{
	//ex_Color = in_Color;
	ex_Color = vec3(1.0, 1.0, 1.0);
	gl_Position = projection * modelview * location * vec4(in_Position, 1.0);
}
