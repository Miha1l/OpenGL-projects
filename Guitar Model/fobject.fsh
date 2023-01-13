#version 330 core

in vec3 Color;
in vec3 FragPos;
in vec3 Normal;
in vec4 LightDirection;
in vec4 positionLightMatrix;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

struct Light {
    int type;

    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 direction;
    float cutOff;

    float constant;
    float linear;
    float quadratic;
};

uniform Light light;

uniform sampler2D ShadowMap;
uniform float ShadowMapSize;
uniform float ShadowPointCloudFilteringQuality;

uniform vec3 ViewPos;

out vec4 color;

float SampleShadowMap(sampler2D map, vec2 coords, float compare)
{
    vec4 v = texture2D(map, coords);
    float value = v.x * 255.0f + (v.y * 255.0f + (v.z * 255.0f + v.w) / 255.0f) / 255.0f;
    return step(compare, value);
}

float SampleShadowMapLinear(sampler2D map, vec2 coords, float compare, vec2 texelSize)
{
    vec2 pixSize = coords / texelSize + 0.5f;
    vec2 pixFractPart = fract(pixSize);
    vec2 startTexel = (pixSize - pixFractPart) * texelSize;

    float bltexel = SampleShadowMap(map, startTexel, compare);
    float brtexel = SampleShadowMap(map, startTexel + vec2(texelSize.x, 0.0f), compare);
    float tltexel = SampleShadowMap(map, startTexel + vec2(0.0f, texelSize.y), compare);
    float trtexel = SampleShadowMap(map, startTexel + texelSize, compare);

    float mixL = mix(bltexel, tltexel, pixFractPart.y);
    float mixR = mix(brtexel, trtexel, pixFractPart.y);

    return mix(mixL, mixR, pixFractPart.x);
}

float SampleShadowMapPCF(sampler2D map, vec2 coords, float compare, vec2 texelSize)
{
    float result = 0.0f;
    float spcfq = ShadowPointCloudFilteringQuality;
    for (float y = -spcfq; y < spcfq; y += 1.0f) {
        for (float x = -spcfq; x < spcfq; x += 1.0f) {
            vec2 offset = vec2(x, y) * texelSize;
            result += SampleShadowMapLinear(map, coords + offset, compare, texelSize);
        }
    }
    return result / 9.0f;
}

float CalcShadowAmount(sampler2D map, vec4 plm)
{
    vec3 value = (plm.xyz / plm.w) * vec3(0.5f) + vec3(0.5f);
    float offset = 2.0f * dot(Normal, LightDirection.xyz); // первый коэффициент должен влиять на качество тени
    return SampleShadowMapPCF(map, value.xy, value.z * 255.0f + offset, vec2(1.0f / ShadowMapSize));
}

void main()
{
    vec3 ambient = light.ambient * material.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDir;
    if (light.type == 2) {
        lightDir = -light.direction;
    }
    else {
        lightDir = normalize(light.position - FragPos);
    }
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    if (light.type != 2) {
        float distance = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;
    }

    float shadowCoef = 1.0f;
    shadowCoef = CalcShadowAmount(ShadowMap, positionLightMatrix);
    shadowCoef += 0.15f; // избавляемся от абсолютной черноты тени
    if (shadowCoef > 1.0f) {
        shadowCoef = 1.0f;
    }

    float theta = dot(lightDir, normalize(-light.direction));

    if (light.type == 0 || theta > light.cutOff) {
        vec3 result = (ambient + diffuse + specular) * Color;
        color = vec4(result, 1.0f) * shadowCoef;
    }
    else {
        vec3 result = ambient * Color;
        color = vec4(result, 1.0f) * shadowCoef;
    }
}
