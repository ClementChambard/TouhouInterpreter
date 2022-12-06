
void Skinning(inout float3 P, inout float3 N, float4 weights, uint4 indices)
{
    if (any(weights))
    {
        float3x4 m = 0;

        for (uint i = 0; i < 4; i++) {
            m += bones[indices[i]] * weights[i];
        }

        P = mul(m, float4(P, 1));
        N = normalize(mul((float3x3)m, N));
    }
}
