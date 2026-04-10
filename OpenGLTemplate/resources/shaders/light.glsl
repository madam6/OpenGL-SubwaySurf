#include_part

#define LIGHTS_MAX 8

struct LightInfo
{
    vec4 position;
    vec3 La;
    vec3 Ld;
    vec3 Ls;
};

struct MaterialInfo
{
    vec3 Ma;
    vec3 Md;
    vec3 Ms;
    float shininess;
};

uniform LightInfo lights[LIGHTS_MAX];
uniform int numLights;
uniform MaterialInfo material1; 

vec3 PhongModel(vec4 eyePosition, vec3 eyeNorm)
{
    vec3 v = normalize(-eyePosition.xyz);
    vec3 n = normalize(eyeNorm);

    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    float eps = 0.000001;

    for (int i = 0; i < numLights; i++)
    {
        vec3 s = normalize(vec3(lights[i].position - eyePosition));
        vec3 r = reflect(-s, n);

        float sDotN = max(dot(s, n), 0.0);

        ambient += lights[i].La * material1.Ma;
        diffuse += lights[i].Ld * material1.Md * sDotN;

        if (sDotN > 0.0)
        {
            specular += lights[i].Ls * material1.Ms * pow(max(dot(r, v), 0.0), material1.shininess + eps);
        }
    }
    return ambient + diffuse + specular;
}