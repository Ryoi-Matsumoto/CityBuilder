#pragma once
#include "Vector3.h"

template<typename T>
struct SVector3;

template<typename T>
struct SVector4
{
	union
	{
		struct
		{
			SVector3<T> XYZ;
			T W;
		};

		struct
		{
			T X;
			T Y;
			T Z;
			T W;
		};

		struct
		{
			T R;
			T G;
			T B;
			T A;
		};

		T V[4];
	};
	
	//======================================================================
	// コンストラクタ
	//======================================================================

	SVector4()
	{
	}

	SVector4(const SVector3<T>& XYZ, T W) : X(XYZ.X), Y(XYZ.Y), Z(XYZ.Z), W(W)
	{
	}

	SVector4(const SVector2<T>& XY, T Z, T W) : X(XY.X), Y(XY.Y), Z(Z), W(W)
	{
	}

	SVector4(T X, T Y, T Z, T W) : X(X), Y(Y), Z(Z), W(W)
	{
	}

	SVector4(T S) : X(S), Y(S), Z(S), W(S)
	{
	}

	//======================================================================
	// 数学
	//======================================================================

	T Sum() const
	{
		return X + Y + Z + A;
	}
	
	//======================================================================
	// 静的メンバー
	//======================================================================

	static SVector4<T> FromRGBwchar_t(char R, char G, char B)
	{
		return SVector4<T>((T)R / 255, (T)G / 255, (T)B / 255, 1.0f);
	}

	static SVector4<T> FromRGBAwchar_t(char R, char G, char B, char A)
	{
		return SVector4<T>((T)R / 255, (T)G / 255, (T)B / 255, (A) / 255);
	}

	//======================================================================
	// 演算子
	//======================================================================

	const SVector4<T>& operator += (const SVector4<T>& v)
	{
		X += v.X;
		Y += v.Y;
		Z += v.Z;
		W += v.W;
		return *this;
	}

	const SVector4<T>& operator += (T s)
	{
		X += s;
		Y += s;
		Z += s;
		W += s;
		return *this;
	}

	const SVector4<T>& operator -= (const SVector4<T>& v)
	{
		X -= v.X;
		Y -= v.Y;
		Z -= v.Z;
		W -= v.W;
		return *this;
	}

	const SVector4<T>& operator -= (T s)
	{
		X -= s;
		Y -= s;
		Z -= s;
		W -= s;
		return *this;
	}

	const SVector4<T>& operator *= (const SVector4<T>& v)
	{
		X *= v.X;
		Y *= v.Y;
		Z *= v.Z;
		W *= v.W; 
		return *this;
	}

	const SVector4<T>& operator *= (T s)
	{
		X *= s;
		Y *= s;
		Z *= s;
		W *= s;
		return *this;
	}

	const SVector4<T>& operator /= (const SVector4<T>& v)
	{
		X /= v.X;
		Y /= v.Y;
		Z /= v.Z;
		W /= v.W;
		return *this;
	}

	const SVector4<T>& operator /= (T s)
	{
		X /= s;
		Y /= s;
		Z /= s;
		W /= s;
		return *this;
	}

	SVector4<T> operator + () const
	{
		return SVector4(*this);
	}

	SVector4<T> operator + (const SVector4<T>& v) const
	{
		return SVector4(X + v.X, Y + v.Y, Z + v.Z, W + v.W);
	}

	SVector4<T> operator + (T s) const
	{
		return SVector4(X + s, Y + s, Z + s, W + s);
	}

	SVector4<T> operator - () const
	{
		return SVector4(-X, -Y, -Z, -W);
	}

	SVector4<T> operator - (const SVector4<T>& v) const
	{
		return SVector4(X - v.X, Y - v.Y, Z - v.Z, W - v.W);
	}

	SVector4<T> operator - (T s) const
	{
		return SVector4(X - s, Y - s, Z - s, W - s);
	}

	SVector4<T> operator * (const SVector4<T>& v) const
	{
		return SVector4(X * v.X, Y * v.Y, Z * v.Z, W * v.W);
	}

	SVector4<T> operator * (T s) const
	{
		return SVector4(X * s, Y * s, Z * s, W * s);
	}

	SVector4<T> operator / (const SVector4<T>& v) const
	{
		return SVector4(X / v.X, Y / v.Y, Z / v.Z, W / v.W);
	}

	SVector4<T> operator / (T s) const
	{
		return SVector4(X / s, Y / s, Z / s, W / s);
	}

	bool operator == (const SVector4<T>& v) const
	{
		return X == v.X && Y == v.Y && Z == v.Z && W == v.W;
	}

	bool operator != (const SVector4<T>& v) const
	{
		return X != v.X && Y != v.Y && Z != v.Z && W != v.W;
	}
};