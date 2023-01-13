#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main()
{
    vec4 objectPos = vec4(position, 1.0);
    mat4 modelViewMatrix = viewMatrix * modelMatrix;
    gl_Position = projectionMatrix * modelViewMatrix * objectPos;
}
