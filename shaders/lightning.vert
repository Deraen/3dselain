#version 130

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 location;

in vec3 in_color_ambient;
in vec3 in_color_diffuse;
//in vec3 in_color_specular;
in vec3 in_normal;
in vec3 in_position;
//out vec3 ex_normal;
out vec3 ex_color;

void main(void)
{
	vec3 sun_normal = normalize(vec3(0.5, 0.7, 0.5));

	gl_Position = projection * modelview * location * vec4(in_position, 1.0);
	//ex_normal = in_normal;
	//ex_normal = normalize((modelview * vec4(in_normal, 1.0)).xyz);

	float diffuse_intensity = clamp(dot(in_normal, sun_normal), 0, 1);

	//ex_color = in_color_ambient;
	ex_color += in_color_diffuse * diffuse_intensity;
}
