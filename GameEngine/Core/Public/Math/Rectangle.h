#pragma once
#include "Vector2.h"

template<typename T>
struct SRectangle
{
	SVector2<T> Position;
	SVector2<T> Size;

	SRectangle()
	{
	}

	SRectangle(SVector2<T> const& Position, SVector2<T> const& Size)
		: Position(Position)
		, Size(Size)
	{
	}

	SVector2<T> GetBottomRight()
	{
		return Position + Size;
	}

	void Move(SVector2<T> Amount)
	{
		Position += Amount;
	}

	SRectangle<T> Moved(SVector2<T> const& Amount)
	{
		return SRectangle<T>(Position + Amount, Size);
	}

	bool IsRange(SVector2<T> const& Point)
	{
		return Position.X <= Point.X && Position.Y <= Point.Y
			&& (Position.X + Size.X) > Point.X && (Position.Y + Size.Y) > Point.Y;
	}
};