#pragma once
#include "Common.h"
#include "World\World.h"

class SConnectedStreet;
class FDesignStreet;
class FDesignCorner;
class FDesignCrosswalk;
class FDetailCorner;
class FDetailCrosswalk;
class FDesignIntersection;
class FWorld;

//======================================================================
// ƒfƒUƒCƒ“
//======================================================================

struct SIntersectionUnique
{
	FDesignIntersection* Intersection;
	FWorld* World;
};

struct SIntersectionParameter : public SPropertySet
{
	void Update(const SIntersectionUnique& Unique);

	float Elevation;
	vector<const SConnectedStreet*> Streets;
};

struct SIntersectionTemp
{
	void Update(const SIntersectionUnique& Unique, const SIntersectionParameter& Parameter);

	vector<const FDesignCorner*> Corners;
	vector<const FDesignCrosswalk*> Crosswalks;	

	vector<const FDetailCorner*> DetailCorners;
	vector<const FDetailCrosswalk*> DetailCrosswalks;
};

class FDesignIntersection : public FDesign<SIntersectionUnique, SIntersectionParameter, SIntersectionTemp>
{	
public:
	FDesignIntersection(FWorld* World)
	{
		Unique.Intersection = this;
		Unique.World = World;
	}

	// Adder
	void AddStreet(const SConnectedStreet* ConnectedStreet);

	// Getter
	float GetElevation() const { return Parameter.Elevation; }	
	int GetNumStreet() const { return Parameter.Streets.size(); }
	const vector<const SConnectedStreet*> GetStreets() const { return Parameter.Streets; }
	const vector<const FDesignCorner*> GetCorners() const { return Temp.Corners; }
};