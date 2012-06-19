#version 130

// precision highp float; // needed only for version 1.30

in  vec3 ex_color;
out vec4 out_color;

void main(void)
{
	out_color = vec4(ex_color, 1.0);
}
