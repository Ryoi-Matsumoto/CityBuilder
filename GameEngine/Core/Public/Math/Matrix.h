#pragma once
#include "Vector3.h"

template<typename T>
struct SVector3;

using CRVector3 = SVector3<float> const&;

struct SMatrix
{
	union
	{
		struct
		{
			float M11, M12, M13, M14;
			float M21, M22, M23, M24;
			float M31, M32, M33, M34;
			float M41, M42, M43, M44;
		};
		float M[4][4];
		float Array[16];
	};
		
	//======================================================================
	// コンストラクタ
	//======================================================================

	SMatrix()
	{
	}

	//======================================================================
	// 演算子
	//======================================================================
	
	SMatrix operator *= (const SMatrix& Source)
	{
		Multiply(Source);
		return *this;
	}

	SMatrix operator * (const SMatrix& Source) const
	{
		return Multiplied(Source);
	}

	//======================================================================
	// 能動型メンバー
	//======================================================================

	void Multiply(const SMatrix& m)
	{
		Multiplication(this, *this, m);
	}

	void Translate(CRVector3 v)
	{
		Translation(this, v);
	}

	void Inverse()
	{
		Inversion(this, *this);
	}

	void Transpose()
	{
		Transposion(this, *this);
	}

	void RotateX(float Angle)
	{
		RotationX(this, Angle);
	}

	void RotateY(float Angle)
	{
		RotationY(this, Angle);
	}

	void RotateZ(float Angle)
	{
		RotationZ(this, Angle);
	}

	void RotateXYZ(float r0, float r1, float r2)
	{
		RotationXYZ(this, r0, r1, r2);
	}

	void RotateXZY(float r0, float r1, float r2)
	{
		RotationXZY(this, r0, r1, r2);
	}

	void RotateYXZ(float r0, float r1, float r2)
	{
		RotationYXZ(this, r0, r1, r2);
	}

	void RotateYZX(float r0, float r1, float r2)
	{
		RotationYZX(this, r0, r1, r2);
	}

	void RotateZXY(float r0, float r1, float r2)
	{
		RotationZXY(this, r0, r1, r2);
	}

	void RotateZYX(float r0, float r1, float r2)
	{
		RotationZYX(this, r0, r1, r2);
	}
	
	void Scale(CRVector3 v)
	{
		Scaling(this, v);
	}

	void Scale(float s)
	{
		Scaling(this, s);
	}

	//======================================================================
	// 加工型メンバー
	//======================================================================

	SMatrix Multiplied(SMatrix const& m) const
	{
		SMatrix Result;
		Multiplication(&Result, *this, m);
		return Result;
	}

	SMatrix Translated(CRVector3 v) const
	{
		SMatrix Result;
		Translation(&Result, v);
		Multiplication(&Result, *this, Result);
		return Result;
	}

	SMatrix Inversed() const
	{
		SMatrix Result;
		Inversion(&Result, *this);
		return Result;
	}

	SMatrix Transposed() const
	{
		SMatrix Result;
		Transposion(&Result, *this);
		return Result;
	}

	SMatrix RotatedX(float Angle) const
	{
		SMatrix Result;
		RotationX(&Result, Angle);
		Multiplication(&Result, *this, Result);
		return Result;
	}

	SMatrix RotatedY(float Angle) const
	{
		SMatrix Result;
		RotationY(&Result, Angle);
		Multiplication(&Result, *this, Result);
		return Result;
	}
	
	SMatrix RotatedZ(float Angle) const
	{
		SMatrix Result;
		RotationZ(&Result, Angle);
		Multiplication(&Result, *this, Result);
		return Result;
	}

	SMatrix RotatedXYZ(float r0, float r1, float r2) const
	{
		SMatrix Result;
		RotationXYZ(&Result, r0, r1, r2);
		Multiplication(&Result, *this, Result);
		return Result;
	}

	SMatrix RotatedXZY(float r0, float r1, float r2) const
	{
		SMatrix Result;
		RotationXZY(&Result, r0, r1, r2);
		Multiplication(&Result, *this, Result);
		return Result;
	}

