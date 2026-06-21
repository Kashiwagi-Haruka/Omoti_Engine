#include "Particle.hlsli"

StructuredBuffer<Particle> gParticles : register(t1);
ConstantBuffer<PerView> gPerView : register(b1);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;

    Particle particle = gParticles[instanceId];

    float3 billboardRight = gPerView.billboardMatrix[0].xyz;
    float3 billboardUp = gPerView.billboardMatrix[1].xyz;
    float3 billboardForward = gPerView.billboardMatrix[2].xyz;

    float3 worldPosition = particle.translate;
    worldPosition += billboardRight * input.position.x * particle.scale.x;
    worldPosition += billboardUp * input.position.y * particle.scale.y;
    worldPosition += billboardForward * input.position.z * particle.scale.z;

    output.position = mul(float4(worldPosition, 1.0f), gPerView.viewProjection);
    output.texcoord = input.texcoord;
    float t = (particle.lifeTime > 0.0f) ? saturate(particle.currentTime / particle.lifeTime) : 1.0f;
    output.color = lerp(particle.beforeColor, particle.afterColor, t);

    return output;
}