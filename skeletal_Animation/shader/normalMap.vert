#version 330 core

layout (location = 0) in vec3   aPosition;
layout (location = 1) in vec3   aNormal;
layout (location = 2) in vec2   aTexCoord;
layout (location = 3) in vec3   aTangent;

out	vec3	FragPos;
out vec2    TexCoords;
out vec3	Normal;
out vec3	Tangent;

uniform mat4	view;
uniform mat4	model;

void    main()
{
	FragPos = (model * vec4(aPosition, 1.0)).xyz;
	gl_Position = view * vec4(FragPos, 1.0);
	TexCoords = aTexCoord;

	mat4	InverseModel = transpose(inverse(model));
	Normal = (InverseModel * vec4(aNormal, 0.0)).xyz;
	Tangent = (InverseModel * vec4(aTangent, 0.0)).xyz;
}
