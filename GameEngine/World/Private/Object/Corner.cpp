#include "Corner.h"
#include "Street.h"
#include "Crosswalk.h"

//======================================================================
// ���[�e�B���e�B
//======================================================================

// ���v���Ŋp�x�Ɗp�x�̊Ԃ̊p�x���擾
inline float DistanceAngle(float Angle0, float Angle1)
{
	// 0�x��360�x�̋��E���e�����Ȃ��ꍇ
	if (Angle0 > Angle1)return Angle0 - Angle1;
	else return Angle0 + (PI * 2.0f - Angle1);
}

// ���v����Start����End�ւ̐��ڂ��擾
// @Degree 0.0f�̂Ƃ�Start, 1.0f�̂Ƃ�End
inline float ApproachAngle(float Start, float End, float Degree)
{
    // 0�x��360�x�̋��E���e�����Ȃ��ꍇ
    if(Start > End)
    {
        return Start * Degree + End * (1.0f - Degree);
    }
    else
    {
        // Start���琄�ڂ���p�x
        float Move = (Start + (PI * 2.0f - End)) * Degree;

        if(Move < Start)return Start - Move;
        else return Move - Start;
    }
}

//======================================================================
// �f�U�C��
//======================================================================

void SCornerParameter::Update(const SCornerUnique& Unique)
{
	ParameterAddProperty(BelongStreetIndex, 0, 100);
	ParameterAddProperty(Radius, 0.01, 100);
	ParameterAddProperty(InnerSplit, 0.01, 100);
	ParameterAddProperty(OuterSplit, 0.01, 100);
}

void SCornerTemp::Update(const SCornerUnique& Unique, const SCornerParameter& Parameter)
{
	//======================================================================
	// �@�p�x
	//======================================================================

	// ���Ɠ��̌����p�x
	float CrossAngle = Vector2::Angle(Unique.Streets[0]->Direction, Unique.Streets[1]->Direction);

	// �J�[�u�̌ʂ̊p�x
	float ArcAngle = PI - CrossAngle;    
	
	float EndAngle = Unique.Streets[0]->Direction.Orthogonal().Atan();
    float StartAngle = Unique.Streets[1]->Direction.Orthogonal().Atan();

	//======================================================================
	// �@����
	//======================================================================

	// �ԓ��ƕ����̋��E���̌�_�Ƃ��̋��E���̃J�[�u�����̏I�[�̋���
	float LenghtFromCrossToEdge = Parameter.Radius / tanf(CrossAngle / 2.0f);

	// �ԓ��ƕ����̋��E���̃J�[�u�����̏I�[�ƃJ�[�u�̌ʂ̒��S�_�̋���
	float LenghtFromCrossToCentor = Parameter.Radius / sinf(CrossAngle / 2.0f);

	// ��
	float Width = Unique.Streets[Parameter.BelongStreetIndex]->SidewalkWidths[1 - Parameter.BelongStreetIndex];;

	//======================================================================
	// �@���W
	//======================================================================

	// ����铹�̕����x�N�g���̓񓙕���
	auto HalfDirection = (Unique.Streets[0]->Direction + Unique.Streets[1]->Direction) / 2.0f;

	// �����Ǝԓ��̋��E���̌�_
	auto LeftBoundaryLine = Unique.Streets[0]->BoundaryLines[1];
	auto RightBoundaryLine = Unique.Streets[1]->BoundaryLines[0];
	auto OuterCrossPoint = SLine::CrossPoint(LeftBoundaryLine, RightBoundaryLine);

	// �J�[�u�̌ʂ̒��S�_	
	auto FromCrossToCentor = -HalfDirection * LenghtFromCrossToCentor;
	auto CentorPoint = OuterCrossPoint + FromCrossToCentor;

	// ����铹�̕����x�N�g���̓񓙕���
	auto Lenght = LenghtFromCrossToCentor - Parameter.Radius;
	auto OuterArcMiddlePoint = OuterCrossPoint - HalfDirection * Lenght;

	// �����Ǝԓ��̋��E���̒[
	Vector2 OuterEdgePoints[2];
	for (int Side = 0; Side < 2; Side++)
	{
		// ���̌�_����J�[�u�̏I�[�܂ł̃x�N�g��
		auto FromCrossToEdge = Unique.Streets[Side]->Direction * LenghtFromCrossToEdge;
		OuterEdgePoints[Side] = OuterCrossPoint - FromCrossToEdge;
	}

	// ���̑��̒[
	Vector2 InnerEdgePoints[2];
	for (int Side = 0; Side < 2; Side++)
	{
		// �����Ƌ��̋��E����ԓ��ƕ����̋��E�ւ̃x�N�g��
		auto FromSideToBoundary = Unique.Streets[Side]->Direction.Orthogonal() * Width;
		if (Side == 1)FromSideToBoundary *= -1.0f;
		InnerEdgePoints[Side] = OuterEdgePoints[Side] + FromSideToBoundary;
	}

	this->StartAngle = StartAngle;
	this->ArcAngle = ArcAngle;
	this->EndAngle = EndAngle;
	this->Width = Width;
	this->Centor = CentorPoint;
	this->OuterArcMiddle = OuterArcMiddlePoint;
	this->OuterEdges[0] = OuterEdgePoints[0];
	this->InnerEdges[0] = InnerEdgePoints[0];
	this->OuterEdges[1] = OuterEdgePoints[1];
	this->InnerEdges[1] = InnerEdgePoints[1];
}

