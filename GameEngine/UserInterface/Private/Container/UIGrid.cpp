#include "UIGrid.h"
#include <numeric>

void GetLenghts(vector<uint>* Out, vector<SUILenght> const& Defines, uint Size)
{
	float TotalNormal = 0;
	float TotalRatio = 0;

	for (auto Define : Defines)
	{
		if (Define.IsRatio)
			TotalRatio += Define.Lenght;
		else 
			TotalNormal += Define.Lenght;
	}

	float Margin = FMath::Max(Size - TotalNormal, 0);

	for (auto Define : Defines)
	{
		if (Define.IsRatio)
			Out->push_back(Margin * Define.Lenght / TotalRatio);
		else 
			Out->push_back(Define.Lenght);
	}
}

void FUIGrid::OnAreaChanged()
{
	auto CopyXDefines = XDefines;
	auto CopyYDefines = YDefines;
	for (auto& Child : Children)
	{
		if (XDefines.size() > 0 && XDefines[Child->XIndex].IsAuto)
			CopyXDefines[Child->XIndex].Lenght = FMath::Max(XDefines[Child->XIndex].Lenght, Child->GetWidth());

		if (YDefines.size() > 0 && YDefines[Child->YIndex].IsAuto)
			CopyYDefines[Child->YIndex].Lenght = FMath::Max(YDefines[Child->YIndex].Lenght, Child->GetHeight());
	}

	vector<uint> XLenghts;
	GetLenghts(&XLenghts, CopyXDefines, Area.Size.X);

	vector<uint> YLenghts;
	GetLenghts(&YLenghts, CopyYDefines, Area.Size.Y);

	for (auto& Child : Children)
	{
		intrect Rectangle = GetRenderingArea();

		if (XLenghts.size() > 0)
		{
			Rectangle.Position.X += accumulate(XLenghts.begin(), XLenghts.begin() + Child->XIndex, 0);
			Rectangle.Size.X = XLenghts[Child->XIndex];
		}
		if (YLenghts.size() > 0)
		{
			Rectangle.Position.Y += accumulate(YLenghts.begin(), YLenghts.begin() + Child->YIndex, 0);
			Rectangle.Size.Y = YLenghts[Child->YIndex];
		}

		Child->SetArea(Rectangle);
	}
}