	SMatrix RotatedYXZ(float r0, float r1, float r2) const
	{
		SMatrix Result;
		RotationYXZ(&Result, r0, r1, r2);
		Multiplication(&Result, *this, Result);
		return Result;
	}

	SMatrix RotatedYZX(float r0, float r1, float r2) const
	{
		SMatrix Result;
		RotationYZX(&Result, r0, r1, r2);
		Multiplication(&Result, *this, Result);
		return Result;
	}

	SMatrix RotatedZXY(float r0, float r1, float r2) const
	{
		SMatrix Result;
		RotationZXY(&Result, r0, r1, r2);
		Multiplication(&Result, *this, Result);
		return Result;
	}

	SMatrix RotatedZYX(float r0, float r1, float r2) const
	{
		SMatrix Result;
		RotationZYX(&Result, r0, r1, r2);
		Multiplication(&Result, *this, Result);
		return Result;
	}

	SMatrix Scaled(CRVector3 v) const
	{
		SMatrix Result;
		Scaling(&Result, v);
		Multiplication(&Result, *this, Result);
		return Result;
	}

	SMatrix Scaled(float s) const
	{
		SMatrix Result;
		Scaling(&Result, s);
		Multiplication(&Result, *this, Result);
		return Result;
	}

	//======================================================================
	// 戻り値型静的メンバー
	//======================================================================

	static SMatrix Multiplication(const SMatrix& m0, const SMatrix& m1)
	{
		SMatrix Result;
		Multiplication(&Result, m0, m1);
		return Result;
	}

	static SMatrix Translation(CRVector3 v)
	{
		SMatrix Result;
		Translation(&Result, v);
		return Result;
	}

	static SMatrix RotationX(float Angle)
	{
		SMatrix Result;
		RotationX(&Result, Angle);
		return Result;
	}

	static SMatrix RotationY(float Angle)
	{
		SMatrix Result;
		RotationY(&Result, Angle);
		return Result;
	}

	static SMatrix RotationZ(float Angle)
	{
		SMatrix Result;
		RotationZ(&Result, Angle);
		return Result;
	}

	static SMatrix RotationAxis(CRVector3 Axis, float Angle)
	{
		SMatrix Result;
		RotationAxis(&Result, Axis, Angle);
		return Result;
	}

	static SMatrix Scaling(CRVector3 Scale)
	{
		SMatrix Result;
		Scaling(&Result, Scale);
		return Result;
	}

	static SMatrix Scaling(float Scale)
	{
		SMatrix Result;
		Scaling(&Result, Scale);
		return Result;
	}

	static SMatrix Inversion(const SMatrix& Source)
	{
		SMatrix Result;
		Inversion(&Result, Source);
		return Result;
	}

	static SMatrix Transposion(const SMatrix& Source)
	{
		SMatrix Result;
		Transposion(&Result, Source);
		return Result;
	}

	static SMatrix LookAt(CRVector3 Position, CRVector3 At, CRVector3 Up)
	{
		SMatrix Result;
		LookAt(&Result, Position, At, Up);
		return Result;
	}

	static SMatrix LookAtRH(CRVector3 Position, CRVector3 At, CRVector3 Up)
	{
		SMatrix Result;
		LookAtRH(&Result, Position, At, Up);
		return Result;
	}

	static SMatrix LookRotation(CRVector3 Eye, float RotationV, float RotationH, float RotationTilde)
	{
		SMatrix Result;
		LookRotation(&Result, Eye, RotationV, RotationH, RotationTilde);
		return Result;
	}

	static SMatrix LookRotationRH(CRVector3 Eye, float RotationV, float RotationH, float RotationTidle)
	{
		SMatrix Result;
		LookRotationRH(&Result, Eye, RotationV, RotationH, RotationTidle);
		return Result;
	}

	static SMatrix PerspectiveFov(float Fov, float Aspect, float Near, float Far)
	{
		SMatrix Result;
		PerspectiveFov(&Result, Fov, Aspect, Near, Far);
		return Result;
	}

