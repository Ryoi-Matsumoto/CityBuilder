#pragma once
#include "Common.h"
#include "Object\Street.h"
#include "Object\Intersection.h"
#include "Object\Corner.h"
#include "Object\Crosswalk.h"

class FWorld
{
private:
	vector<FDesignStreet*> Streets;
	vector<FDesignIntersection*> Intersections;
	vector<FDetail*> Details;

public:
	void Render() { for (auto Detail : Details)Detail->Render(); }

	// Adder
	void AddStreet(FDesignStreet* Street) { Streets.push_back(Street); }
	void AddIntersection(FDesignIntersection* Intersection) { Intersections.push_back(Intersection); }
	void AddDetail(FDetail* Detail) { Details.push_back(Detail); }

	// Getter
	const vector<FDesignStreet*> GetStreets() const { return Streets; }
	const vector<FDesignIntersection*> GetIntersections() const { return Intersections; }
};