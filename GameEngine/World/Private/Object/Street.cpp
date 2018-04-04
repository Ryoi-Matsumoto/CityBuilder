#include "Street.h"
#include "Crosswalk.h"

//======================================================================
// デザイン
//======================================================================

void SConnectedStreet::Update(const SStreetParameter& Parameter, int ConnectedIndex)
{
	// 接続されている方を向いた方向ベクトル
	if (ConnectedIndex == 0)Direction = Parameter.CentorsOfRoadway[ConnectedIndex].Direction;
	else Direction = -Parameter.CentorsOfRoadway[ConnectedIndex].Direction;

	// 歩道の幅
	for (int i = 0; i < 2; i++)
	{	
		int Side = i;
		if (ConnectedIndex == 1)Side = 1 - i;
		SidewalkWidths[i] = Parameter.SidewalkWidths[Side];
	}

	// 歩道と区画の境界線
	for (int Side = 0; Side < 2; Side++)
	{
		// 車道の幅 / 2 + 歩道の幅
		float Width = Parameter.RoadwayWidth / 2.0f + Parameter.SidewalkWidths[Side];
		if (Side == 1)Width *= -1.0f;
		// 任意の1点 + 道に垂直 * 幅
		auto OnePoint = Parameter.CentorsOfRoadway[ConnectedIndex].OnePoint + Direction.Orthogonal() * Width;
		SideLines[Side] = SLine(OnePoint, Direction);
	}

	// 車道と歩道の境界線
	for (int Side = 0; Side < 2; Side++)
	{
		// 車道の幅 / 2
		float Width = Parameter.RoadwayWidth / 2.0f;
		if (Side == 1)Width *= -1.0f;
		// 任意の1点 + 道に垂直 * 幅
		auto OnePoint = Parameter.CentorsOfRoadway[ConnectedIndex].OnePoint + Direction.Orthogonal() * Width;
		BoundaryLines[Side] = SLine(OnePoint, Direction);
	}
}

void SStreetTemp::Update(const SStreetUnique& Unique, const SStreetParameter& Parameter)
{
	for (int i = 0; i < 2; i++)
	{
		ConnectedStreets[i].Update(Parameter, i);
	}
}

//======================================================================
// ディティール
//======================================================================

FStartupShape* FDetailStreet::CreateShape()
{
	SQuadrangle2 ShapeOrigin
	(
		StartCrosswalk->GetRectangle().BackLeft, 
		StartCrosswalk->GetRectangle().BackRight,
		EndCrosswalk->GetRectangle().BackLeft,
		EndCrosswalk->GetRectangle().BackRight
	);
	return new FStartupShape(ShapeOrigin.ToRectangle3(0));
}
