#version 330 core

layout(location = 0) in vec3 objectPosition;
layout(location = 1) in vec3 objectColor;
layout(location = 2) in vec3 normal;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform mat4 projectionLightMatrix;
uniform mat4 shadowLightMatrix;
uniform mat4 lightMatrix;
uniform vec4 lightDirection;

out vec3 Color;
out vec3 FragPos;
out vec3 Normal;
out vec4 LightDirection;
out vec4 positionLightMatrix;

void main()
{
    vec4 objectPos = vec4(objectPosition, 1.0);
    mat4 modelViewMatrix = viewMatrix * modelMatrix;
    gl_Position = projectionMatrix * modelViewMatrix * objectPos;
    Color = objectColor;
    FragPos = vec3(modelMatrix * objectPos);
    Normal = mat3(transpose(inverse(modelMatrix))) * normal;
    positionLightMatrix = projectionLightMatrix * shadowLightMatrix * modelMatrix * objectPos;
    LightDirection = viewMatrix * lightMatrix * lightDirection;
}
