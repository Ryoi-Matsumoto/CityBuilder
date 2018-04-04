#pragma once
#include "Common.h"
#include "Crosswalk.h"

//======================================================================
// デザイン
//======================================================================

class SConnectedStreet;

struct SCornerUnique
{
	const SConnectedStreet* Streets[2];
};

struct SCornerParameter : SPropertySet
{
	void Update(const SCornerUnique& Unique);

	int BelongStreetIndex;
	float Radius;
	int InnerSplit;
	int OuterSplit;
};

struct SCornerTemp
{
	void Update(const SCornerUnique& Unique, const SCornerParameter& Parameter);

	float StartAngle;
	float ArcAngle;
	float EndAngle;
	float Width;
	float2 OuterEdges[2];
	float2 InnerEdges[2];
	float2 OuterArcMiddle;
	float2 Centor;
};

class FDesignCorner : public FDesign<SCornerUnique, SCornerParameter, SCornerTemp>
{
private:
	// カーブ部分の歩道と車道の境界が描く弧の長さあたりの分割数
	static float SplitPerArc;
		
public:
	FDesignCorner(const SConnectedStreet* Left, const SConnectedStreet* Right)
	{
		Unique.Streets[0] = Left;
		Unique.Streets[1] = Right;
	}

	// Getter
	const SConnectedStreet* GetStreet(int Side) const { return Unique.Streets[Side]; }

	int GetBelongStreetIndex() const { return Parameter.BelongStreetIndex; }
	float GetRadius() const { return Parameter.Radius; }
	int GetInnerSplit() const { return Parameter.InnerSplit; }
	int GetOuterSplit() const { return Parameter.OuterSplit; }

	float GetStartAngle() const { return Temp.StartAngle; }
	float GetArcAngle() const { return Temp.ArcAngle; }
	float GetEndAngle() const { return Temp.EndAngle; }
	float GetWidth() const { return Temp.Width; }
	float2 GetOuterEdge(int Side) const { return Temp.OuterEdges[Side]; }
	float2 GetInnerEdge(int Side) const { return Temp.InnerEdges[Side]; }
	float2 GetOuterArcMiddle() const { return Temp.OuterArcMiddle; }
	float2 GetCentor() const { return Temp.Centor; }
};

//======================================================================
// ディティール
//======================================================================

class FDesignCrosswalk;
struct SPairedCurveWithAngle;

class FDetailCorner : public FDetail
{
private:
	const FDesignCorner* Corner;
	const FDesignCrosswalk* Crosswalks[2];

private:
	void BuildOriginCurve(SPairedCurveWithAngle* Out, float elevetion);
	
public:
	FDetailCorner(const FDesignCorner* Corner, const FDesignCrosswalk* LeftCrosswalk, const FDesignCrosswalk* RightCrosswalk)
		: Corner(Corner), Crosswalks{LeftCrosswalk, RightCrosswalk}
	{	
		Corner->AddDetail(this);
		LeftCrosswalk->AddDetail(this);
		RightCrosswalk->AddDetail(this);
	}

	FStartupShape* CreateShape() override;
};