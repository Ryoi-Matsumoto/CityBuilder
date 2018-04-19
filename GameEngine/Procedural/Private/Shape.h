#pragma once

struct SMaterialObject
{
	string Texture;
	float Specular;
};

struct SShapeObject
{
	vector<array<uint32,3>> Indexes;
	SMaterialObject Material;
};

struct SShapeIndex
{
	long Index;
	float3 Position;
	float3 Rotation;
};

struct SModelObject
{
	vector<float3> Vertexes;
	vector<SShapeObject> Shapes;
	vector<SShapeIndex> ShapeIndex;
};