#include "../../../Public/Control/Container/UIGrid.h"
#include <numeric>

void GetLenghts(vector<uint>* Out, vector<SUILenght> const& Defines, uint Size)
{
	float TotalNormal = 0;
	float TotalRatio = 0;

	for (auto Define : Defines)
	{
		if (Define.IsRatio)TotalRatio += Define.Lenght;
		else TotalNormal +=  TotalNormal += Define.Lenght;
	}

	float Margin = FMath::Max(Size - TotalNormal, 0);

	for (auto Define : Defines)
	{
		if (Define.IsRatio)Out->push_back(Margin * Define.Lenght / TotalRatio);
		else Out->push_back(Define.Lenght);
	}
}

void FUIGrid::OnAreaChanged()
{
	auto XDefines = _XDefines;
	auto YDefines = _YDefines;
	for (auto Content : Contents)
	{
		if (XDefines.size() > 0 && XDefines[Content->_XIndex].IsAuto)
		{
			XDefines[Content->_XIndex].Lenght = FMath::Max(XDefines[Content->_XIndex].Lenght, Content->GetWidth());
		}
		if (YDefines.size() > 0 && YDefines[Content->_YIndex].IsAuto)
		{
			YDefines[Content->_YIndex].Lenght = FMath::Max(YDefines[Content->_YIndex].Lenght, Content->GetHeight());
		}
	}

	vector<uint> XLenghts;
	GetLenghts(&XLenghts, XDefines, Area.Size.X);

	vector<uint> YLenghts;
	GetLenghts(&YLenghts, YDefines, Area.Size.Y);

	for (auto Content : Contents)
	{
		intrect Rectangle;
		Rectangle.Position = Area.Position;
		Rectangle.Size = Area.Size;

		if (XLenghts.size() > 0)
		{
			Rectangle.Position.X += accumulate(XLenghts.begin(), XLenghts.begin() + Content->_XIndex, 0);
			Rectangle.Size.X = XLenghts[Content->_XIndex];
		}
		if (YLenghts.size() > 0)
		{
			Rectangle.Position.Y += accumulate(YLenghts.begin(), YLenghts.begin() + Content->_YIndex, 0);
			Rectangle.Size.Y = YLenghts[Content->_YIndex];
		}

		Content->SetArea(Rectangle);
	}
}