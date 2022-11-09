#version 460 core

layout (location = 0) out vec3  gPosition;
layout (location = 1) out vec3  gNormal;
layout (location = 2) out vec4  gAlbedoSpec;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 Tangent;

struct Material
{
	sampler2D	diffuse;
	sampler2D	specular;
	sampler2D	normal;
	float		shininess;
};

uniform Material    material;

void    main()
{
    gPosition = FragPos;

    vec3    norm = texture(material.normal, TexCoords).rgb;
    norm = normalize(norm * 2.0 - 1.0);
    vec3    T = normalize(Tangent);
    vec3    N = normalize(Normal);
    vec3    B = cross(N, T);
    mat3    TBN = mat3(T, B, N);
    gNormal = normalize(TBN * norm);
    
    gAlbedoSpec.rgb = texture(material.diffuse, TexCoords).rgb;

    gAlbedoSpec.a = texture(material.specular, TexCoords).r;
}