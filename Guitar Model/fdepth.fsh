#version 330 core

in vec4 Position;

out vec4 color;

void main()
{
    float depth = Position.z / Position.w;
    depth = depth * 0.5f + 0.5f;

    float v1 = depth * 255.0;
    float f1 = fract(v1); // Дробная часть v1
    float vn1 = floor(v1) / 255.0;

    float v2 = f1 * 255.0;
    float f2 = fract(v2);
    float vn2 = floor(v2) / 255.0;

    float v3 = f2 * 255.0;
    float f3 = fract(v3);
    float vn3 = floor(v3) / 255.0;

    color = vec4(vn1, vn2, vn3, f3);
}
