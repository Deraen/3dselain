// Vertex Shader – file "minimal.vert"

#version 130

// object space to camera space transformation
uniform mat4 modelview_matrix;

// camera space to clip coordinates
uniform mat4 projection_matrix;

in vec3 in_Position;
in vec3 in_Normal;
in  vec3 in_Color;
out vec3 ex_Color;

void main(void)
{
	//ex_Color = in_Color;
	ex_Color = vec3(1.0, 1.0, 1.0);
	gl_Position = projection_matrix * modelview_matrix * vec4(in_Position,1.0);
}
