#version 130

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 location;

in  vec3 in_color;
//in vec3 in_Normal;
in vec3 in_position;
out vec3 ex_color;

void main(void)
{
	gl_Position = projection * modelview * location * vec4(in_position, 1.0);
	ex_color = in_color;
	//ex_Color = vec3(1.0, 1.0, 1.0);
}
