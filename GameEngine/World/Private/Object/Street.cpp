#include "Street.h"
#include "Crosswalk.h"

//======================================================================
// �f�U�C��
//======================================================================

void SConnectedStreet::Update(const SStreetParameter& Parameter, int ConnectedIndex)
{
	// �ڑ�����Ă�����������������x�N�g��
	if (ConnectedIndex == 0)Direction = Parameter.CentorsOfRoadway[ConnectedIndex].Direction;
	else Direction = -Parameter.CentorsOfRoadway[ConnectedIndex].Direction;

	// �����̕�
	for (int i = 0; i < 2; i++)
	{	
		int Side = i;
		if (ConnectedIndex == 1)Side = 1 - i;
		SidewalkWidths[i] = Parameter.SidewalkWidths[Side];
	}

	// �����Ƌ��̋��E��
	for (int Side = 0; Side < 2; Side++)
	{
		// �ԓ��̕� / 2 + �����̕�
		float Width = Parameter.RoadwayWidth / 2.0f + Parameter.SidewalkWidths[Side];
		if (Side == 1)Width *= -1.0f;
		// �C�ӂ�1�_ + ���ɐ��� * ��
		auto OnePoint = Parameter.CentorsOfRoadway[ConnectedIndex].OnePoint + Direction.Orthogonal() * Width;
		SideLines[Side] = SLine(OnePoint, Direction);
	}

	// �ԓ��ƕ����̋��E��
	for (int Side = 0; Side < 2; Side++)
	{
		// �ԓ��̕� / 2
		float Width = Parameter.RoadwayWidth / 2.0f;
		if (Side == 1)Width *= -1.0f;
		// �C�ӂ�1�_ + ���ɐ��� * ��
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
// �f�B�e�B�[��
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
