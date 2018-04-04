#pragma once
#include "Core\Math\Public\All.h"
#include "Core\System\Public\Template.h"
#define database struct
#define array struct

database first
{
	uint* vertexes;
	float3* indexes;
};

database second
{
	array street
	{
		uint start;
		uint end;	

		SLine CentorsOfRoadway[2];
		float RoadwayWidth;
		float SidewalkWidths[2];
	};

	array intersection
	{
		street* streets;
	};	
};

database third
{
	array street
	{
		uint ConnectedIndex;

		SLine SideLines[2];
		SLine BoundaryLines[2];
		float SidewalkWidths[2];
		Vector2 Direction;

	};


	array crosswalk
	{
		FDesignIntersection* Intersection;
		SConnectedStreet* Street;
		FDesignCorner* Corners[2];
	};

};