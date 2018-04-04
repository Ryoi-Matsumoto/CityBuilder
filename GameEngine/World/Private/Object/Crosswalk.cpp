#include "Crosswalk.h"
#include "Corner.h"
#include "Street.h"
#include "Intersection.h"

//======================================================================
// �f�U�C��
//======================================================================

void SCrosswalkParameter::Update(const SCrosswalkUnique& Unique)
{
	ParameterAddProperty(AlignStreetIndex, 0, Unique.Intersection->GetNumStreet());
	ParameterAddProperty(Shift, 0, 50);
	ParameterAddProperty(Width, 0, 50);
}

void SCrosswalkTemp::Update(const SCrosswalkUnique& Unique, const SCrosswalkParameter& Parameter)
{
    // ���f�����̕����x�N�g��(�E����)
	auto AlginStreet = Unique.Intersection->GetStreets()[Parameter.AlignStreetIndex];
    auto Direction = AlginStreet->Direction;
    float Angle = Vector2::Angle(Unique.Street->Direction, Direction);
    if (Angle < PI / 6.0f)Direction = Direction.Orthogonal();

    // �߂����̊�_
    int Index = Unique.Corners[0]->GetArcAngle() > Unique.Corners[1]->GetArcAngle() ? 0 : 1;
	auto NearStandard = Unique.Corners[Index]->GetOuterArcMiddle();

    // �O��̒���
    auto FrontToBack = Direction.Orthogonal();
    auto FrontSide = SLine(NearStandard + FrontToBack * Parameter.Shift, Direction);
    auto BackSide = SLine(NearStandard + FrontToBack * (Parameter.Shift - Parameter.Width), Direction);

    // ���f�����̍��E�̕����̃J�[�u���ȗ�����������
    auto LeftCurve = SLine::TwoPoints(Unique.Corners[0]->GetOuterEdge(0), Unique.Corners[0]->GetOuterEdge(1));
    auto RightCurve = SLine::TwoPoints(Unique.Corners[1]->GetOuterEdge(0), Unique.Corners[1]->GetOuterEdge(1));

    // �����`�쐬
    auto FrontLeft = SLine::CrossPoint(FrontSide, LeftCurve);
    auto FrontRight = SLine::CrossPoint(FrontSide, RightCurve);

    this->Rectangle = SQuadrangle2::TwoPointsAndWidth(FrontLeft, FrontRight, Parameter.Width);
	this->FrontSide = FrontSide;
	this->BackSide = BackSide;
}

//======================================================================
// �f�B�e�B�[��
//======================================================================

FStartupShape* FDetailCrosswalk::CreateShape()
{
	return new FStartupShape(Crosswalk->GetRectangle().ToRectangle3(Intersection->GetElevation()));
}