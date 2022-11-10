#version 460 core

out vec4    FragColor;
  
in vec2 TexCoords;

uniform sampler2D   gPosition;
uniform sampler2D   gNormal;
uniform sampler2D   gAlbedoSpec;

struct PointLight
{
    vec3	position;
    vec3	ambient;
    vec3	diffuse;
    vec3	specular;
};
struct DirectionLight
{
    vec3	direction;
    vec3	ambient;
    vec3	diffuse;
    vec3	specular;
};
struct SpotLight
{
    vec3	position;
    vec3	direction;
    vec2	cutoff;
    vec3	ambient;
    vec3	diffuse;
    vec3	specular;
};

uniform bool            lightswitch[3];
uniform PointLight		pointLight;
uniform DirectionLight	directionLight;
uniform SpotLight		spotLight;
uniform float           shininess;

uniform vec3    viewPos;

vec3	CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3	CalcDirectionLight(DirectionLight light, vec3 normal, vec3 viewDir);
vec3	CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void    main()
{
    vec3    FragPos = texture(gPosition, TexCoords).rgb;
    vec3    Noraml = texture(gNormal, TexCoords).rgb;

    vec3	viewDir = normalize(viewPos - FragPos);
	vec3	result = vec3(0.0);

    if (lightswitch[0])
	    result += CalcPointLight(pointLight, Noraml, FragPos, viewDir);
    if (lightswitch[1])
	    result += CalcDirectionLight(directionLight, Noraml, viewDir);
    if (lightswitch[2])
	    result += CalcSpotLight(spotLight, Noraml, FragPos, viewDir);
    FragColor = vec4(result, 1.0);
};


vec3	CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	float	constant = 1.0f;
    float   linear = 0.09f;
    float   quadratic = 0.032f;

    vec3	lightDir = normalize(light.position - fragPos);
	
    float	diff = max(dot(normal, lightDir), 0.0);

    vec3	reflectDir = reflect(-lightDir, normal);
    vec3	halfwayDir = normalize(lightDir + viewDir);
    float	spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	
    float	distance = length(light.position - fragPos);
    float	attenuation = 1.0 / (constant + (linear * distance) + (quadratic * distance * distance));

    vec3    Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float   Mspec = texture(gAlbedoSpec, TexCoords).a;

    vec3	ambient = light.ambient  * Albedo;
    vec3	diffuse = light.diffuse  * diff * Albedo;
    vec3	specular = light.specular * spec * vec3(Mspec);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
};

vec3	CalcDirectionLight(DirectionLight light, vec3 normal, vec3 viewDir)
{
    vec3	lightDir = normalize(-light.direction);
	
    float	diff = max(dot(normal, lightDir), 0.0);
	
    vec3	reflectDir = reflect(-lightDir, normal);
    vec3	halfwayDir = normalize(lightDir + viewDir);
    float	spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	
    vec3    Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float   Mspec = texture(gAlbedoSpec, TexCoords).a;

    vec3	ambient = light.ambient  * Albedo;
    vec3	diffuse = light.diffuse  * diff * Albedo;
    vec3	specular = light.specular * spec * vec3(Mspec);
    return (ambient + diffuse + specular);
};

vec3	CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	float	constant = 1.0f;
    float   linear = 0.09f;
    float   quadratic = 0.032f;

    vec3	lightDir = normalize(light.position - fragPos);
	
    float	diff = max(dot(normal, lightDir), 0.0);
	
    vec3	reflectDir = reflect(-lightDir, normal);
    vec3	halfwayDir = normalize(lightDir + viewDir);
    float	spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	
    float	distance = length(light.position - fragPos);
    float	attenuation = 1.0 / (constant + (linear * distance) + (quadratic * distance * distance));

    float	theta = dot(lightDir, normalize(-light.direction)); 
    float	epsilon = light.cutoff[0] - light.cutoff[1];
    float	intensity = clamp((theta - light.cutoff[1]) / epsilon, 0.0, 1.0);

    vec3    Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float   Mspec = texture(gAlbedoSpec, TexCoords).a;

    vec3	ambient = light.ambient  * Albedo;
    vec3	diffuse = light.diffuse  * diff * Albedo;
    vec3	specular = light.specular * spec * vec3(Mspec);
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
};