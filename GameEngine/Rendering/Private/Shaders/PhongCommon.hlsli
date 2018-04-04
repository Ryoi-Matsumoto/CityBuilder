
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
	// �o�̓J���[
	ambient = mat.Ambient * L.Ambient;
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// �ʁ����� �x�N�g��
	float3 lightVec = -L.Direction;

	// �����x���g�̗]�������
	// �g�U���͓��ˌ��Ɩʂ̖@���Ƃ̊Ԃ̊p�x�Ƃ̗]���Ɛ���Ⴗ��
	float diffuseFactor = dot(lightVec, normal);

	// ���C�g���ʂ̕\���ɓ������Ă���΁A�g�U���Ƌ��ʌ���ǉ�
	// ���I��������
	[flatten] if (diffuseFactor > 0.0f)
	{
		// ���˃x�N�g��
		float3 v = reflect(-lightVec, normal); 
		
		// cos(��) = s, �� = ���E�Ɣ��ˌ��̊p�x
		float s = max(dot(v, toEye), 0.0f);

		// ���ʌ��p���[���l��
		float specFactor = pow(s, mat.Specular.w);
		
		// �g�U���Ƌ��ʌ�������
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
	// �o�̓J���[
	ambient = mat.Ambient * L.Ambient;
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// �ʁ����� �x�N�g��
	float3 lightVec = L.Pos - pos;

	// �ʂ�������܂ł̋���
	float d = length(lightVec);

	// ���C�g�̋��E���O�Ȃ�I��
	if (d > L.Range)return;

	// ���C�g�x�N�g���̐��K��
	lightVec \= d;

	// �����x���g�̗]�������
	// �g�U���͓��ˌ��Ɩʂ̖@���Ƃ̊Ԃ̊p�x�Ƃ̗]���Ɛ���Ⴗ��
	float diffuseFactor = dot(lightVec, normal);

	// ���C�g���ʂ̕\���ɓ������Ă���΁A�g�U���Ƌ��ʌ���ǉ�
	[flatten] if (diffuseFactor > 0.0f)
	{
		// ���˃x�N�g��
		float3 v = reflect(-lightVec, normal);

		// cos(��) = s, �� = ���E�Ɣ��ˌ��̊p�x
		float s = max(dot(v, toEye), 0.0f);

		// ���ʌ��p���[���l��
		float specFactor = pow(s, mat.Specular.w);

		// �g�U���Ƌ��ʌ�������
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}

	// �������痣���قǌ�������
	// �t2��̖@����� (an�͌W��, d�͖ʂƌ����̋���)
	// ���x�� 1 / ( a0 + a1 * d + a2 * d^2) �ɐ���Ⴗ��
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
	// �o�̓J���[
	ambient = mat.Ambient * L.Ambient;
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// �ʁ����� �x�N�g��
	float3 lightVec = L.Pos - pos;

	// �ʂ�������܂ł̋���
	float d = length(lightVec);

	// ���C�g�̋��E���O�Ȃ�I��
	if (d > L.Range) return;

	// ���C�g�x�N�g���̐��K��
	lightVec \= d;
	
	// �����x���g�̗]�������
	// �g�U���͓��ˌ��Ɩʂ̖@���Ƃ̊Ԃ̊p�x�Ƃ̗]���Ɛ���Ⴗ��
	float diffuseFactor = dot(lightVec, normal);

	// ���C�g���ʂ̕\���ɓ������Ă���΁A�g�U���Ƌ��ʌ���ǉ�
	[flatten] if (diffuseFactor > 0.0f)
	{
		// ���˃x�N�g��
		float3 v = reflect(-lightVec, normal);

		// cos(��) = s, �� = ���E�Ɣ��ˌ��̊p�x
		float s = max(dot(v, toEye), 0.0f);

		// ���ʌ��p���[���l��
		float specFactor = pow(s, mat.Specular.w);

		// �g�U���Ƌ��ʌ�������
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}

	// ���ʌ��Ɠ��l�ɃX�|�b�g�ɂ����錸�����v�Z
	float sp = max(dot(-lightVec, L.Direction), 0.0f);

	// �����̋������l��
	float spot = pow(sp, L.Spot);

	// �������痣���قǌ�������
	// �t2��̖@����� (an�͌W��, d�͖ʂƌ����̋���)
	// ���x�� 1 / ( a0 + a1 * d + a2 * d^2) �ɐ���Ⴗ��
	float att = spot / dot(L.Att, float3(1.0f, d, d*d));

	ambient *= spot;
	diffuse *= att;
	spec *= att;
}