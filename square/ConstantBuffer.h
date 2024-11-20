#pragma once
#include <directxtk/SimpleMath.h>
using namespace DirectX::SimpleMath;

struct ConstantBuffer
{
	Matrix mWorld;
	Matrix mView;
	Matrix mProjection;
	Vector4 lightColor;
	Vector4 AmbientLightColor;
	Vector4 MatSpecularColor;
	Vector3 lightDirection;
	float MatSpecularPower;
	Vector3 CamPosition;
	float MatAmbientPower;
	float LightPower;
	float LightSpecularPower;
	bool isDiffuse;
	bool isNormal;
	bool isSpecular;
	bool isEmissive;
	bool isOpacity;
	bool padding[3];
};
