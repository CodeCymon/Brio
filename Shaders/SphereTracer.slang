
#include "FullscreenTriangle.slang"


struct PushConstants {
    float aspectRatio;
};

[push_constant]
PushConstants pc;

struct Ray {
    float3 origin;
    float3 direction;
};


float intersectSphere(Ray ray, float3 center, float radius) {
    float3 oc = ray.origin - center;

    float b = dot(oc, ray.direction);
    float c = dot(oc, oc) - radius * radius;

    float h = b * b - c;

    if (h < 0.0)
        return -1.0;

    h = sqrt(h);

    float t = -b - h;

    if (t < 0.0)
        t = -b + h;

    return t;
}


[shader("pixel")]
float4 PSMain(VSOut input) : SV_Target {
    float3 cameraPosition = float3(0.0, -3.0, 0.0);

    float3 cameraForward = float3(0.0, 1.0,  0.0);
    float3 cameraRight = float3(1.0, 0.0,  0.0);
    float3 cameraUp = float3(0.0, 0.0,  1.0);

    float fov = radians(60.0);

    float2 screen = input.uv * 2.0 - 1.0;
    screen.y = -screen.y;

    float tanHalfFov = tan(fov * 0.5);

    float3 rayDirection = normalize(
        cameraForward +
        cameraRight * screen.x * tanHalfFov * pc.aspectRatio +
        cameraUp * screen.y * tanHalfFov
    );

    Ray ray;
    ray.origin = cameraPosition;
    ray.direction = rayDirection;

    float3 sphereCenter = float3(0.0, 0.0, 0.0);
    float sphereRadius = 1.0;

    float t = intersectSphere(ray, sphereCenter, sphereRadius);

    if (t < 0.0) {
        return float4(0.02, 0.025, 0.04, 1.0);
    }

    float3 position = ray.origin + ray.direction * t;

    float3 normal = normalize(
        position - sphereCenter
    );

    float3 lightDirection = normalize(float3(-0.6, -0.5, 0.8));

    float diffuse = max(dot(normal, lightDirection), 0.0);

    float3 sphereColor = float3(0.8, 0.2, 0.05);

    float3 color = sphereColor * (0.05 + diffuse);

    return float4(color, 1.0);
}
