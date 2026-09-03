static const float3 pos[3] = {
    float3( 0, -1, 0),
    float3(-1, 1, 0),
    float3( 1, 1, 0)
};

static const float3 col[3] = {
    float3(1,0,0),
    float3(0,1,0),
    float3(0,0,1)
};

struct VertexOut {
    float4 position : SV_Position;
    float4 color : TEXCOORD0;
};

[shader("vertex")]
VertexOut VSMain(uint vertexID : SV_VulkanVertexID) {
    VertexOut out;
    out.position = float4(pos[vertexID], 1.0);
    out.color = float4(col[vertexID], 1.0);

    return out;
}

[shader("fragment")]
float4 PSMain(VertexOut input) : SV_Target {
    return input.color;
}
