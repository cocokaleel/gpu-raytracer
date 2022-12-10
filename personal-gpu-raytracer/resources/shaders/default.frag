#version 330 core
//Adapted from lab 10

// Declare "in" variables for the world-space position and normal,
//         received post-interpolation from the vertex shader

// UV coordinate in variable representing a pixel in the full-screen quad
in vec2 uvCoords;
out vec4 fragColor;// output color, which goes into the openGL clipping situation

uniform vec4 cameraPositionWorld;


// Declare relevant uniform(s) here, for ambient lighting
uniform float ka;

// Declare relevant uniform here, for diffuse lighting
uniform float kd;

// holds the number of lights that exist in the scene (for looping purposes)
uniform int numLights;

//create a struct for holding light information
struct LightColorPos {
    vec4 color;
    vec4 dir; //only for directional light
    vec4 pos; //only for point light and spot light
    int lightType; //0 is directional, 1 is point, 2 is for spot
    vec3 attenuation;
    float angle; //only for spot lights
    float penumbra; //only for spot lights
};

//create a uniform input for the light data
uniform LightColorPos lights[8];

// Declare relevant uniform(s) here, for specular lighting

uniform float ks;
uniform float shininess;

// INSPO FOR SHAPE STRUCT
struct ShapeData {
    mat4 ctm;
    mat3 ctmInv;
    int type; //0 is sphere
    vec4 cAmbient;
    vec4 cDiffuse;
    vec4 cSpecular;
    float shininess;
    vec4 cReflective;
};

uniform ShapeData shapes[1]; //constrains it to only one shape being passed in
uniform int numShapes;

//Color contributions from shapes
uniform vec4 cSpecular;
uniform vec4 cDiffuse;
uniform vec4 cAmbient;

void main() {
    // Remember that you need to renormalize vectors here if you want them to be normalized
//    vec3 normal = normalize(normalWorld);

    //CHECK IF NORMALS ARE CORRECT BY COMMENTING THIS IN:
//    fragColor = vec4(abs(normal), 1.f);

    fragColor = vec4(uvCoords[1], 0.f, 0.f, 1.f);

    for (int i = 0; i < numShapes; i++) {
        if (shapes[i].type == 0) {
            fragColor = vec4(1.f);
        }
    }

//    // ADD AMBIENT COMPONENT
//    fragColor += ka * cAmbient;

//    for (int i = 0; i < numLights; i++) {
//        if (lights[i].lightType == 0) {
//            //DIRECTIONAL LIGHTS
//            // Add diffuse component to output color
//           float Id = kd * dot(normalize(normal), vec3(normalize(-lights[i].dir))); //only for directional lights

//           //ADD FINAL DIFFUSE COMPONENT
//           fragColor += vec4(vec3(max(Id,0)), 0.f) * cDiffuse * lights[i].color;

//           // Add specular component to output color
//           vec4 pos4 = vec4(positionWorld, 1.f);

//           vec4 reflected = reflect(-lights[i].dir, vec4(normal, 0.f));
//           vec4 directionToCamera = cameraPositionWorld - pos4;

//           float dotRE = dot(normalize(reflected), normalize(directionToCamera));
//           float spec = dotRE < 0 ? ks*pow(dotRE, shininess) : 0;

//           //ADD FINAL SPECULAR COMPONENT
//           fragColor += vec4(vec3(spec), 0.f)*lights[i].color * cSpecular;

//        } else if (lights[i].lightType == 1) {
//           //POINT LIGHTS
//           vec3 lightDirection = positionWorld - vec3(lights[i].pos);
//           float lightDistance = distance(positionWorld, vec3(lights[i].pos));
//           float lightComponent = dot(normal, -normalize(lightDirection));

//           //compute attenuation function
//           float attenuationFactor = 1.f/(lights[i].attenuation[0] + lights[i].attenuation[1]*lightDistance + lights[i].attenuation[2]*lightDistance*lightDistance);

//           //compute and add diffuse component
//           float diffuseComponent = attenuationFactor*lightComponent*kd;
//           fragColor += vec4(vec3(max(diffuseComponent,0)), 0.f) * cDiffuse * lights[i].color;

//           //compute and add specular component
//           vec3 reflectedLight = reflect(-normalize(lightDirection), normal);
//           vec3 directionToCamera = normalize(vec3(cameraPositionWorld)-positionWorld);
//           float specularComponent = attenuationFactor * ks * pow(dot(reflectedLight, directionToCamera), shininess);

//           //if the light and the normal are in the same direction
//           fragColor += dot(-lightDirection, normal) > 0 ?vec4(vec3(max(specularComponent,0)), 0.f) * cSpecular * lights[i].color : vec4(0);

//        } else {
//            //SPOT LIGHTS

//            //calculate the falloff of the light intensity
//            vec4 lightColor;
//            vec3 lightDirection = positionWorld - vec3(lights[i].pos);
//            float angleOfLight = acos(dot(normalize(lightDirection), vec3(normalize(lights[i].dir))));
//            float outerTheta = lights[i].angle;
//            float innerTheta = lights[i].angle-lights[i].penumbra;
//            if (angleOfLight <= innerTheta) {
//                lightColor = lights[i].color;
//            } else if (angleOfLight <=outerTheta) {
//                float division = (angleOfLight-innerTheta)/(outerTheta-innerTheta);
//                float falloff = -2.f*pow(division, 3.f) + 3.f*pow(division, 2.f);
//                lightColor = lights[i].color*(1.f-falloff);
//            } else {
//                lightColor = vec4(0,0,0,1);
//            }



//            float lightDistance = distance(positionWorld, vec3(lights[i].pos));
//            float lightComponent = dot(normal, -normalize(lightDirection));

//            //compute attenuation function
//            float attenuationFactor = 1.f/(lights[i].attenuation[0] + lights[i].attenuation[1]*lightDistance + lights[i].attenuation[2]*lightDistance*lightDistance);

//            //compute and add diffuse component
//            float diffuseComponent = attenuationFactor*lightComponent*kd;
//            fragColor += vec4(vec3(max(diffuseComponent,0)), 0.f) * cDiffuse * lightColor;

//            //compute and add specular component
//            vec3 reflectedLight = reflect(-normalize(lightDirection), normal);
//            vec3 directionToCamera = normalize(vec3(cameraPositionWorld)-positionWorld);
//            float specularComponent = attenuationFactor * ks * pow(dot(reflectedLight, directionToCamera), shininess);

//            //if the light and the normal are in the same direction
//            fragColor += dot(-lightDirection, normal) > 0 ?vec4(vec3(max(specularComponent,0)), 0.f) * cSpecular * lightColor : vec4(0);

//        }
//    }

//    fragColor[3] = 1.f;
}
