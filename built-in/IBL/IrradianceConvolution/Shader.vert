#version 460 core

out V2F
{
    vec2 texCoord;
} v2f;

void main()
{
    v2f.texCoord = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    gl_Position = vec4(v2f.texCoord * 2.0 - vec2(1.0), 0.0, 1.0);
}
