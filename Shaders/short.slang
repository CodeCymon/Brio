RWStructuredBuffer<float> result;

[shader("compute")]
[numthreads(1,1,1)]
void CSMain(uint3 threadID : SV_DispatchThreadID) {
    result[threadID.x] = threadID.x;
}