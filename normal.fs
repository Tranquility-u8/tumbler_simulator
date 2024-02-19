#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

uniform int nr_point;
struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
#define NR_MAX_POINT_LIGHT 10
uniform PointLight plights[NR_MAX_POINT_LIGHT];

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dlight;

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutoff;
    float outcutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform SpotLight slight;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 cameraPos;

uniform float lightingMode; // 0 for pure color, 1 for texture, 2 for light
const vec3 LightColor = vec3(0.5, 0.5, 0.5);

uniform samplerCube depthMap;
uniform float far_plane;
uniform bool shadows;
const float bias = 0.25;

#define NR_COLOR 6
const vec3 colors[NR_COLOR] = vec3[](
    vec3(1.0, 1.0, 1.0), // White
    vec3(0.8, 0.0, 0.0), // Red
    vec3(0.0, 0.8, 0.0), // Green
    vec3(0.0, 0.0, 0.8), // Blue
    vec3(0.9, 0.7, 0.5), // Board
    vec3(0.98, 0.98, 0.90)  // Fire
);
uniform float colorId;

float ShadowCalculation(vec3 fragPos);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcPointLight_(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

#define NR_SAMPLE 20
vec3 sampleGrid[NR_SAMPLE] = vec3[](
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

void main()
{	
    vec3 result;
    vec3 viewDir = normalize(cameraPos - FragPos);

    int id = int(colorId);

    if(lightingMode == 0.0){
        
        vec3 color;
        color = colors[id];
        
        for (int i = 0; i < nr_point; i++){
            result += CalcPointLight_(plights[i], Normal, FragPos, viewDir, color);
        }
        
    } else if(lightingMode == 1.0){
       
        for (int i = 0; i < nr_point; i++){
            result += CalcPointLight(plights[i], Normal, FragPos, viewDir);
        }
    
    } else if(lightingMode == 2.0){
        result = colors[id];
    }
    
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    if(lightingMode == 2.0)
        BrightColor = vec4(colors[0], 1.0);

    FragColor = vec4(result, 1.0);
}

float ShadowCalculation(vec3 fragPos)
{
    float shadow = 0.0;
    vec3 fragToLight = fragPos - plights[0].position;
    float currentDepth = length(fragToLight);
    float viewDistance = length(cameraPos - fragPos);
    //float sampleRadius = 0.04;
    float sampleRadius = (1.0 + (viewDistance / far_plane)) / 30.0;
    for(int i = 0; i < NR_SAMPLE; i++)
    {
        float closestDepth = texture(depthMap, fragToLight + sampleGrid[i] * sampleRadius).r;
        closestDepth *= far_plane;   
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }      
    shadow /= float(NR_SAMPLE);
    return shadow;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){

    vec3 lightDir = normalize(fragPos - light.position);   
    vec3 reflectDir = reflect(lightDir, normal);

	float distance = length(light.position - FragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoord).rgb;
	vec3 diffuse = light.diffuse * texture(texture_diffuse1, TexCoord).rgb * max(dot(normal, -lightDir), 0);
    //Phong model
	//vec3 specular = light.specular * texture(material.specular, TexCoord).rgb * pow(max(dot(reflectDir, viewDir), 0), material.shininess);
    //Blinn-phong model 
    vec3 specular = light.specular * texture(texture_specular1, TexCoord).rgb * pow(max(dot(normalize(-lightDir + viewDir), normal), 0), material.shininess);
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float isShadow = ShadowCalculation(fragPos);

    return (ambient + (1.0 - isShadow) * (diffuse + specular));
}

vec3 CalcPointLight_(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color){

    vec3 lightDir = normalize(fragPos - light.position);   
    vec3 reflectDir = reflect(lightDir, normal);

	float distance = length(light.position - FragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 ambient = light.ambient * color;
	vec3 diffuse = light.diffuse * color * max(dot(normal, -lightDir), 0);
    vec3 specular = light.specular * color * pow(max(dot(normalize(-lightDir + viewDir), normal), 0), material.shininess);
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float isShadow = ShadowCalculation(fragPos);

    return (ambient + (1.0 - isShadow) * (diffuse + specular));
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){

    vec3 lightDir = normalize(fragPos - light.position);   
    vec3 reflectDir = reflect(lightDir, normal);

	float distance = length(light.position - FragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);

    float theta = dot(lightDir, light.direction);
    float epsilon = light.cutoff - light.outcutoff;
    float intensity = clamp((theta - light.outcutoff) / epsilon, 0.0, 1.0);

	vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;
	vec3 diffuse = light.diffuse * texture(material.diffuse, TexCoord).rgb * max(dot(normal, -lightDir), 0);
	vec3 specular = light.specular * texture(material.specular, TexCoord).rgb * pow(max(dot(reflectDir, viewDir), 0), material.shininess);
		
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}