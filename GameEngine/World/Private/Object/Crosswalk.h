#pragma once
#include "Common.h"

class SConnectedStreet;
class FDesignCorner;
class FDesignIntersection;

//======================================================================
// デザイン
//======================================================================

struct SCrosswalkUnique
{
	const FDesignIntersection* Intersection;
	const SConnectedStreet* Street;
	const FDesignCorner* Corners[2];
};

struct SCrosswalkParameter : SPropertySet
{
	void Update(const SCrosswalkUnique& Unique);

	int AlignStreetIndex;
	float Width;
	float Shift;
};	
	
struct SCrosswalkTemp
{
	void Update(const SCrosswalkUnique& Unique, const SCrosswalkParameter& Parameter);

	SQuadrangle2 Rectangle;
	SLine FrontSide;
	SLine BackSide;
};

class FDesignCrosswalk : public FDesign<SCrosswalkUnique, SCrosswalkParameter, SCrosswalkTemp>
{    
public:
	FDesignCrosswalk(FDesignIntersection* Intersection, const SConnectedStreet* Street,
		const FDesignCorner* LeftCorner, const FDesignCorner* RightCorner)
	{
		Unique.Intersection = Intersection;
		Unique.Street = Street;
		Unique.Corners[0] = LeftCorner;
		Unique.Corners[1] = RightCorner;
	}

	SQuadrangle2 GetRectangle() const { return Temp.Rectangle; }
	SLine GetFrontSide() const { return Temp.FrontSide; }
	SLine GetBackSide() const { return Temp.BackSide; }
};

//======================================================================
// ディティール
//======================================================================

class FDetailCrosswalk : public FDetail
{
private:
	const FDesignCrosswalk* Crosswalk;
	const FDesignIntersection* Intersection;

public:
	FDetailCrosswalk::FDetailCrosswalk(const FDesignCrosswalk* Crosswalk, const FDesignIntersection* Intersection)
		:Crosswalk(Crosswalk), Intersection(Intersection)
	{
		Crosswalk->AddDetail(this);
	}

	FStartupShape* CreateShape() override;
};