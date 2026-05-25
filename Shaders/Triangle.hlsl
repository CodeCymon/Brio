float4 VSMain(uint vertexID : SV_VertexID) : SV_Position {
    float2 positions[3] = {
        float2(0.0, 0.5),
        float2(0.5, -0.5),
        float2(-0.5, -0.5)
    };

    return float4(positions[vertexID], 0.0, 1.0);
}

float4 PSMain() : SV_Target {
    return float4(1.0, 0.5, 0.0, 1.0);
}