
Texture2D DiffuseTex : register(t0);
Texture2D NormalTex : register(t1);
Texture2D SpecularTex : register(t2);
Texture2D EmissiveTex : register(t3);
Texture2D OpacityTex : register(t4);

SamplerState samLinear : register(s0);

cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    float4 lightColor;
    float4 AmbientLightColor;
    float4 MatSpecularColor;
    float3 lightDirection;
    float1 MatSpecularPower;
    float3 CamPosition;
    float1 MatAmbientPower;
    float1 LightPower;
    float1 LightSpecularPower;
    bool isDiffuse;
    bool isNormal;
    bool isSpecular;
    bool isEmissive;
    bool isOpacity;
    float3 Padding;
}

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float3 TAN : TANGENT;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 WorldPos : POSITION;
    float3 Norm : NORMAL;
    float3 Tangent : TANGENT;
    float2 Tex : TEXCOORD0;
};