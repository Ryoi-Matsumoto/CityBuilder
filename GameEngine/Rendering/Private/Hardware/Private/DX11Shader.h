#pragma once
#include "Common.h"

class FDX11Shader : public FRHIShader
{ 
private:	
	ID3D11DeviceContext* Context;
	ID3D11InputLayout* InputLayout;
	vector<ID3D11Buffer*> ConstantBuffers;
	vector<ID3D11ShaderResourceView*> ShaderResources;
	bool IsConstantBuffersUpdated;
	bool IsShaderResourcesUpdated;
	union
	{
		IUnknown* Shader;
		ID3D11VertexShader* VertexShader;
		ID3D11PixelShader* PixelShader;
	};

public:
	~FDX11Shader() override;
	void SetBuffer(uint Index, const void* Buffer) override;
	void SetTexture(uint Index, FRHITexture* Texture) override;	
	
	FDX11Shader(ID3D11Device* Device, ERHIShaderType ShaderType, vector<char> Binary);
	ID3D11VertexShader* GetVertexShader() const { return VertexShader; }
	ID3D11PixelShader* GetPixelShader() const { return PixelShader; }
	void Apply();
};