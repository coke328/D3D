#include "shared.fxh"

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.Pos = mul(input.Pos, World);
    output.WorldPos = output.Pos.xyz;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Norm = normalize(mul(input.Norm, (float3x3) World));
    output.Tangent = normalize(mul(input.TAN, (float3x3) World));
    output.Tex = input.Tex;
    return output;
}