	static SMatrix PerspectiveFovRH(float Fov, float Aspect, float Near, float Far)
	{
		SMatrix Result;
		PerspectiveFovRH(&Result, Fov, Aspect, Near, Far);
		return Result;
	}

	static SMatrix Ortho(float Width, float Height, float Near, float Far)
	{
		SMatrix Result;
		Ortho(&Result, Width, Height, Near, Far);
		return Result;
	}

	static SMatrix OrthoRH(float Width, float Height, float Near, float Far)
	{
		SMatrix Result;
		OrthoRH(&Result, Width, Height, Near, Far);
		return Result;
	}

	static SMatrix RotationXYZ(float RotationX, float RotationY, float RotationZ)
	{
		SMatrix Result;
		RotationXYZ(&Result, RotationX, RotationY, RotationZ);
		return Result;
	}

	static SMatrix RotationXZY(float RotationX, float RotationY, float RotationZ)
	{
		SMatrix Result;
		RotationXZY(&Result, RotationX, RotationY, RotationZ);
		return Result;
	}

	static SMatrix RotationYXZ(float RotationX, float RotationY, float RotationZ)
	{
		SMatrix Result;
		RotationYXZ(&Result, RotationX, RotationY, RotationZ);
		return Result;
	}

	static SMatrix RotationYZX(float RotationX, float RotationY, float RotationZ)
	{
		SMatrix Result;
		RotationYZX(&Result, RotationX, RotationY, RotationZ);
		return Result;
	}

	static SMatrix RotationZXY(float RotationX, float RotationY, float RotationZ)
	{
		SMatrix Result;
		RotationZXY(&Result, RotationX, RotationY, RotationZ);
		return Result;
	}

	static SMatrix RotationZYX(float RotationX, float RotationY, float RotationZ)
	{
		SMatrix Result;
		RotationZYX(&Result, RotationX, RotationY, RotationZ);
		return Result;
	}

	//======================================================================
	// 原型静的メンバー
	//======================================================================

	static void Multiplication(SMatrix* Out, const SMatrix& Left, const SMatrix& Right);

	static void Translation(SMatrix* Out, CRVector3 Vector);

	static void RotationX(SMatrix* Out, float Angle);

	static void RotationY(SMatrix* Out, float Angle);

	static void RotationZ(SMatrix* Out, float Angle);

	static void RotationAxis(SMatrix* Out, CRVector3 Axis, float Angle);
	
	static void Scaling(SMatrix* Out, CRVector3 Scale);
	
	static void Scaling(SMatrix* Out, float Scale);

	static void Inversion(SMatrix* Out, const SMatrix& Source);

	static void Transposion(SMatrix* Out, const SMatrix& Source);

	static void LookAt(SMatrix* Out, CRVector3 Position, CRVector3 At, CRVector3 Up);

	static void LookAtRH(SMatrix* Out, CRVector3 Position, CRVector3 At, CRVector3 Up);

	static void LookRotation(SMatrix* Out, CRVector3 Eye, float RotationVertical, float RotationHorizontal, float RotationTilde);

	static void LookRotationRH(SMatrix* Out, CRVector3 Eye, float RotationVertical, float RotationHorizontal, float RotationTilde);

	static void PerspectiveFov(SMatrix* Out, float Fov, float Aspect, float Near, float Far);
	
	static void PerspectiveFovRH(SMatrix* Out, float Fov, float Aspect, float Near, float Far);

	static void Ortho(SMatrix* Out, float Width, float Height, float Near, float Far);

	static void OrthoRH(SMatrix* Out, float Width, float Height, float Near, float Far);

	static void RotationXYZ(SMatrix* Out, float RotationX, float RotationY, float RotationZ);

	static void RotationXZY(SMatrix* Out, float RotationX, float RotationY, float RotationZ);
	
	static void RotationYXZ(SMatrix* Out, float RotationX, float RotationY, float RotationZ);

	static void RotationYZX(SMatrix* Out, float RotationX, float RotationY, float RotationZ);

	static void RotationZXY(SMatrix* Out, float RotationX, float RotationY, float RotationZ);
	
	static void RotationZYX(SMatrix* Out, float RotationX, float RotationY, float RotationZ);
};