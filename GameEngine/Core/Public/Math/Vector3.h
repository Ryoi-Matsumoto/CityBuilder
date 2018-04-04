#pragma once
#include "Matrix.h"
#include "Vector2.h"
#include "Function.h"

struct SMatrix;

template<typename T>
struct SVector3
{
	union
	{
		struct
		{
			T X;
			T Y;
			T Z;
		};

		struct
		{
			T R;
			T G;
			T B;
		};

		T V[3];
	};

	//======================================================================
	// コンストラクタ
	//======================================================================

	SVector3()
	{
	}

	SVector3(SVector2<T> XZ, float Z) : X(XZ.X), Y(XZ.Y), Z(Z)
	{
	}

	SVector3(T X, T Y, T Z) : X(X), Y(Y), Z(Z)
	{
	}

	SVector3(T S) : X(S), Y(S), Z(S)
	{
	}

	//======================================================================
	// 変換
	//======================================================================

	template<typename TC>
	SVector3<TC> Convert() const
	{
		return SVector3<TC>((TC)X, (TC)Y, (TC)Z);
	}

	SVector3<unsigned int> ToUint() const
	{
		return Convert<unsigned int>();
	}

	SVector3<int> ToInt() const
	{
		return Convert<int>();
	}

	SVector3<float> ToVector() const
	{
		return Convert<float>();
	}
	
	//======================================================================
	// 数学
	//======================================================================

	T Lenght()
	{
		return FMath::Sqrt(X * X + Y * Y + Z * Z);;
	}

	void Normalize()
	{
		T r = FMath::Sqrt(X * X + Y * Y + Z * Z);
		X = X / r;
		Y = Y / r;
		Z = Z / r;
	}

	void Transform(const SMatrix& m)
	{
		Transformation(this, *this, m);
	}

	void RotateX(T Angle)
	{
		RotationX(this, *this, Angle);
	}

	void RotateY(T Angle)
	{
		RotationY(this, *this, Angle);
	}

	void RotateZ(T Angle)
	{
		RotationZ(this, *this, Angle);
	}

	T Sum() const
	{
		return X + Y + Z;
	}

	T Dot(const SVector3<T>& v) const
	{
		return X * v.X + Y * v.Y + Z * v.Z;
	}

	SVector3<T>& Cross(const SVector3<T>& v) const
	{
		return SVector3<T>
		(
			Y * v.Z - Z * v.Y,
			Z * v.X - X * v.Z,
			X * v.Y - Y * v.X
		);
	}

	SVector3<T> CrossUnitX()
	{
		return SVector3(0, Z, Y);
	}

	SVector3<T> CrossUnitY()
	{
		return SVector3(Z, 0, X);
	}

	SVector3<T> CrossUnitZ()
	{
		return SVector3(Y, X, 0);
	}

	SVector3<T> Normalized() const
	{
		T r = FMath::Sqrt(X * X + Y * Y + Z * Z);
		return SVector3(X / r, Y / r, Z / r);
	}

	SVector3<T> Transformed(const SMatrix& m) const
	{
		SVector3<T> result;
		Transformation(&result, *this, m);
		return result;
	}

	SVector3<T> RotatedX(T Angle) const
	{
		SVector3<T> result;
		RotationX(&result, *this, Angle);
		return result;
	}

	SVector3<T> RotatedY(T Angle) const
	{
		SVector3<T> result;
		RotationX(&result, *this, Angle);
		return result;
	}

	SVector3<T> RotatedZ(T Angle) const
	{
		SVector3<T> result;
		RotationZ(&result, *this, Angle);
		return result;
	}

	//======================================================================
	// 静的メンバー
	//======================================================================

	static SVector3<T> FromRGBwchar_t(char R, char G, char B)
	{
		return SVector3<T>((T)R / 255, (T)G / 255, (T)B / 255);
	}

	static T Dot(const SVector3<T>& v0, const SVector3<T>& v1)
	{
		return v0.X * v1.X + v0.Y * v1.Y + v0.Z * v1.Z;
	}

	static SVector3<T> Cross(const SVector3<T>& v0, const SVector3<T>& v1)
	{
		return SVector3<T>
		(
			v0.Y * v1.Z - v0.Z * v1.Y,
			v0.Z * v1.X - v0.X * v1.Z,
			v0.X * v1.Y - v0.Y * v1.X
		);
	}

