#include "DX11Shader.h"
#include "DX11Device.h"
#include "DX11Texture.h"
#include "DX11Buffer.h"
#include <d3dCompiler.h>
#pragma comment(lib, "d3dCompiler.lib")
#pragma comment(lib, "dxguid.lib")

DXGI_FORMAT GetDxgiFormat(D3D10_REGISTER_COMPONENT_TYPE Type, BYTE Mask)
{
	switch (Mask)
	{
	case 0x0F:
		switch (Type)
		{
		case D3D10_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case D3D10_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32G32B32A32_SINT;
		case D3D10_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32B32A32_UINT;
		default:
			FDebugger::OutputError(L"Mask is XYZW, but Type is wrong.");
			return DXGI_FORMAT_UNKNOWN;
		}
	case 0x07:
		switch (Type)
		{
		case D3D10_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case D3D10_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32G32B32_SINT;
		case D3D10_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32B32_UINT;
		default:
			FDebugger::OutputError(L"Mask is XYZ, but Type is wrong.");
			return DXGI_FORMAT_UNKNOWN;
		}
	case 0x3:
		switch (Type)
		{
		case D3D10_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32_FLOAT;
		case D3D10_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32G32_SINT;
		case D3D10_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32_UINT;
		default:
			FDebugger::OutputError(L"Mask is XY, but Type is wrong.");
			return DXGI_FORMAT_UNKNOWN;
		}
	case 0x1:
		switch (Type)
		{
		case D3D10_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32_FLOAT;
		case D3D10_REGISTER_COMPONENT_SINT32:
			return DXGI_FORMAT_R32_SINT;
		case D3D10_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32_UINT;
		default:
			FDebugger::OutputError(L"Mask is X, but Type is wrong.");
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	FDebugger::OutputError(L"Mask is wrong.");
	return DXGI_FORMAT_UNKNOWN;
}

auto CreateInputLayout(ID3D11Device* Device, ID3D11ShaderReflection* Reflection, 
	D3D11_SHADER_DESC ShaderDesc, binarydata const& Binary)
{
	ID3D11InputLayout* Result;
	vector<D3D11_INPUT_ELEMENT_DESC> InputElementDescs;
	
	for (uint i = 0; i<ShaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC ParamDesc;
		HR(Reflection->GetInputParameterDesc(i, &ParamDesc));

		auto Format = GetDxgiFormat(ParamDesc.ComponentType, ParamDesc.Mask);

		D3D11_INPUT_ELEMENT_DESC ElementDesc;
		ElementDesc.SemanticName = ParamDesc.SemanticName;
		ElementDesc.SemanticIndex = ParamDesc.SemanticIndex;
		ElementDesc.Format = Format;
		ElementDesc.InputSlot = 0;
		ElementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		ElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		ElementDesc.InstanceDataStepRate = 0;

		InputElementDescs.push_back(ElementDesc);
	}

	HR(Device->CreateInputLayout
	(
		&InputElementDescs[0],
		InputElementDescs.size(),
		&Binary[0],
		Binary.size(),
		&Result
	));

	return Result;
}

auto CreateConstantBuffers(ID3D11Device* Device, ID3D11ShaderReflection* Reflection, D3D11_SHADER_DESC ShaderDesc)
{
	vector<ID3D11Buffer*> Result;

	for (uint i = 0; i < ShaderDesc.ConstantBuffers; i++) 
	{
		auto ReflectionConstantBuffer = Reflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC BufferDesc;
		HR(ReflectionConstantBuffer->GetDesc(&BufferDesc));
			
		D3D11_BUFFER_DESC Desc;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.ByteWidth = BufferDesc.Size;
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = 0;
		Desc.MiscFlags = 0;
		Desc.StructureByteStride = 0;

		ID3D11Buffer* Buffer;
		HR(Device->CreateBuffer(&Desc, nullptr, &Buffer));		
			
		Result.push_back(Buffer);
	}

	return Result;
}

uint GetTextureCount(ID3D11Device* Device, ID3D11ShaderReflection* Reflection, D3D11_SHADER_DESC ShaderDesc)
{
	uint TextureCount = 0;

	for (uint i = 0; i < ShaderDesc.BoundResources; i++)
	{
		D3D11_SHADER_INPUT_BIND_DESC Desc;
		HR(Reflection->GetResourceBindingDesc(i, &Desc));
		ID3D10ShaderResourceView* ResourceView;
		switch (Desc.Type)
		{
		case D3D_SIT_CBUFFER:
			break;
		case D3D_SIT_TEXTURE:
			TextureCount++;
			break;
		case D3D_SIT_SAMPLER:
			break;
		}
	}

	return TextureCount;
}

FDX11Shader::FDX11Shader(ID3D11Device* Device, ERHIShaderType ShaderType, binarydata Binary)
	: FRHIShader(ShaderType)
	, InputLayout(nullptr)
	, IsConstantBuffersUpdated(false)
	, IsShaderResourcesUpdated(false)
{
	Device->GetImmediateContext(&Context);

	ID3D11ShaderReflection* Reflection;
	HR(D3DReflect(&Binary[0], Binary.size(), IID_ID3D11ShaderReflection, (void**)&Reflection));
	
	D3D11_SHADER_DESC ShaderDesc;
	HR(Reflection->GetDesc(&ShaderDesc));

	ConstantBuffers = CreateConstantBuffers(Device, Reflection, ShaderDesc);
	ShaderResources.resize(GetTextureCount(Device, Reflection, ShaderDesc));

	switch (ShaderType)
	{
	case ERHIShaderType::Vertex:
		InputLayout = CreateInputLayout(Device, Reflection, ShaderDesc, Binary);
		HR(Device->CreateVertexShader(&Binary[0], Binary.size(), nullptr, &VertexShader));	
		break;
	case ERHIShaderType::Pixel:
		HR(Device->CreatePixelShader(&Binary[0], Binary.size(), nullptr, &PixelShader));
		break;
	default:
		FDebugger::OutputError(L"ShaderType is wrong.");
		return;
	}

	Reflection->Release();
}

FDX11Shader::~FDX11Shader()
{
	Shader->Release();
	if (InputLayout)InputLayout->Release();
	for (auto ContextBuffer : ConstantBuffers)ContextBuffer->Release();
	for (auto ShaderResource : ShaderResources)ShaderResource->Release();
}

void FDX11Shader::SetBuffer(uint Index, const void* Buffer)
{
	assert(ConstantBuffers.size() > 0);
	assert(Index <= ConstantBuffers.size());
	Context->UpdateSubresource(ConstantBuffers[Index], 0, nullptr, Buffer, 0, 0);
	IsConstantBuffersUpdated = true;
}

void FDX11Shader::SetTexture(uint Index, FRHITexture* Texture)
{
	assert(ShaderResources.size() > 0);
	assert(Index <= ShaderResources.size());
	ShaderResources[Index] = ((FDX11Texture*)Texture)->GetShaderResourceView();
	IsShaderResourcesUpdated = true;
}

void FDX11Shader::Apply()
{
	assert(IsConstantBuffersUpdated || ConstantBuffers.size() == 0);
	assert(IsShaderResourcesUpdated || ShaderResources.size() == 0);

	switch (GetShaderType())
	{
	case ERHIShaderType::Vertex:
		Context->IASetInputLayout(InputLayout);
		Context->VSSetConstantBuffers(0, ConstantBuffers.size(), &ConstantBuffers[0]);
		if(ShaderResources.size() > 0)Context->VSSetShaderResources(0, ShaderResources.size(), &ShaderResources[0]);
		Context->VSSetShader(VertexShader, nullptr, 0);
		break;
	case ERHIShaderType::Pixel:
		Context->PSSetConstantBuffers(0, ConstantBuffers.size(), &ConstantBuffers[0]);
		if (ShaderResources.size() > 0)Context->PSSetShaderResources(0, ShaderResources.size(), &ShaderResources[0]);
		Context->PSSetShader(PixelShader, nullptr, 0);
		break;
	}

	IsConstantBuffersUpdated = false;
	IsShaderResourcesUpdated = false;
}