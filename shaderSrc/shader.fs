#version 330 core
out vec4 FragColor;

#define MAX_POINT_LIGHTS 64

struct Material {
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
    sampler2D alphaMap;

    vec3 diffuseColor;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 color;
};

struct PointLight{
    vec3 ambient;
    vec3 color;
    vec3 position;
    float constantVar;
    float linearVar;
    float quadraticVar;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

const vec3 WireFrameColor = vec3(0.f,0.f,0.f);
uniform bool ShouldDisableTexture;

uniform Material material;
uniform DirectionalLight DirLight; 
uniform int numPointLights;
uniform PointLight PLights[MAX_POINT_LIGHTS]; 

uniform vec3 viewPos;

vec3 CalculateDirectionalLightContribution(){

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(DirLight.direction, Normal);

    float NormalDirectionalLight = max(dot(Normal, -DirLight.direction), 0);
    float ViewDirectionReflected = pow(max(dot(viewDir, reflectDir), 0), material.shininess);

    vec3 materialDiffuseColor = vec3(texture(material.diffuseMap, TexCoord)) * material.diffuseColor;
    vec3 materialSpecularColor = vec3(texture(material.specularMap, TexCoord));

    vec3 ambientContribution = DirLight.ambient * materialDiffuseColor;
    vec3 diffuseContribution = DirLight.color * materialDiffuseColor * NormalDirectionalLight;
    vec3 specularContribution = DirLight.color * materialSpecularColor * ViewDirectionReflected;

   return ambientContribution + diffuseContribution + specularContribution;
}

vec3 CalculatePointLightContribution(){
    
    vec3 materialDiffuseColor = vec3(texture(material.diffuseMap, TexCoord)) * material.diffuseColor;
    vec3 materialSpecularColor = vec3(texture(material.specularMap, TexCoord));

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 finalColor = vec3(0);

    for(int i = 0; i < numPointLights; i++){
      
      vec3 PointLightDirection =  normalize(PLights[i].position - FragPos);
       vec3 reflectDir = reflect(-PointLightDirection, Normal);

       float NormalDirectionalLight = max(dot(Normal, PointLightDirection), 0);
       float ViewDirectionReflected = pow(max(dot(viewDir, reflectDir), 0), material.shininess);

       vec3 ambientContribution = PLights[i].ambient * materialDiffuseColor;
       vec3 diffuseContribution = PLights[i].color * materialDiffuseColor * NormalDirectionalLight;
       vec3 specularContribution = PLights[i].color * materialSpecularColor * ViewDirectionReflected;
    
       float distance    = length(PLights[i].position - FragPos);
       float attenuation = 1.0 / (PLights[i].constantVar + PLights[i].linearVar * distance + PLights[i].quadraticVar * (distance * distance)); 
        
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
        vec3 directionalLightContribution = CalculateDirectionalLightContribution();
        vec3 pointLightsContribution = CalculatePointLightContribution();
        vec3 finalColor = pointLightsContribution + directionalLightContribution;
        FragColor = vec4(finalColor, 1.0);
   }
}