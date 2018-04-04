#pragma once
#include "Common.h"

//======================================================================
// デザイン
//======================================================================

struct SStreetUnique
{
};

struct SStreetParameter
{
	// 両端のRoadwayの中心線
	SLine CentorsOfRoadway[2];
	// 歩道と車道のそれぞれの幅
	float RoadwayWidth;
	// 方向ベクトルと同じ向きに見て左が0右が1
	float SidewalkWidths[2];
};

struct SConnectedStreet
{
	void Update(const SStreetParameter& Parameter, int ConnectedIndex);

	// if 0 : GetDirection()の指している方向に接続されている
	// if 1 : GetDirection()の指している方向の反対側に接続されている
	uint ConnectedIndex;

	SLine SideLines[2];
	SLine BoundaryLines[2];
	float SidewalkWidths[2];
	Vector2 Direction;
};

struct SStreetTemp
{
	void Update(const SStreetUnique& Unique, const SStreetParameter& Parameter);

	SConnectedStreet ConnectedStreets[2];
};

class FDesignStreet : public FDesign<SStreetUnique, SStreetParameter, SStreetTemp>
{
private:
	static float SplitPerLenght;

public:
	FDesignStreet(SLine StartLine, SLine EndLine)
	{
		Parameter.CentorsOfRoadway[0] = StartLine;
		Parameter.CentorsOfRoadway[1] = EndLine;
	}

	const SConnectedStreet* GetConnectedStreet(int Side) { return &Temp.ConnectedStreets[Side]; }
};

//======================================================================
// ディティール
//======================================================================

class FDesignCrosswalk;

class FDetailStreet : public FDetail
{
private:
	FDesignCrosswalk* StartCrosswalk;
	FDesignCrosswalk* EndCrosswalk;

public:
	FDetailStreet(FDesignCrosswalk* StartCrosswalk, FDesignCrosswalk* EndCrosswalk)
		: StartCrosswalk(StartCrosswalk), EndCrosswalk(EndCrosswalk)
	{
	}

	FStartupShape* CreateShape() override;
};