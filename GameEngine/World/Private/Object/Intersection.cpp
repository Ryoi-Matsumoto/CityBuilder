#include "Intersection.h"
#include "Street.h"
#include "Corner.h"
#include "Crosswalk.h"

//======================================================================
// デザイン
//======================================================================

void SIntersectionParameter::Update(const SIntersectionUnique& Unique)
{
	ParameterAddProperty(Elevation, 0, 0);
}

void SIntersectionTemp::Update(const SIntersectionUnique& Unique, const SIntersectionParameter& Parameter)
{	
	// 初期化
	Corners.clear();
	Crosswalks.clear();
	DetailCorners.clear();
	DetailCrosswalks.clear();

	// 絶対角度でソート
	vector<const SConnectedStreet*> SortedStreets(Parameter.Streets);
	auto sortFunc = [](const SConnectedStreet* x, const SConnectedStreet* y)
	{
		return (-(x->Direction)).Atan() < (-(y->Direction)).Atan();
	};
	sort(SortedStreets.begin(), SortedStreets.end(), sortFunc);

	// 全てのコーナーを作成
	for (uint i = 0; i < SortedStreets.size(); i++)
	{
		uint Left = i, Right = i + 1;
		if (Right >= SortedStreets.size())Right = 0;

		// コーナー作成
		auto Corner = new FDesignCorner(SortedStreets[Left], SortedStreets[Right]);
		Corners.push_back(Corner);		
	}

	// 　全ての横断歩道を作成
	for (uint i = 0; i < SortedStreets.size(); i++)
	{
		uint Left = i, Right = i + 1;
		if (Right >= SortedStreets.size())Right = 0;

		// 横断歩道作成
		auto Crosswalk = new FDesignCrosswalk
		(
			Unique.Intersection, 
			SortedStreets[i], 
			Corners[Left], 
			Corners[Right]
		);
		Crosswalks.push_back(Crosswalk);
	}

	// ディティールを作成
	for (uint i = 0; i < SortedStreets.size(); i++)
	{
		uint Left = i, Right = i + 1;
		if (Right >= SortedStreets.size())Right = 0;

		auto DetailCorner = new FDetailCorner(Corners[i], Crosswalks[Left], Crosswalks[Right]);
		auto DetailCrosswalk = new FDetailCrosswalk(Crosswalks[i], Unique.Intersection);

		DetailCorners.push_back(DetailCorner);
		DetailCrosswalks.push_back(DetailCrosswalk);

		Unique.World->AddDetail(DetailCorner);
		Unique.World->AddDetail(DetailCrosswalk);
	}
}

void FDesignIntersection::AddStreet(const SConnectedStreet* ConnectedStreet)
{
	Parameter.Streets.push_back(ConnectedStreet);
	OnParameterChanged();
}

//======================================================================
// ディティール
//======================================================================

