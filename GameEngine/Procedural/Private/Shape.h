#pragma once
#include "Common.h"

struct SMaterial
{
	string Texture;
	float Specular;
};

struct SShape
{
	vector<uint> Triangles;
	SMaterial Material;
};

struct SShapeIndex
{
	long Index;
	float3 Position;
	float3 Rotation;
};

struct SModel
{
	vector<float3> Vertexes;
	vector<SShape> Shapes;
	vector<SShapeIndex> ShapeIndex;
};