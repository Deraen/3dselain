#version 130

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 location;

in vec3 in_normal;
in vec3 in_position;

out vec3 ex_normal;

void main(void)
{
	gl_Position = projection * modelview * location * vec4(in_position, 1.0);
	ex_normal = in_normal;
}
