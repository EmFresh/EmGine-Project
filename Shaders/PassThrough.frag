#version 420

//enums
const int DEFAULT=0;
const int POINT=1;
const int DIRECTIONAL=2;
const int SPOTLIGHT=3;


uniform vec4 LightPosition;

//colour
uniform vec3 LightAmbient;
uniform vec3 LightDiffuse;
uniform vec3 LightSpecular;

//scalar
uniform float LightSpecularExponent;
uniform float Attenuation_Constant;
uniform float Attenuation_Linear;
uniform float Attenuation_Quadratic;

//amount of lights
uniform int LightAmount;

//other
uniform bool LightEnable;
uniform int LightType;
uniform vec3 LightDirection;
uniform float LightAngleConstraint;


//uniform sampler2D uTex;
uniform sampler2D uPos;
uniform sampler2D uNorm;
uniform sampler2D uScene;
uniform sampler2D uRamp;
uniform vec3 uViewPos = vec3(0, 0, 0);

uniform bool toonActive;

in vec2 texcoord;

out vec4 outColor;

void pointLight();

void defaultLight()
{
    pointLight();
}

void spotLight()
{
   
}

void pointLight()
{
    vec3 normal = normalize(texture(uNorm, texcoord).xyz * 2 - 1);
    vec3 lightVec = normalize(LightPosition.xyz - (texture(uPos, texcoord).xyz * 2 - 1));
    float diff = max(dot(normal, lightVec), 0.0);
   // float dist = length(lightVec);
   
   
    //The light contributes to this surface
    //Calculate attenuation (falloff)
    float attenuation = 1.0 / (Attenuation_Constant + (Attenuation_Linear * diff) + (Attenuation_Quadratic * diff * diff));
      
    //Calculate diffuse contribution
    outColor.rgb += LightDiffuse * diff * attenuation ;
    
    //Blinn-Phong half vector
    float NdotHV =  max(dot(uViewPos - (texture(uPos,texcoord).xyz* 2 - 1), reflect(-lightVec, normal)), 0.0); 
    
    //Calculate specular contribution
    outColor.rgb += LightSpecular * pow(NdotHV, LightSpecularExponent) * attenuation;
}

void directionalLight()
{
    vec3 colour = texture(uScene, texcoord).rgb; 

    vec3 normal = normalize(texture(uNorm,texcoord).rgb* 2 - 1);
    vec3 lightVec = LightPosition.xyz - (texture(uPos,texcoord).xyz * 2 - 1);
    float dist = length(lightVec);
	vec3 direc = LightDirection;
	vec3 reflection = reflect(-direc,normal);
	vec3 eye = normalize(- texture(uPos,texcoord).xyz);
	float viewToRe = max(dot(eye,reflection),0.0);

    float NdotL = max(dot(normal, lightVec),0.0);

   
    //The light contributes to this surface
    //Calculate attenuation (falloff)
    float attenuation = 1.0 / (Attenuation_Constant + (Attenuation_Linear * dist) + (Attenuation_Quadratic * dist * dist));
      
    //Calculate diffuse contribution
	outColor.rgb +=  max(NdotL, 0.0) *  LightDiffuse * attenuation * colour;
	//NdotL = NdotL * 0.5 + 0.5;
    //outColor.rgb += LightDiffuse * attenuation * texture(uRamp, vec2(NdotL, 0.5)).rgb;
    
    //Blinn-Phong half vector
    float NdotHV =  max(dot(normal, normalize(LightDirection + lightVec)), 0.0); 
        
    //Calculate specular contribution
	float spec = pow(max(viewToRe, 0.0), 16);
	outColor.rgb +=  LightSpecular * spec * attenuation * colour * LightSpecularExponent;
    //outColor.rgb += LightSpecular * texture(uRamp, vec2(viewToRe, 0.5)).rgb * attenuation;
}

//void pointLight()
//{
//	vec3 colour = texture(uScene, texcoord).rgb; 
//
//    vec3 normal = normalize(texture(uNorm,texcoord).rgb);
//    vec3 lightVec = LightPosition.xyz - texture(uPos,texcoord).xyz;
//    float dist = length(lightVec);
//	vec3 direc = lightVec / dist;
//	vec3 reflection = reflect(-direc,normal);
//	vec3 eye = normalize(- texture(uPos,texcoord).xyz);
//	float viewToRe =  max(dot(eye,reflection),0.0);
//
//    float NdotL = max(dot(normal, lightVec),0.0);
//
//   
//    //The light contributes to this surface
//    //Calculate attenuation (falloff)
//    float attenuation = 1.0 / (Attenuation_Constant + (Attenuation_Linear * dist) + (Attenuation_Quadratic * dist * dist));
//      
//    //Calculate diffuse contribution
//	outColor.rgb +=  max(NdotL, 0.0) *  LightDiffuse * attenuation * colour;
//	//NdotL = NdotL * 0.5 + 0.5;
//    //outColor.rgb += LightDiffuse * attenuation * texture(uRamp, vec2(NdotL, 0.5)).rgb;
//    
//    //Blinn-Phong half vector
//    float NdotHV =  max(dot(normal, normalize(LightDirection + lightVec)), 0.0); 
//        
//    //Calculate specular contribution
//	float spec = pow(max(viewToRe, 0.0), 16);
//	
//    if(toonActive)
//	{
//		outColor.rgb += LightSpecular * spec * attenuation * texture(uRamp, vec2(viewToRe, 0.5)).rgb ;
//	}
//	else
//	{
//		outColor.rgb += LightSpecular * spec * colour * attenuation ;
//	}
//}


//float ShadowCalculation(vec4 fragPosLightSpace)
//{
//    // perform perspective divide
//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//    // transform to [0,1] range
//    projCoords = projCoords * 0.5 + 0.5;
//    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
//    float closestDepth = texture(shadowMap, projCoords.xy).r; 
//    // get depth of current fragment from light's perspective
//    float currentDepth = projCoords.z;
//    // check whether current frag pos is in shadow
//    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
//
//    return shadow;
//}  

void main()
{

   vec3 colour = texture(uScene, texcoord).rgb; 
   
   //Ambient Light
   float ambientStrength = 1.0;
   outColor.rgb = colour * LightAmbient * ambientStrength;
    
    if(LightEnable)
        switch(LightType)
        {        
        case POINT:
            pointLight();
            break;
        case DIRECTIONAL:
            directionalLight();
            break;
        case SPOTLIGHT:
            spotLight();
            break;
        default:
            defaultLight();
        }
        
    outColor.a = 1;
   // outColor.rgb = abs(texture(uNorm,texcoord).rgb);
}