#pragma once
#include "Common.h"

class PhCollision
{
	static float RayTriangleIntersect
	(
		const Vector3& rayOrigin,	
		const Vector3& rayDelta, 						              
		const Vector3& p0, 
		const Vector3& p1, 
		const Vector3& p2, 
		float minDistance
	);



};