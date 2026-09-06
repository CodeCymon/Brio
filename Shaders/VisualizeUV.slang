
#include "FullscreenTriangle.slang"

[shader("pixel")]
float4 PSMain(VSOut input) {
    return float4(input.uv, 0, 1);
}
