#version 330 core
out vec4 FragColor;

#define MAX_POINT_LIGHTS 64

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct Material {
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
    sampler2D alphaMap;

    vec3 diffuseColor;
    float shininess;
};

struct DirectionalLight {
    vec3 ambient;
    vec3 color;
    vec3 direction;
};

struct PointLight{
    vec3 ambient;
    vec3 color;
    vec3 position;
    float constant;
    float linear;
    float quadratic;
};

const vec3 WireFrameColor = vec3(0.f,0.f,0.f);

// Uniforms
uniform bool ShouldDisableTexture;

uniform Material material;
uniform DirectionalLight dl; 

uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS]; 

uniform vec3 viewPos;

vec3 CalculateDirectionalLightContribution(){

    vec3 lightDir = normalize(dl.direction);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, Normal);

    float NdL = max(dot(Normal, -lightDir), 0);
    float VdR = pow(max(dot(viewDir, reflectDir), 0), material.shininess);
    
    vec3 materialDiffuseColor = vec3(texture(material.diffuseMap, TexCoord)) * material.diffuseColor;
    vec3 materialSpecularColor = vec3(texture(material.specularMap, TexCoord));

    vec3 ambientContribution = dl.color * dl.ambient * materialDiffuseColor;
    vec3 diffuseContribution = dl.color * materialDiffuseColor * NdL;
    vec3 specularContribution = dl.color * materialSpecularColor * VdR;

    return ambientContribution + diffuseContribution + specularContribution;
}

vec3 CalculatePointLightContribution(){
    
    vec3 materialDiffuseColor = vec3(texture(material.diffuseMap, TexCoord)) * material.diffuseColor;
    vec3 materialSpecularColor = vec3(texture(material.specularMap, TexCoord));

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 finalColor = vec3(0);
    for (int i = 0; i < numPointLights; i++)
    {
        vec3 plDir = normalize(pointLights[i].position - FragPos);

        vec3 reflectDir = reflect(-plDir, Normal);
        float NdL = max(dot(Normal, plDir), 0);
        float VdR = pow(max(dot(viewDir, reflectDir), 0), material.shininess);

        vec3 ambientContribution = pointLights[i].color * pointLights[i].ambient * materialDiffuseColor;
        vec3 diffuseContribution = pointLights[i].color * materialDiffuseColor * NdL;
        vec3 specularContribution = pointLights[i].color * materialSpecularColor * VdR;

        float distance    = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * (distance * distance)); 

        finalColor += (ambientContribution + diffuseContribution + specularContribution) * attenuation;
    }

    return finalColor;
}


void main()
{
    if(ShouldDisableTexture){
        FragColor = vec4(WireFrameColor, 1.0);
   }
   else {
         if (texture(material.diffuseMap, TexCoord).a < 0.5)
        discard;

    vec3 directionalLightContribution = CalculateDirectionalLightContribution();
    vec3 pointLightsContribution = CalculatePointLightContribution();
    vec3 finalColor = pointLightsContribution + directionalLightContribution;
    FragColor = vec4(finalColor, 1);    
    //FragColor = vec4(0,0,1,1); // blue color
   }
}