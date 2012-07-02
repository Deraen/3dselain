#version 130

// precision highp float; // needed only for version 1.30

//in vec4 in_color_diffuse;
//in vec4 in_color_ambient;
in vec4 ex_color;
//in vec3 ex_normal;
out vec4 out_color;

void main(void)
{
	//vec3 sun_normal = normalize(vec3(0.5, -0.7, 0.5));
	out_color = ex_color;
}
