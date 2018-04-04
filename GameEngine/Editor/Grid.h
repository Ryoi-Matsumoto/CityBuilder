#pragma once
#include "Common.h"
#include "Core\Hardware\Hardware.h"

class FGrid
{
private:
	IVertexBuffer* VertexBuffer;
	Matrix WVPMatrix;
	float4 Color;

public:
	FGrid(float Size, uint SpritNum, float4 Color);
	void Render();
	void SetWVPMatrix(const Matrix& WVPMatrix) { this->WVPMatrix = WVPMatrix; }
};