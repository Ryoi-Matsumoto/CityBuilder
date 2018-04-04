#include "PhongCommon.hlsli"

cbuffer cbPerFrame
{
	DirectionalLight gDirLight;
	float3 gEyePosW;
};

cbuffer cbPerObject
{
	Material gMaterial;
	bool gUseTexture;
};

Texture2D gDiffuseMap;

SamplerState samTriLiNear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;
};

float4 main(VertexOut pin) : SV_Target
{
	// 補間法では正規化できる
	pin.NormalW = normalize(pin.NormalW);

	// 視点までのベクトル
	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	float4 texColor = float4(1, 1, 1, 1);
	if (gUseTexture)
	{
		texColor = gDiffuseMap.Sample(samTriLiNear, pin.Tex);
	}

	float4 litColor = texColor;

	// ゼロに初期化
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 各光からの影響を計算
	float4 A, D, S;

	ComputeDirectionalLight(gMaterial, gDirLight, pin.NormalW, toEyeW, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;		

	litColor = texColor * (ambient + diffuse) + spec;

	// 透過度を決定
	litColor.a = gMaterial.Diffuse.a * texColor.a;

	return litColor;
}