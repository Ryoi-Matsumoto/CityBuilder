#pragma once
#include "Common.h"

//======================================================================
// �f�U�C��
//======================================================================

struct SStreetUnique
{
};

struct SStreetParameter
{
	// ���[��Roadway�̒��S��
	SLine CentorsOfRoadway[2];
	// �����Ǝԓ��̂��ꂼ��̕�
	float RoadwayWidth;
	// �����x�N�g���Ɠ��������Ɍ��č���0�E��1
	float SidewalkWidths[2];
};

struct SConnectedStreet
{
	void Update(const SStreetParameter& Parameter, int ConnectedIndex);

	// if 0 : GetDirection()�̎w���Ă�������ɐڑ�����Ă���
	// if 1 : GetDirection()�̎w���Ă�������̔��Α��ɐڑ�����Ă���
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
// �f�B�e�B�[��
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