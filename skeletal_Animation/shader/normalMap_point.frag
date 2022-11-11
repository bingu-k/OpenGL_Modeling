#version 330 core

in vec3     FragPos;
in vec2     TexCoords;
in vec3     Normal;
in vec3     Tangent;

out vec4    FragColor;

struct PointLight
{
    vec3	position;
    vec3	ambient;
    vec3	diffuse;
    vec3	specular;
};

struct Material
{
	sampler2D	diffuse;
	sampler2D	specular;
	sampler2D	normal;
	float		shininess;
};

uniform int             numPointLight;
uniform PointLight		pointLight[100];

uniform Material	material;

uniform vec3	viewPos;

vec3	CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void    main()
{
    vec3    norm = texture(material.normal, TexCoords).rgb;
    norm = normalize(norm * 2.0 - 1.0);
    vec3    T = normalize(Tangent);
    vec3    N = normalize(Normal);
    vec3    B = cross(N, T);
    mat3    TBN = mat3(T, B, N);
    norm = normalize(TBN * norm);

    vec3	viewDir = normalize(viewPos - FragPos);
	vec3	result = vec3(0.0);

    for (int i = 0; i < numPointLight; ++i)
	    result += CalcPointLight(pointLight[i], norm, FragPos, viewDir);
    FragColor = vec4(result, 1.0);
}

vec3	CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	float	constant = 1.0f;
    float   linear = 0.09f;
    float   quadratic = 0.032f;

    vec3	lightDir = normalize(light.position - fragPos);
	
    float	diff = max(dot(normal, lightDir), 0.0);
	
    vec3	reflectDir = reflect(-lightDir, normal);
    vec3	halfwayDir = normalize(lightDir + viewDir);
    float	spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	
    float	distance = length(light.position - fragPos);
    float	attenuation = 1.0 / (constant + (linear * distance) + (quadratic * distance * distance));

    vec3	ambient = light.ambient  * texture(material.diffuse, TexCoords).rgb;
    vec3	diffuse = light.diffuse  * diff * texture(material.diffuse, TexCoords).rgb;
    vec3	specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
};
