#pragma once
#include "Core\System\BasicLibrary.h"
#include "Core\System\Math.h"

struct SPairedCurve
{
	// 0 : 外側
	// 1 : 内側
	vector<array<float3, 2>> Pairs;
};

\*
struct SRoundedArea
{
	vector<vector<float3>> Rounds;
};
*\
// 任意の1点と方向ベクトルによって定義される直線
struct SLine
{
	SLine()
	{
	}

	SLine(Vector2 OnePoint, Vector2 direction)
		:OnePoint(OnePoint), Direction(direction)
	{
	}

	static SLine TwoPoints(Vector2 point0, Vector2 point1)
	{
		return SLine(point0, point0 - point1);
	}

	static Vector2 CrossPoint(SLine line0, SLine line1)
	{
		float a0 = line0.Direction.X / line0.Direction.Y;
		float a1 = line1.Direction.X / line0.Direction.Y;
		Vector2 result;
		result.X = (a0 * line0.OnePoint.X - line0.OnePoint.Y + a1 * line1.OnePoint.X - line0.OnePoint.Y) / (a0 - a1);
		result.Y = (a0 * a1 * (line0.OnePoint.X - line1.OnePoint.X) - a1 * line1.OnePoint.Y + a0 * line1.OnePoint.Y) / (a0 - a1);
		return result;
	}

	Vector2 CrossedToCircle(Vector2 Centor, float radius) const
	{
		// todo
		return Vector2();
	}

	Vector2 OnePoint;
	Vector2 Direction;
};

template<typename T>
struct SQuadrangle
{
	SQuadrangle()
	{
	}

	SQuadrangle(T FL, T FR, T BL, T BR)
		:FrontLeft(FL), FrontRight(FR), BackRight(BR), BackLeft(BL)
	{
	}

	union
	{
		// 前左→前右→後右→後左
		T Points[4];

		struct
		{
			T FrontLeft;
			T FrontRight;
			T BackRight;
			T BackLeft;
		};
	};
};

using SQuadrangle3 = SQuadrangle<float3>;
using SQuadrangleIndex = SQuadrangle<uint>;

struct SQuadrangle2 : SQuadrangle<Vector2>
{
	SQuadrangle2()
	{
	}

	SQuadrangle2(Vector2 FL, Vector2 FR, Vector2 BL, Vector2 BR)
		:SQuadrangle(FL, FR, BR, BL)
	{
	}

	static SQuadrangle2 TwoPointsAndWidth(Vector2 point0, Vector2 point1, float Width)
	{
		SQuadrangle2 result;
		result.Points[0] = point0;
		result.Points[1] = point1;
		auto direction = (point0 - point1).Normalized();
		auto verDirection = direction.Orthogonal();
		result.Points[3] = point0 + verDirection * Width;
		result.Points[2] = point1 + verDirection * Width;
		return result;
	}

	SQuadrangle3 ToRectangle3(float y)
	{
		SQuadrangle3 result;
		for (int i = 0; i < 4; i++)
		{
			result.Points[i] = float3(Points[i].X, y, Points[i].Y);
		}
		return result;
	}
};