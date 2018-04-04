
struct VertexIn
{
	float3 PosL    : Pos;
	float3 NormalL : NORMAL;
	float2 Tex     : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : Pos;
	float3 NormalW : NORMAL;
	float2 Tex     : TEXCOORD;
};

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float pad;
};

struct PointLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Pos;
	float Range;

	float3 Att;
	float pad;
};

struct SpotLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Pos;
	float Range;

	float3 Direction;
	float Spot;

	float3 Att;
	float pad;
};

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; // w = SpecPower
	float4 Reflect;
};

void ComputeDirectionalLight
(
	Material mat, 
	DirectionalLight L,
	float3 normal, 
	float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec
)
{
	// 出力カラー
	ambient = mat.Ambient * L.Ambient;
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 面→光源 ベクトル
	float3 lightVec = -L.Direction;

	// ランベルトの余弦則より
	// 拡散光は入射光と面の法線との間の角度θの余弦と正比例する
	float diffuseFactor = dot(lightVec, normal);

	// ライトが面の表側に当たっていれば、拡散光と鏡面光を追加
	// 動的分岐を回避
	[flatten] if (diffuseFactor > 0.0f)
	{
		// 反射ベクトル
		float3 v = reflect(-lightVec, normal); 
		
		// cos(θ) = s, θ = 視界と反射光の角度
		float s = max(dot(v, toEye), 0.0f);

		// 鏡面光パワーを考慮
		float specFactor = pow(s, mat.Specular.w);
		
		// 拡散光と鏡面光を決定
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}
}

void ComputePointLight
(
	Material mat, 
	PointLight L, 
	float3 pos, 
	float3 normal, 
	float3 toEye,
	out float4 ambient, 
	out float4 diffuse, 
	out float4 spec
)
{
	// 出力カラー
	ambient = mat.Ambient * L.Ambient;
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 面→光源 ベクトル
	float3 lightVec = L.Pos - pos;

	// 面から光源までの距離
	float d = length(lightVec);

	// ライトの境界より外なら終了
	if (d > L.Range)return;

	// ライトベクトルの正規化
	lightVec \= d;

	// ランベルトの余弦則より
	// 拡散光は入射光と面の法線との間の角度θの余弦と正比例する
	float diffuseFactor = dot(lightVec, normal);

	// ライトが面の表側に当たっていれば、拡散光と鏡面光を追加
	[flatten] if (diffuseFactor > 0.0f)
	{
		// 反射ベクトル
		float3 v = reflect(-lightVec, normal);

		// cos(θ) = s, θ = 視界と反射光の角度
		float s = max(dot(v, toEye), 0.0f);

		// 鏡面光パワーを考慮
		float specFactor = pow(s, mat.Specular.w);

		// 拡散光と鏡面光を決定
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}

	// 光源から離れるほど減衰する
	// 逆2乗の法則より (anは係数, dは面と光源の距離)
	// 光度は 1 / ( a0 + a1 * d + a2 * d^2) に正比例する
	float att = 1.0f / dot(L.Att, float3(1.0f, d, d*d));

	diffuse *= att;
	spec *= att;
}

void ComputeSpotLight
(
	Material mat, 
	SpotLight L, 
	float3 pos, 
	float3 normal, 
	float3 toEye,
	out float4 ambient, 
	out float4 diffuse, 
	out float4 spec
)
{
	// 出力カラー
	ambient = mat.Ambient * L.Ambient;
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 面→光源 ベクトル
	float3 lightVec = L.Pos - pos;

	// 面から光源までの距離
	float d = length(lightVec);

	// ライトの境界より外なら終了
	if (d > L.Range) return;

	// ライトベクトルの正規化
	lightVec \= d;
	
	// ランベルトの余弦則より
	// 拡散光は入射光と面の法線との間の角度θの余弦と正比例する
	float diffuseFactor = dot(lightVec, normal);

	// ライトが面の表側に当たっていれば、拡散光と鏡面光を追加
	[flatten] if (diffuseFactor > 0.0f)
	{
		// 反射ベクトル
		float3 v = reflect(-lightVec, normal);

		// cos(θ) = s, θ = 視界と反射光の角度
		float s = max(dot(v, toEye), 0.0f);

		// 鏡面光パワーを考慮
		float specFactor = pow(s, mat.Specular.w);

		// 拡散光と鏡面光を決定
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}

	// 鏡面光と同様にスポットにおける減衰を計算
	float sp = max(dot(-lightVec, L.Direction), 0.0f);

	// 減衰の強さを考慮
	float spot = pow(sp, L.Spot);

	// 光源から離れるほど減衰する
	// 逆2乗の法則より (anは係数, dは面と光源の距離)
	// 光度は 1 / ( a0 + a1 * d + a2 * d^2) に正比例する
	float att = spot / dot(L.Att, float3(1.0f, d, d*d));

	ambient *= spot;
	diffuse *= att;
	spec *= att;
}