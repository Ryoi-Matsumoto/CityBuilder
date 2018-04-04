#pragma once
#include "Core\System\BasicLibrary.h"

enum class EShaderParameterType
{
	None, Scalar, Vector, ScalarTexture, VectorTexture
};

struct SShaderDesc
{
	EShaderParameterType Ambient;
	EShaderParameterType Diffuse;
	EShaderParameterType Specular;
	EShaderParameterType SpecularPower;
	EShaderParameterType Normal;   
};

class FShaderCompiler
{
private:

public:
	FShaderCompiler();


	static void Compile(char* FileName);

};