//======================================================================
// �f�B�e�B�[��
//======================================================================

struct SPairedCurveWithAngle : SPairedCurve
{
	vector<float> Angles;
};

// �������̃J�[�u�𐶐�
// @Out 0���O���A1������
void FDetailCorner::BuildOriginCurve(SPairedCurveWithAngle* Out, float Elevetion)
{
    // OuterSplit >= InnerSplit
    // OuterSplit��InnerSplit�̔{��
    int InnerSplit = Corner->GetInnerSplit(), OuterSplit = Corner->GetOuterSplit();
    if(OuterSplit < InnerSplit)OuterSplit = InnerSplit;
    else OuterSplit += InnerSplit - OuterSplit % InnerSplit;
	
	// �z��m��
	Out->Pairs.reserve(OuterSplit);
	Out->Angles.resize(OuterSplit);

	// �O���̕������͓����̉��{��
    int InnerRate = OuterSplit / InnerSplit;

	// 3�������S���W
    auto Centor = float3(Corner->GetCentor(), Elevetion);

    // �P�Ȑ�����
	float StartAngle = Corner->GetStartAngle(), EndAngle = Corner->GetEndAngle();
	float InnerRadius = Corner->GetRadius();
	float OuterRadius = InnerRadius + Corner->GetWidth();
    for(int i=0;i<OuterSplit;i++)
    {		
        float Angle = ApproachAngle(StartAngle, EndAngle, (float)i / OuterSplit);		
        auto Outer = Centor + float3(cos(Angle) * OuterRadius, 0, sin(Angle) * OuterRadius);
        Out->Pairs[i][0] = Outer;
        if(i % InnerRate == 0)
        {
            auto Inner = Centor + float3(cos(Angle) * InnerRadius, 0, sin(Angle) * InnerRadius);
            Out->Pairs[i][1] = Inner;
        }
    }

    // ��������
    for(int i=0;i<OuterSplit;i++)
    {
        if(i % InnerRate > 0)
        {
            auto Prev = Out->Pairs[i - i % InnerRate][1];
            auto next = Out->Pairs[i - i % InnerRate + InnerRate][1];
            int Index = i / InnerRate, max = InnerRate;
            Out->Pairs[i][1] = Centor + Prev * (max - Index) + next * Index;
        }
    }
}

