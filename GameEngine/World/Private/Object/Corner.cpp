#include "Corner.h"
#include "Street.h"
#include "Crosswalk.h"

//======================================================================
// ユーティリティ
//======================================================================

// 時計回りで角度と角度の間の角度を取得
inline float DistanceAngle(float Angle0, float Angle1)
{
	// 0度と360度の境界が影響しない場合
	if (Angle0 > Angle1)return Angle0 - Angle1;
	else return Angle0 + (PI * 2.0f - Angle1);
}

// 時計回りでStartからEndへの推移を取得
// @Degree 0.0fのときStart, 1.0fのときEnd
inline float ApproachAngle(float Start, float End, float Degree)
{
    // 0度と360度の境界が影響しない場合
    if(Start > End)
    {
        return Start * Degree + End * (1.0f - Degree);
    }
    else
    {
        // Startから推移する角度
        float Move = (Start + (PI * 2.0f - End)) * Degree;

        if(Move < Start)return Start - Move;
        else return Move - Start;
    }
}

//======================================================================
// デザイン
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
	// 　角度
	//======================================================================

	// 道と道の交わる角度
	float CrossAngle = Vector2::Angle(Unique.Streets[0]->Direction, Unique.Streets[1]->Direction);

	// カーブの弧の角度
	float ArcAngle = PI - CrossAngle;    
	
	float EndAngle = Unique.Streets[0]->Direction.Orthogonal().Atan();
    float StartAngle = Unique.Streets[1]->Direction.Orthogonal().Atan();

	//======================================================================
	// 　長さ
	//======================================================================

	// 車道と歩道の境界線の交点とその境界線のカーブ部分の終端の距離
	float LenghtFromCrossToEdge = Parameter.Radius / tanf(CrossAngle / 2.0f);

	// 車道と歩道の境界線のカーブ部分の終端とカーブの弧の中心点の距離
	float LenghtFromCrossToCentor = Parameter.Radius / sinf(CrossAngle / 2.0f);

	// 幅
	float Width = Unique.Streets[Parameter.BelongStreetIndex]->SidewalkWidths[1 - Parameter.BelongStreetIndex];;

	//======================================================================
	// 　座標
	//======================================================================

	// 交わる道の方向ベクトルの二等分線
	auto HalfDirection = (Unique.Streets[0]->Direction + Unique.Streets[1]->Direction) / 2.0f;

	// 歩道と車道の境界線の交点
	auto LeftBoundaryLine = Unique.Streets[0]->BoundaryLines[1];
	auto RightBoundaryLine = Unique.Streets[1]->BoundaryLines[0];
	auto OuterCrossPoint = SLine::CrossPoint(LeftBoundaryLine, RightBoundaryLine);

	// カーブの弧の中心点	
	auto FromCrossToCentor = -HalfDirection * LenghtFromCrossToCentor;
	auto CentorPoint = OuterCrossPoint + FromCrossToCentor;

	// 交わる道の方向ベクトルの二等分線
	auto Lenght = LenghtFromCrossToCentor - Parameter.Radius;
	auto OuterArcMiddlePoint = OuterCrossPoint - HalfDirection * Lenght;

	// 歩道と車道の境界線の端
	Vector2 OuterEdgePoints[2];
	for (int Side = 0; Side < 2; Side++)
	{
		// 道の交点からカーブの終端までのベクトル
		auto FromCrossToEdge = Unique.Streets[Side]->Direction * LenghtFromCrossToEdge;
		OuterEdgePoints[Side] = OuterCrossPoint - FromCrossToEdge;
	}

	// 道の側の端
	Vector2 InnerEdgePoints[2];
	for (int Side = 0; Side < 2; Side++)
	{
		// 歩道と区画の境界から車道と歩道の境界へのベクトル
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
// ディティール
//======================================================================

struct SPairedCurveWithAngle : SPairedCurve
{
	vector<float> Angles;
};

// 未分割のカーブを生成
// @Out 0が外側、1が内側
void FDetailCorner::BuildOriginCurve(SPairedCurveWithAngle* Out, float Elevetion)
{
    // OuterSplit >= InnerSplit
    // OuterSplitはInnerSplitの倍数
    int InnerSplit = Corner->GetInnerSplit(), OuterSplit = Corner->GetOuterSplit();
    if(OuterSplit < InnerSplit)OuterSplit = InnerSplit;
    else OuterSplit += InnerSplit - OuterSplit % InnerSplit;
	
	// 配列確保
	Out->Pairs.reserve(OuterSplit);
	Out->Angles.resize(OuterSplit);

	// 外側の分割数は内側の何倍か
    int InnerRate = OuterSplit / InnerSplit;

	// 3次元中心座標
    auto Centor = float3(Corner->GetCentor(), Elevetion);

    // 単曲線部分
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

    // 直線部分
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

    // 未分割のカーブの原型を作成
	SPairedCurveWithAngle OriginCurve;
    BuildOriginCurve(&OriginCurve, elevetion);

    // [0] 通常 [1] 横断部分 [2] 通常 [3] 横断部分 [4] 通常 [5]
    // [0] 通常                     [1] 横断部分 [2] 通常 [3]
    // [0] 通常 [1] 通常部分 [2]                     通常 [3]
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

	// 配列を確保
	ShapeOrigin.resize(Angles.size() - 1);

	// 分割する
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
			// Left → Angle → Rightとなっていなければそのまま追加
			if (DistanceAngle(Angle, Right) < Between)
			{							
				NewPart.Pairs.push_back(Pair);

				// 今回の分を格納
				PrevPair = Pair;
				PrevAngle = Angle;

				// 次回の分を用意
				Pair = OriginCurve.Pairs[Index + 1];
				Angle = OriginCurve.Angles[Index + 1];
			}
			// Left → Right → Angleとなっていれば分割
			else if(1 < Index)
			{
				// どれだけズレているか
				float ShiftAngle = DistanceAngle(Right, Angle);
				float ShiftRatio = ShiftAngle / DistanceAngle(PrevAngle, Angle);

				// ズレを補正したペアを作成
				auto Outer = PrevPair[0] * (1.0f - ShiftRatio) + Pair[0] * ShiftRatio;
				auto Inner = PrevPair[1] * (1.0f - ShiftRatio) + Pair[1] * ShiftRatio;
				array<float3, 2> NewPair = { Outer, Inner };

				// 追加
				NewPart.Pairs.push_back(NewPair);

				// １つ戻す
				Index--;

				// 今回の分を格納
				// (１つ戻すので更新する必要なし)
				// PrevPair = PrevPair;
				// PrevAngle = PrevAngle;

				// 次回の分を用意
				Pair = NewPair;
				Angle = PrevAngle + ShiftAngle;

				break;
			}			
			else
			{
				// ここには来ないはず
				assert(false);
			}
		}

		ShapeOrigin[i] = NewPart;
	}

	return new FStartupShape(ShapeOrigin);
}