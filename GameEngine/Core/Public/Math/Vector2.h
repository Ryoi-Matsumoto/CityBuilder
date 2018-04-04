#pragma once
#include "Function.h"

template<typename T>
struct SVector2
{
	union
	{
		struct
		{
			T X;
			T Y;
		};
		T V[2];
	};

	//======================================================================
	// コンストラクタ 
	//======================================================================

	SVector2()
	{
	}

	SVector2(T X, T Y) : X(X), Y(Y)
	{
	}

	SVector2(T S) : X(S), Y(S)
	{
	}

	//======================================================================
	// 変換
	//======================================================================

	template<typename TC>
	SVector2<TC> Convert() const
	{
		return SVector2<TC>((TC)X, (TC)Y);
	}

	SVector2<unsigned int> ToUint() const
	{
		return Convert<unsigned int>();
	}

	SVector2<int> ToInt() const
	{
		return Convert<int>();
	}

	SVector2<float> ToVector() const
	{
		return Convert<float>();
	}

	//======================================================================
	// 数学
	//======================================================================

	T Lenght() const
	{
		return FMath::Sqrt(X * X + Y * Y);
	}
	
	static T Dot(const SVector2<T>& v0, const SVector2<T>& v1)
	{
		return v0.X * v1.X + v0.Y * v1.Y;
	}

	T Dot(const SVector2<T>& v) const
	{
		return X * v.X + Y * v.Y;
	}

	static SVector2<T>& Normalizetion(const SVector2<T>& v)
	{
		T r = FMath::Sqrt(v.X * v.X + v.Y * v.Y);
		return SVector2(v.X / r, v.Y / r);
	}

	void Normalize()
	{
		T r = FMath::Sqrt(X * X + Y * Y);
		X = X / r;
		Y = Y / r;
	}

	SVector2<T> Normalized() const
	{
		T r = FMath::Sqrt(X * X + Y * Y);
		return SVector2(X / r, Y / r);
	}
	
	static T Angle(const SVector2<T>& v0, const SVector2<T>& v1)
	{
		return FMath::Acos(Dot(v0, v1) / (v0.Lenght() * v1.Lenght()));
	}

	// 時計回りの方の垂直なベクトル
	SVector2<T> Orthogonal() const
	{
		return SVector2(-X, Y);
	}

	T Atan() const
	{
		return atan2(X, Y);
	}

	//======================================================================
	// 曲線
	//======================================================================

	static SVector2<T> BezierCurve2D(T t, SVector2<T> p0, SVector2<T> p1, SVector2<T> p2)
	{
		SVector2<T> result;
		result.X = (1 - t) * (1 - t) * p0.X
			 + 2 * (1 - t) * t * p1.X
			 + t * t * p2.X;
		result.Y = (1 - t) * (1 - t) * p0.Y
			 + 2 * (1 - t) * t * p1.Y
			 + t * t * p2.Y;
		return result;
	}

 	static SVector2<T> BezierCurve3D(T t, SVector2<T> p0, SVector2<T> p1, SVector2<T> p2, SVector2<T> p3)
	{
		SVector2<T> result;
		result.X = (1 - t) * (1 - t) * (1 - t) * p0.X
			 + 3 * (1 - t) * (1 - t) * t * p1.X
			 + 3 * (1 - t) * t * t * p2.X
			 + t * t * t * p3.X;
		result.Y = (1 - t) * (1 - t) * (1 - t) * p0.Y
			 + 3 * (1 - t) * (1 - t) * t * p1.Y
			 + 3 * (1 - t) * t * t * p2.Y
			 + t * t * t * p3.Y;
		return result;
	}

	static SVector2<T> BezierCurve4D(T t, SVector2<T> p0, SVector2<T> p1, SVector2<T> p2, SVector2<T> p3, SVector2<T> p4)
	{
		SVector2<T> result;
		result.X = (1 - t) * (1 - t) * (1 - t) * (1 - t) * p0.X
			 + 4 * (1 - t) * (1 - t) * (1 - t) * t * p1.X
			 + 6 * (1 - t) * (1 - t) * t * t * p2.X
			 + 4 * (1 - t) * t * t * t * p3.X
			 + t * t * t * p4.X;
		result.Y = (1 - t) * (1 - t) * (1 - t) * (1 - t) * p0.Y
			 + 4 * (1 - t) * (1 - t) * (1 - t) * t * p1.Y
			 + 6 * (1 - t) * (1 - t) * t * t * p2.Y
			 + 4 * (1 - t) * t * t * t * p3.Y
			 + t * t * t * p4.Y;
		return result;
	}

	//======================================================================
	// 演算子
	//======================================================================

	SVector2<T>& operator += (const SVector2<T>& v)
	{
		X += v.X;
		Y += v.Y;
		return *this;
	}

	SVector2<T>& operator += (T s)
	{
		X += s;
		Y += s;
		return *this;
	}

	SVector2<T>& operator -= (const SVector2<T>& v)
	{
		X -= v.X;
		Y -= v.Y;
		return *this;
	}

	SVector2<T>& operator -= (T s)
	{
		X -= s;
		Y -= s;
		return *this;
	}

	SVector2<T>& operator *= (const SVector2<T>& v)
	{
		X *= v.X;
		Y *= v.Y;
		return *this;
	}

	SVector2<T>& operator *= (T s)
	{
		X *= s;
		Y *= s;
		return *this;
	}

	SVector2<T>& operator /= (const SVector2<T>& v)
	{
		X /= v.X;
		Y /= v.Y;
		return *this;
	}

	SVector2<T>& operator /= (T s)
	{
		X /= s;
		Y /= s;
		return *this;
	}

	SVector2<T> operator + (const SVector2<T>& v) const
	{
		return SVector2(X + v.X, Y + v.Y);
	}

	SVector2<T> operator + (T s) const
	{
		return SVector2(X + s, Y + s);
	}

	SVector2<T> operator - (const SVector2<T>& v) const
	{
		return SVector2(X - v.X, Y - v.Y);
	}

	SVector2<T> operator - (T s) const
	{
		return SVector2(X - s, Y - s);
	}

	SVector2<T> operator - () const
	{
		return SVector2(-X, -Y);
	}

	SVector2<T> operator * (const SVector2<T>& v) const
	{
		return SVector2(X * v.X, Y * v.Y);
	}

	SVector2<T> operator * (T s) const
	{
		return SVector2(X * s, Y * s);
	}

	SVector2<T> operator / (const SVector2<T>& v) const
	{
		return SVector2(X / v.X, Y / v.Y);
	}

	SVector2<T> operator / (T s) const
	{
		return SVector2(X / s, Y / s);
	}

	bool operator == (const SVector2<T>& v) const
	{
		return X == v.X && Y == v.Y;
	}

	bool operator != (const SVector2<T>& v) const
	{
		return X != v.X && Y != v.Y;
	}
};