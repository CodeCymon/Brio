
struct PushConstants {
    uint64_t bufferAddress;
};

[push_constant]
PushConstants pc;

struct Vertex {
    float3 position;
    float padding;
    float3 color;
    float padding2;
};

struct VertexOut {
    float4 position : SV_Position;
    float4 color : TEXCOORD0;
};

[shader("vertex")]
VertexOut VSMain(uint vertexID : SV_VulkanVertexID) {
    Vertex* vertices = (Vertex*)pc.bufferAddress;
    Vertex v = vertices[vertexID];

    VertexOut out;
    out.position = float4(v.position, 1.0);
    out.color = float4(v.color, 1.0);

    return out;
}

[shader("fragment")]
float4 PSMain(VertexOut input) : SV_Target {
    return input.color;
}