FStartupShape* FDetailCorner::CreateShape()
{
	vector<SPairedCurve> ShapeOrigin;

	// todo 
	float elevetion = 0;

    // �������̃J�[�u�̌��^���쐬
	SPairedCurveWithAngle OriginCurve;
    BuildOriginCurve(&OriginCurve, elevetion);

    // [0] �ʏ� [1] ���f���� [2] �ʏ� [3] ���f���� [4] �ʏ� [5]
    // [0] �ʏ�                     [1] ���f���� [2] �ʏ� [3]
    // [0] �ʏ� [1] �ʏ핔�� [2]                     �ʏ� [3]
    vector<float> Angles;
	Angles.push_back(Corner->GetStartAngle());
	auto Centor = Corner->GetCentor();
	float radius = Corner->GetRadius();
    if(Crosswalks[0] != nullptr)
    {
        Angles.push_back(Crosswalks[0]->GetBackSide().CrossedToCircle(Centor, radius).Atan());
        Angles.push_back(Crosswalks[0]->GetFrontSide().CrossedToCircle(Centor, radius).Atan());
    }
    if(Crosswalks[1] != nullptr)
    {
        Angles.push_back(Crosswalks[1]->GetFrontSide().CrossedToCircle(Centor, radius).Atan());
        Angles.push_back(Crosswalks[1]->GetBackSide().CrossedToCircle(Centor, radius).Atan());
    }
    Angles.push_back(Corner->GetEndAngle());

	// �z����m��
	ShapeOrigin.resize(Angles.size() - 1);

	// ��������
	int Index = 0;		
	
	array<float3, 2> PrevPair;
	float PrevAngle;

	array<float3, 2> Pair = OriginCurve.Pairs[0];
	float Angle = OriginCurve.Angles[0];

	for (int i = 0; i < (Angles.size() - 1); i++)
	{
		float Left = Angles[i];
		float Right = Angles[i + 1];
		float Between = DistanceAngle(Left, Right);

		SPairedCurve NewPart;

		for (; Index < OriginCurve.Pairs.size(); Index++)
		{	
			// Left �� Angle �� Right�ƂȂ��Ă��Ȃ���΂��̂܂ܒǉ�
			if (DistanceAngle(Angle, Right) < Between)
			{							
				NewPart.Pairs.push_back(Pair);

				// ����̕����i�[
				PrevPair = Pair;
				PrevAngle = Angle;

				// ����̕���p��
				Pair = OriginCurve.Pairs[Index + 1];
				Angle = OriginCurve.Angles[Index + 1];
			}
			// Left �� Right �� Angle�ƂȂ��Ă���Ε���
			else if(1 < Index)
			{
				// �ǂꂾ���Y���Ă��邩
				float ShiftAngle = DistanceAngle(Right, Angle);
				float ShiftRatio = ShiftAngle / DistanceAngle(PrevAngle, Angle);

				// �Y����␳�����y�A���쐬
				auto Outer = PrevPair[0] * (1.0f - ShiftRatio) + Pair[0] * ShiftRatio;
				auto Inner = PrevPair[1] * (1.0f - ShiftRatio) + Pair[1] * ShiftRatio;
				array<float3, 2> NewPair = { Outer, Inner };

				// �ǉ�
				NewPart.Pairs.push_back(NewPair);

				// �P�߂�
				Index--;

				// ����̕����i�[
				// (�P�߂��̂ōX�V����K�v�Ȃ�)
				// PrevPair = PrevPair;
				// PrevAngle = PrevAngle;

				// ����̕���p��
				Pair = NewPair;
				Angle = PrevAngle + ShiftAngle;

				break;
			}			
			else
			{
				// �����ɂ͗��Ȃ��͂�
				assert(false);
			}
		}

		ShapeOrigin[i] = NewPart;
	}

	return new FStartupShape(ShapeOrigin);
}