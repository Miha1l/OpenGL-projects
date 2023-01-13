#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 projectionLightMatrix;
uniform mat4 shadowLightMatrix;
uniform mat4 modelMatrix;

out vec4 Position;

void main()
{
    Position = projectionLightMatrix * shadowLightMatrix * modelMatrix * vec4(position, 1.0);
    gl_Position = Position;
}
