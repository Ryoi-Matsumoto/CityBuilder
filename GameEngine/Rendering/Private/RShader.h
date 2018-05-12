#pragma once
#include "Hardware\Public\RHIDevice.h"
#include "Hardware\Public\RHIResource.h"
#include "RResourceManager.h"
#include <array>

#define _Shader(X, Y)	\
void Initialize##X##Y(FRHIDevice* Device) \
{ \
	Shader##X##Y = Device->CreateShader(ERHIShaderType::##Y, \
	FFileManager::ReadBinaryFile(BeginFilePass + (#X #Y) + EndFilePass)); \
} \
struct SRBuffer##X##Y; \
unique_ptr<FRHIShader> Shader##X##Y; \
FRHIShader* GetShader##X##Y(SRBuffer##X##Y const& Buffer){ SetBuffer(Shader##X##Y.get(), &Buffer); return Shader##X##Y.get(); } \
struct SRBuffer##X##Y {
#define _ShaderEnd };
#define _TEXTURE_START_ struct SRBufferTexture {
#define _TEXTURE_END_ }; SRBufferTexture Textures;
#define Texture2D FRHITexture*

class FRShaderManager
{
private:
	string BeginFilePass = "Shaders/";
	string EndFilePass = ".cso";

private:
	template<typename TBuffer>
	void SetBuffer(FRHIShader* Shader, TBuffer* Buffer)
	{
		Shader->SetBuffer(0, Buffer);
		if (sizeof(TBuffer::SRBufferTexture) > sizeof(char))
		{
			using TextureArray = array<FRHITexture*, sizeof(TBuffer::SRBufferTexture) / sizeof(FRHITexture*)>;
			auto Textures = *((TextureArray*)&Buffer->Textures);

			uint Index = 0;
			for(auto Texture : Textures)
			{
				Shader->SetTexture(Index, Texture);
				Index++;
			}
		}
	}

public:
	FRShaderManager(FRHIDevice* Device)
	{
		InitializeBoardPixel(Device);
		InitializeBoardTexturePixel(Device);
		InitializeBoardVertex(Device);	
	}

_Shader(Board, Pixel)
#include "Shaders\BoardPixel.h"
_ShaderEnd

_Shader(BoardTexture, Pixel)
#include "Shaders\BoardTexturePixel.h"
_ShaderEnd

_Shader(Board, Vertex)
#include "Shaders\BoardVertex.h"
_ShaderEnd
};