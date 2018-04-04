#include "Crosswalk.h"
#include "Corner.h"
#include "Street.h"
#include "Intersection.h"

//======================================================================
// デザイン
//======================================================================

void SCrosswalkParameter::Update(const SCrosswalkUnique& Unique)
{
	ParameterAddProperty(AlignStreetIndex, 0, Unique.Intersection->GetNumStreet());
	ParameterAddProperty(Shift, 0, 50);
	ParameterAddProperty(Width, 0, 50);
}

void SCrosswalkTemp::Update(const SCrosswalkUnique& Unique, const SCrosswalkParameter& Parameter)
{
    // 横断歩道の方向ベクトル(右向き)
	auto AlginStreet = Unique.Intersection->GetStreets()[Parameter.AlignStreetIndex];
    auto Direction = AlginStreet->Direction;
    float Angle = Vector2::Angle(Unique.Street->Direction, Direction);
    if (Angle < PI / 6.0f)Direction = Direction.Orthogonal();

    // 近い方の基準点
    int Index = Unique.Corners[0]->GetArcAngle() > Unique.Corners[1]->GetArcAngle() ? 0 : 1;
	auto NearStandard = Unique.Corners[Index]->GetOuterArcMiddle();

    // 前後の直線
    auto FrontToBack = Direction.Orthogonal();
    auto FrontSide = SLine(NearStandard + FrontToBack * Parameter.Shift, Direction);
    auto BackSide = SLine(NearStandard + FrontToBack * (Parameter.Shift - Parameter.Width), Direction);

    // 横断歩道の左右の歩道のカーブを簡略化した直線
    auto LeftCurve = SLine::TwoPoints(Unique.Corners[0]->GetOuterEdge(0), Unique.Corners[0]->GetOuterEdge(1));
    auto RightCurve = SLine::TwoPoints(Unique.Corners[1]->GetOuterEdge(0), Unique.Corners[1]->GetOuterEdge(1));

    // 長方形作成
    auto FrontLeft = SLine::CrossPoint(FrontSide, LeftCurve);
    auto FrontRight = SLine::CrossPoint(FrontSide, RightCurve);

    this->Rectangle = SQuadrangle2::TwoPointsAndWidth(FrontLeft, FrontRight, Parameter.Width);
	this->FrontSide = FrontSide;
	this->BackSide = BackSide;
}

//======================================================================
// ディティール
//======================================================================

FStartupShape* FDetailCrosswalk::CreateShape()
{
	return new FStartupShape(Crosswalk->GetRectangle().ToRectangle3(Intersection->GetElevation()));
}