	static SVector3<T> Normalizetion(const SVector3<T>& v)
	{
		T r = FMath::Sqrt(v.X * v.X + v.Y * v.Y + v.Z * v.Z);
		return SVector3(v.X / r, v.Y / r, v.Z / r);
	}

	static void Transformation(SVector3<T>* Out, const SVector3<T>& v, const SMatrix& m)
	{
		Out->X = v.X * m.M[0][0] + v.Y * m.M[1][0] + v.Z * m.M[2][0] + m.M[3][0];
		Out->Y = v.X * m.M[0][1] + v.Y * m.M[1][1] + v.Z * m.M[2][1] + m.M[3][1];
		Out->Z = v.X * m.M[0][2] + v.Y * m.M[1][2] + v.Z * m.M[2][2] + m.M[3][2];
	}

	static void RotationX(SVector3<T>* Out, const SVector3<T>& v, T Angle)
	{
		T sin = sinf(Angle), cos = cosf(Angle);

		Out->X = v.X;
		Out->Y = v.Z * sin + v.Y * cos;
		Out->Z = v.Z * cos - v.Y * sin;
	}

	static void RotationY(SVector3<T>* Out, const SVector3<T>& v, T Angle)
	{
		T sin = sinf(Angle), cos = cosf(Angle);

		Out->X = v.X * cos - v.Z * sin;
		Out->Y = v.Y;
		Out->Z = v.X * sin + v.Z * cos;
	}

	static void RotationZ(SVector3<T>* Out, const SVector3<T>& v, T Angle)
	{
		T sin = sinf(Angle), cos = cosf(Angle);

		Out->X = v.X * cos - v.Y * sin;
		Out->Y = v.X * sin + v.Y * cos;
		Out->Z = v.Z;
	}

	//======================================================================
	// 演算子
	//======================================================================

	const SVector3<T>& operator += (const SVector3<T>& v)
	{
		X += v.X;
		Y += v.Y;
		Z += v.Z;
		return *this;
	}

	const SVector3<T>& operator += (T s)
	{
		X += s;
		Y += s;
		Z += s;
		return *this;
	}

	const SVector3<T>& operator -= (const SVector3<T>& v)
	{
		X -= v.X;
		Y -= v.Y;
		Z -= v.Z;
		return *this;
	}

	const SVector3<T>& operator -= (T s)
	{
		X -= s;
		Y -= s;
		Z -= s;
		return *this;
	}

	const SVector3<T>& operator *= (const SVector3<T>& v)
	{
		X *= v.X;
		Y *= v.Y;
		Z *= v.Z;
		return *this;
	}

	const SVector3<T>& operator *= (T s)
	{
		X *= s;
		Y *= s;
		Z *= s;
		return *this;
	}

	const SVector3<T>& operator /= (const SVector3<T>& v)
	{
		X /= v.X;
		Y /= v.Y;
		Z /= v.Z;
		return *this;
	}

	const SVector3<T>& operator /= (T s)
	{
		X /= s;
		Y /= s;
		Z /= s;
		return *this;
	}

	SVector3<T> operator + () const
	{
		return SVector3(*this);
	}

	SVector3<T> operator + (const SVector3<T>& v) const
	{
		return SVector3(X + v.X, Y + v.Y, Z + v.Z);
	}

	SVector3<T> operator + (T s) const
	{
		return SVector3(X + s, Y + s, Z + s);
	}

	SVector3<T> operator - () const
	{
		return SVector3(-X, -Y, -Z);
	}

	SVector3<T> operator - (const SVector3<T>& v) const
	{
		return SVector3(X - v.X, Y - v.Y, Z - v.Z);
	}

	SVector3<T> operator - (T s) const
	{
		return SVector3(X - s, Y - s, Z - s);
	}

	SVector3<T> operator * (const SVector3<T>& v) const
	{
		return SVector3(X * v.X, Y * v.Y, Z * v.Z);
	}

	SVector3<T> operator * (T s) const
	{
		return SVector3(X * s, Y * s, Z * s);
	}

	SVector3<T> operator / (const SVector3<T>& v) const
	{
		return SVector3(X / v.X, Y / v.Y, Z / v.Z);
	}

	SVector3<T> operator / (T s) const
	{
		return SVector3(X / s, Y / s, Z / s);
	}

	bool operator == (const SVector3<T>& v) const
	{
		return X == v.X && Y == v.Y && Z == v.Z;
	}

	bool operator != (const SVector3<T>& v) const
	{
		return X != v.X && Y != v.Y && Z != v.Z;
	}
};