#version 130

// precision highp float; // needed only for version 1.30

uniform vec3  in_color_ambient;
uniform vec3  in_color_diffuse;
uniform float in_color_opacity;

in vec3 ex_normal;
out vec4 out_color;

void main(void)
{
	vec3 sun_normal = normalize(vec3(0.5, 0.7, 0.5));

	float diffuse_intensity = clamp(dot(ex_normal, sun_normal), 0, 1);
	vec3 color;
	color = in_color_ambient * 0.4;
	color += in_color_diffuse * diffuse_intensity * 0.75;

	out_color = vec4(color, in_color_opacity);
}

