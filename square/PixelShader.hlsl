#include "shared.fxh"

float4 main(PS_INPUT input) : SV_TARGET
{
    
    
    float4 Diffuse = lightColor * DiffuseTex.Sample(samLinear, input.Tex);
    
    float3 vNormal = input.Norm;
    
    if (isNormal)
    {
        float3 vNormal = NormalTex.Sample(samLinear, input.Tex).rgb * 2.0f - 1.0f;
        float3x3 WorldMatrix = float3x3(input.Tangent, cross(input.Norm, input.Tangent), input.Norm);
        vNormal = normalize(mul(vNormal, WorldMatrix));
    }
        
    
    float4 Ambient = AmbientLightColor * Diffuse;
    
    Diffuse = saturate(dot(-lightDirection, vNormal) * Diffuse * LightPower);
    
    float3 View = normalize(CamPosition - input.WorldPos);
    float3 HalfVec = normalize(-lightDirection + View);
    float1 DotV = max(0.0f, dot(HalfVec, vNormal));
    float4 Specular = pow(DotV, MatSpecularPower) * MatSpecularColor * lightColor;
    
    if (isSpecular)
    {
        Specular = saturate(Specular * SpecularTex.Sample(samLinear, input.Tex) * LightSpecularPower);
    }
    else
    {
        Specular = saturate(Specular * LightSpecularPower);
    }
   
    
    return saturate(Diffuse * (1-MatAmbientPower) + Ambient * MatAmbientPower + Specular);
}