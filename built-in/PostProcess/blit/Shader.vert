#version 460 core

out V2F
{
	vec2 texCoord;
} v2f;

void main()
{
    // 根据顶点ID (0, 1, 2) 计算纹理坐标 (0,0), (2,0), (0,2)
    v2f.texCoord = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);

    // 将 UV [0, 2] 映射到 NDC [-1, 3]
    gl_Position = vec4( v2f.texCoord * 2.0 - vec2(1.0), 0.0, 1.0);
}