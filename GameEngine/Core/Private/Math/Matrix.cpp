#include "..\..\Public\Math\Matrix.h"
#include "..\..\Public\Math\Function.h"
#include <Windows.h>

void SMatrix::Multiplication(SMatrix* Out, const SMatrix& m0, const SMatrix& m1)
{
	Out->M[0][0] = m0.M[0][0] * m1.M[0][0] + m0.M[0][1] * m1.M[1][0] + m0.M[0][2] * m1.M[2][0] + m0.M[0][3] * m1.M[3][0];
	Out->M[0][1] = m0.M[0][0] * m1.M[0][1] + m0.M[0][1] * m1.M[1][1] + m0.M[0][2] * m1.M[2][1] + m0.M[0][3] * m1.M[3][1];
	Out->M[0][2] = m0.M[0][0] * m1.M[0][2] + m0.M[0][1] * m1.M[1][2] + m0.M[0][2] * m1.M[2][2] + m0.M[0][3] * m1.M[3][2];
	Out->M[0][3] = m0.M[0][0] * m1.M[0][3] + m0.M[0][1] * m1.M[1][3] + m0.M[0][2] * m1.M[2][3] + m0.M[0][3] * m1.M[3][3];

	Out->M[1][0] = m0.M[1][0] * m1.M[0][0] + m0.M[1][1] * m1.M[1][0] + m0.M[1][2] * m1.M[2][0] + m0.M[1][3] * m1.M[3][0];
	Out->M[1][1] = m0.M[1][0] * m1.M[0][1] + m0.M[1][1] * m1.M[1][1] + m0.M[1][2] * m1.M[2][1] + m0.M[1][3] * m1.M[3][1];
	Out->M[1][2] = m0.M[1][0] * m1.M[0][2] + m0.M[1][1] * m1.M[1][2] + m0.M[1][2] * m1.M[2][2] + m0.M[1][3] * m1.M[3][2];
	Out->M[1][3] = m0.M[1][0] * m1.M[0][3] + m0.M[1][1] * m1.M[1][3] + m0.M[1][2] * m1.M[2][3] + m0.M[1][3] * m1.M[3][3];

	Out->M[2][0] = m0.M[2][0] * m1.M[0][0] + m0.M[2][1] * m1.M[1][0] + m0.M[2][2] * m1.M[2][0] + m0.M[2][3] * m1.M[3][0];
	Out->M[2][1] = m0.M[2][0] * m1.M[0][1] + m0.M[2][1] * m1.M[1][1] + m0.M[2][2] * m1.M[2][1] + m0.M[2][3] * m1.M[3][1];
	Out->M[2][2] = m0.M[2][0] * m1.M[0][2] + m0.M[2][1] * m1.M[1][2] + m0.M[2][2] * m1.M[2][2] + m0.M[2][3] * m1.M[3][2];
	Out->M[2][3] = m0.M[2][0] * m1.M[0][3] + m0.M[2][1] * m1.M[1][3] + m0.M[2][2] * m1.M[2][3] + m0.M[2][3] * m1.M[3][3];

	Out->M[3][0] = m0.M[3][0] * m1.M[0][0] + m0.M[3][1] * m1.M[1][0] + m0.M[3][2] * m1.M[2][0] + m0.M[3][3] * m1.M[3][0];
	Out->M[3][1] = m0.M[3][0] * m1.M[0][1] + m0.M[3][1] * m1.M[1][1] + m0.M[3][2] * m1.M[2][1] + m0.M[3][3] * m1.M[3][1];
	Out->M[3][2] = m0.M[3][0] * m1.M[0][2] + m0.M[3][1] * m1.M[1][2] + m0.M[3][2] * m1.M[2][2] + m0.M[3][3] * m1.M[3][2];
	Out->M[3][3] = m0.M[3][0] * m1.M[0][3] + m0.M[3][1] * m1.M[1][3] + m0.M[3][2] * m1.M[2][3] + m0.M[3][3] * m1.M[3][3];	
}

void SMatrix::Translation(SMatrix* Out, CRVector3 v)
{
	ZeroMemory(Out, sizeof(SMatrix));
	Out->M[3][0] = v.X;
	Out->M[3][1] = v.Y;
	Out->M[3][2] = v.Z;
	Out->M[0][0] = 1.0f;
	Out->M[1][1] = 1.0f;
	Out->M[2][2] = 1.0f;		
	Out->M[3][3] = 1.0f;
}

void SMatrix::RotationX(SMatrix* Out, float Angle)
{
	float Sin = FMath::Sin(Angle), Cos = FMath::Cos(Angle);

	ZeroMemory(Out, sizeof(SMatrix));
	Out->M[0][0] = 1.0f;
	Out->M[1][1] = Cos;
	Out->M[1][2] = Sin;
	Out->M[2][1] = -Sin;
	Out->M[2][2] = Cos;
	Out->M[3][3] = 1.0f;	
}

void SMatrix::RotationY(SMatrix* Out, float Angle)
{
	float Sin = FMath::Sin(Angle), Cos = FMath::Cos(Angle);

	ZeroMemory(Out, sizeof(SMatrix));
	Out->M[0][0] = Cos;
	Out->M[0][2] = -Sin;
	Out->M[2][0] = Sin;
	Out->M[2][2] = Cos;
	Out->M[1][1] = 1.0f;
	Out->M[3][3] = 1.0f;	
}

void SMatrix::RotationZ(SMatrix* Out, float Angle)
{
	float Sin = FMath::Sin(Angle), Cos = FMath::Cos(Angle);

	ZeroMemory(Out, sizeof(SMatrix));
	Out->M[0][0] = Cos;
	Out->M[0][1] = Sin;
	Out->M[1][0] = -Sin;
	Out->M[1][1] = Cos;
	Out->M[2][2] = 1.0f;
	Out->M[3][3] = 1.0f;	
}

void SMatrix::RotationAxis(SMatrix* Out, CRVector3 Axis, float Angle)
{
	SVector3<float> xv(1.0f, 0.0f, 0.0f), yv(0.0f, 1.0f, 0.0f), zv(0.0f, 0.0f, 1.0f);
	SVector3<float> xv2, yv2(0.0f, 1.0f, 0.0f), zv2, xv3, yv3, zv3;
	float f;

	zv2 = Axis.Normalized();
	xv2 = SVector3<float>::Cross(zv2, yv2);
	yv2 = SVector3<float>::Cross(xv2, zv2).Normalized();
	xv2 = xv2.Normalized();

	xv3.X = xv2.X * xv.X + xv2.Y * xv.Y + xv2.Z * xv.Z;
	xv3.Y = yv2.X * xv.X + yv2.Y * xv.Y + yv2.Z * xv.Z;
	xv3.Z = zv2.X * xv.X + zv2.Y * xv.Y + zv2.Z * xv.Z;

	yv3.X = xv2.X * yv.X + xv2.Y * yv.Y + xv2.Z * yv.Z;
	yv3.Y = yv2.X * yv.X + yv2.Y * yv.Y + yv2.Z * yv.Z;
	yv3.Z = zv2.X * yv.X + zv2.Y * yv.Y + zv2.Z * yv.Z;

	zv3.X = xv2.X * zv.X + xv2.Y * zv.Y + xv2.Z * zv.Z;
	zv3.Y = yv2.X * zv.X + yv2.Y * zv.Y + yv2.Z * zv.Z;
	zv3.Z = zv2.X * zv.X + zv2.Y * zv.Y + zv2.Z * zv.Z;

	float Sin = FMath::Sin(Angle), Cos = FMath::Cos(Angle);

	f = xv3.X * Cos - xv3.Y * Sin;
	xv3.Y = xv3.X * Sin + xv3.Y * Cos;
	xv3.X = f;

	f = yv3.X * Cos - yv3.Y * Sin;
	yv3.Y = yv3.X * Sin + yv3.Y * Cos;
	yv3.X = f;

	f = zv3.X * Cos - zv3.Y * Sin;
	zv3.Y = zv3.X * Sin + zv3.Y * Cos;
	zv3.X = f;

	xv.X = xv2.X * xv3.X + yv2.X * xv3.Y + zv2.X * xv3.Z;
	xv.Y = xv2.Y * xv3.X + yv2.Y * xv3.Y + zv2.Y * xv3.Z;
	xv.Z = xv2.Z * xv3.X + yv2.Z * xv3.Y + zv2.Z * xv3.Z;

	yv.X = xv2.X * yv3.X + yv2.X * yv3.Y + zv2.X * yv3.Z;
	yv.Y = xv2.Y * yv3.X + yv2.Y * yv3.Y + zv2.Y * yv3.Z;
	yv.Z = xv2.Z * yv3.X + yv2.Z * yv3.Y + zv2.Z * yv3.Z;

	zv.X = xv2.X * zv3.X + yv2.X * zv3.Y + zv2.X * zv3.Z;
	zv.Y = xv2.Y * zv3.X + yv2.Y * zv3.Y + zv2.Y * zv3.Z;
	zv.Z = xv2.Z * zv3.X + yv2.Z * zv3.Y + zv2.Z * zv3.Z;

	Out->M[0][0] = xv.X;
	Out->M[1][0] = xv.Y;
	Out->M[2][0] = xv.Z;
	Out->M[3][0] = 0.0f;

	Out->M[0][1] = yv.X;
	Out->M[1][1] = yv.Y;
	Out->M[2][1] = yv.Z;
	Out->M[3][1] = 0.0f;

	Out->M[0][2] = zv.X;
	Out->M[1][2] = zv.Y;
	Out->M[2][2] = zv.Z;
	Out->M[3][2] = 0.0f;

	Out->M[0][3] = 0.0f;
	Out->M[1][3] = 0.0f;
	Out->M[2][3] = 0.0f;
	Out->M[3][3] = 1.0f;	
}

void SMatrix::Scaling(SMatrix* Out, CRVector3 Scale)
{
	ZeroMemory(Out, sizeof(SMatrix));
	Out->M[0][0] = Scale.X;
	Out->M[1][1] = Scale.Y;
	Out->M[2][2] = Scale.Z;
	Out->M[3][3] = 1.0f;	
}

void SMatrix::Scaling(SMatrix* Out, float Scale)
{
	ZeroMemory(Out, sizeof(SMatrix));
	Out->M[0][0] = Scale;
	Out->M[1][1] = Scale;
	Out->M[2][2] = Scale;
	Out->M[3][3] = 1.0f;	
}

void SMatrix::Inversion(SMatrix* Out, const SMatrix& Source)
{
	float detA =
		Source.M[0][0] * Source.M[1][1] * Source.M[2][2] * Source.M[3][3] +
		Source.M[0][0] * Source.M[1][2] * Source.M[2][3] * Source.M[3][1] +
		Source.M[0][0] * Source.M[1][3] * Source.M[2][1] * Source.M[3][2] +
		Source.M[0][1] * Source.M[1][0] * Source.M[2][3] * Source.M[3][2] +
		Source.M[0][1] * Source.M[1][2] * Source.M[2][0] * Source.M[3][3] +
		Source.M[0][1] * Source.M[1][3] * Source.M[2][2] * Source.M[3][0] +
		Source.M[0][2] * Source.M[1][0] * Source.M[2][1] * Source.M[3][3] +
		Source.M[0][2] * Source.M[1][1] * Source.M[2][3] * Source.M[3][0] +
		Source.M[0][2] * Source.M[1][3] * Source.M[2][0] * Source.M[3][1] +
		Source.M[0][3] * Source.M[1][0] * Source.M[2][2] * Source.M[3][1] +
		Source.M[0][3] * Source.M[1][1] * Source.M[2][0] * Source.M[3][2] +
		Source.M[0][3] * Source.M[1][2] * Source.M[2][1] * Source.M[3][0] -

		Source.M[0][0] * Source.M[1][1] * Source.M[2][3] * Source.M[3][2] -
		Source.M[0][0] * Source.M[1][2] * Source.M[2][1] * Source.M[3][3] -
		Source.M[0][0] * Source.M[1][3] * Source.M[2][2] * Source.M[3][1] -
		Source.M[0][1] * Source.M[1][0] * Source.M[2][2] * Source.M[3][3] -
		Source.M[0][1] * Source.M[1][2] * Source.M[2][3] * Source.M[3][0] -
		Source.M[0][1] * Source.M[1][3] * Source.M[2][0] * Source.M[3][2] -
		Source.M[0][2] * Source.M[1][0] * Source.M[2][3] * Source.M[3][1] -
		Source.M[0][2] * Source.M[1][1] * Source.M[2][0] * Source.M[3][3] -
		Source.M[0][2] * Source.M[1][3] * Source.M[2][1] * Source.M[3][0] -
		Source.M[0][3] * Source.M[1][0] * Source.M[2][1] * Source.M[3][2] -
		Source.M[0][3] * Source.M[1][1] * Source.M[2][2] * Source.M[3][0] -
		Source.M[0][3] * Source.M[1][2] * Source.M[2][0] * Source.M[3][1];

	Out->M[0][0] =
		(
			Source.M[1][1] * Source.M[2][2] * Source.M[3][3] +
			Source.M[1][2] * Source.M[2][3] * Source.M[3][1] +
			Source.M[1][3] * Source.M[2][1] * Source.M[3][2] -
			Source.M[1][1] * Source.M[2][3] * Source.M[3][2] -
			Source.M[1][2] * Source.M[2][1] * Source.M[3][3] -
			Source.M[1][3] * Source.M[2][2] * Source.M[3][1]
			) / detA;

	Out->M[0][1] =
		(
			Source.M[0][1] * Source.M[2][3] * Source.M[3][2] +
			Source.M[0][2] * Source.M[2][1] * Source.M[3][3] +
			Source.M[0][3] * Source.M[2][2] * Source.M[3][1] -
			Source.M[0][1] * Source.M[2][2] * Source.M[3][3] -
			Source.M[0][2] * Source.M[2][3] * Source.M[3][1] -
			Source.M[0][3] * Source.M[2][1] * Source.M[3][2]
			) / detA;

	Out->M[0][2] =
		(
			Source.M[0][1] * Source.M[1][2] * Source.M[3][3] +
			Source.M[0][2] * Source.M[1][3] * Source.M[3][1] +
			Source.M[0][3] * Source.M[1][1] * Source.M[3][2] -
			Source.M[0][1] * Source.M[1][3] * Source.M[3][2] -
			Source.M[0][2] * Source.M[1][1] * Source.M[3][3] -
			Source.M[0][3] * Source.M[1][2] * Source.M[3][1]
			) / detA;

	Out->M[0][3] =
		(
			Source.M[0][1] * Source.M[1][3] * Source.M[2][2] +
			Source.M[0][2] * Source.M[1][1] * Source.M[2][3] +
			Source.M[0][3] * Source.M[1][2] * Source.M[2][1] -
			Source.M[0][1] * Source.M[1][2] * Source.M[2][3] -
			Source.M[0][2] * Source.M[1][3] * Source.M[2][1] -
			Source.M[0][3] * Source.M[1][1] * Source.M[2][2]
			) / detA;

	Out->M[1][0] =
		(
			Source.M[1][0] * Source.M[2][3] * Source.M[3][2] +
			Source.M[1][2] * Source.M[2][0] * Source.M[3][3] +
			Source.M[1][3] * Source.M[2][2] * Source.M[3][0] -
			Source.M[1][0] * Source.M[2][2] * Source.M[3][3] -
			Source.M[1][2] * Source.M[2][3] * Source.M[3][0] -
			Source.M[1][3] * Source.M[2][0] * Source.M[3][2]
			) / detA;

	Out->M[1][1] =
		(
			Source.M[0][0] * Source.M[2][2] * Source.M[3][3] +
			Source.M[0][2] * Source.M[2][3] * Source.M[3][0] +
			Source.M[0][3] * Source.M[2][0] * Source.M[3][2] -
			Source.M[0][0] * Source.M[2][3] * Source.M[3][2] -
			Source.M[0][2] * Source.M[2][0] * Source.M[3][3] -
			Source.M[0][3] * Source.M[2][2] * Source.M[3][0]
			) / detA;

	Out->M[1][2] =
		(
			Source.M[0][0] * Source.M[1][3] * Source.M[3][2] +
			Source.M[0][2] * Source.M[1][0] * Source.M[3][3] +
			Source.M[0][3] * Source.M[1][2] * Source.M[3][0] -
			Source.M[0][0] * Source.M[1][2] * Source.M[3][3] -
			Source.M[0][2] * Source.M[1][3] * Source.M[3][0] -
			Source.M[0][3] * Source.M[1][0] * Source.M[3][2]
			) / detA;

	Out->M[1][3] =
		(
			Source.M[0][0] * Source.M[1][2] * Source.M[2][3] +
			Source.M[0][2] * Source.M[1][3] * Source.M[2][0] +
			Source.M[0][3] * Source.M[1][0] * Source.M[2][2] -
			Source.M[0][0] * Source.M[1][3] * Source.M[2][2] -
			Source.M[0][2] * Source.M[1][0] * Source.M[2][3] -
			Source.M[0][3] * Source.M[1][2] * Source.M[2][0]
			) / detA;

	Out->M[2][0] =
		(
			Source.M[1][0] * Source.M[2][1] * Source.M[3][3] +
			Source.M[1][1] * Source.M[2][3] * Source.M[3][0] +
			Source.M[1][3] * Source.M[2][0] * Source.M[3][1] -
			Source.M[1][0] * Source.M[2][3] * Source.M[3][1] -
			Source.M[1][1] * Source.M[2][0] * Source.M[3][3] -
			Source.M[1][3] * Source.M[2][1] * Source.M[3][0]
			) / detA;

	Out->M[2][1] =
		(
			Source.M[0][0] * Source.M[2][3] * Source.M[3][1] +
			Source.M[0][1] * Source.M[2][0] * Source.M[3][3] +
			Source.M[0][3] * Source.M[2][1] * Source.M[3][0] -
			Source.M[0][0] * Source.M[2][1] * Source.M[3][3] -
			Source.M[0][1] * Source.M[2][3] * Source.M[3][0] -
			Source.M[0][3] * Source.M[2][0] * Source.M[3][1]
			) / detA;

	Out->M[2][2] =
		(
			Source.M[0][0] * Source.M[1][1] * Source.M[3][3] +
			Source.M[0][1] * Source.M[1][3] * Source.M[3][0] +
			Source.M[0][3] * Source.M[1][0] * Source.M[3][1] -
			Source.M[0][0] * Source.M[1][3] * Source.M[3][1] -
			Source.M[0][1] * Source.M[1][0] * Source.M[3][3] -
			Source.M[0][3] * Source.M[1][1] * Source.M[3][0]
			) / detA;

	Out->M[2][3] =
		(
			Source.M[0][0] * Source.M[1][3] * Source.M[2][1] +
			Source.M[0][1] * Source.M[1][0] * Source.M[2][3] +
			Source.M[0][3] * Source.M[1][1] * Source.M[2][0] -
			Source.M[0][0] * Source.M[1][1] * Source.M[2][3] -
			Source.M[0][1] * Source.M[1][3] * Source.M[2][0] -
			Source.M[0][3] * Source.M[1][0] * Source.M[2][1]
			) / detA;

	Out->M[3][0] =
		(
			Source.M[1][0] * Source.M[2][2] * Source.M[3][1] +
			Source.M[1][1] * Source.M[2][0] * Source.M[3][2] +
			Source.M[1][2] * Source.M[2][1] * Source.M[3][0] -
			Source.M[1][0] * Source.M[2][1] * Source.M[3][2] -
			Source.M[1][1] * Source.M[2][2] * Source.M[3][0] -
			Source.M[1][2] * Source.M[2][0] * Source.M[3][1]
			) / detA;

	Out->M[3][1] =
		(
			Source.M[0][0] * Source.M[2][1] * Source.M[3][2] +
			Source.M[0][1] * Source.M[2][2] * Source.M[3][0] +
			Source.M[0][2] * Source.M[2][0] * Source.M[3][1] -
			Source.M[0][0] * Source.M[2][2] * Source.M[3][1] -
			Source.M[0][1] * Source.M[2][0] * Source.M[3][2] -
			Source.M[0][2] * Source.M[2][1] * Source.M[3][0]
			) / detA;

	Out->M[3][2] =
		(
			Source.M[0][0] * Source.M[1][2] * Source.M[3][1] +
			Source.M[0][1] * Source.M[1][0] * Source.M[3][2] +
			Source.M[0][2] * Source.M[1][1] * Source.M[3][0] -
			Source.M[0][0] * Source.M[1][1] * Source.M[3][2] -
			Source.M[0][1] * Source.M[1][2] * Source.M[3][0] -
			Source.M[0][2] * Source.M[1][0] * Source.M[3][1]
			) / detA;

	Out->M[3][3] =
		(
			Source.M[0][0] * Source.M[1][1] * Source.M[2][2] +
			Source.M[0][1] * Source.M[1][2] * Source.M[2][0] +
			Source.M[0][2] * Source.M[1][0] * Source.M[2][1] -
			Source.M[0][0] * Source.M[1][2] * Source.M[2][1] -
			Source.M[0][1] * Source.M[1][0] * Source.M[2][2] -
			Source.M[0][2] * Source.M[1][1] * Source.M[2][0]
			) / detA;	
}

void SMatrix::Transposion(SMatrix* Out, const SMatrix& Source)
{
	Out->M[0][0] = Source.M[0][0];
	Out->M[0][1] = Source.M[1][0];
	Out->M[0][2] = Source.M[2][0];
	Out->M[0][3] = Source.M[3][0];

	Out->M[1][0] = Source.M[0][1];
	Out->M[1][1] = Source.M[1][1];
	Out->M[1][2] = Source.M[2][1];
	Out->M[1][3] = Source.M[3][1];

	Out->M[2][0] = Source.M[0][2];
	Out->M[2][1] = Source.M[1][2];
	Out->M[2][2] = Source.M[2][2];
	Out->M[2][3] = Source.M[3][2];

	Out->M[3][0] = Source.M[0][3];
	Out->M[3][1] = Source.M[1][3];
	Out->M[3][2] = Source.M[2][3];
	Out->M[3][3] = Source.M[3][3];
}

void SMatrix::LookAt(SMatrix* Out, CRVector3 Position, CRVector3 At, CRVector3 Up)
{
	SVector3<float> AtVector, UpVector, XVector, YVector, ZVector;

	// 視点→注視点のベクトルを得る
	AtVector = At - Position;

	// 視点に垂直なベクトルを得る
	UpVector = Up;

	// 新X軸のベクトルを算出
	XVector = SVector3<float>::Cross(UpVector, AtVector);

	// 新Y軸のベクトルを算出
	YVector = SVector3<float>::Cross(AtVector, XVector);

	// 新Z軸のベクトルをセット
	ZVector = AtVector;

	// 各ベクトルの正規化
	XVector.Normalize();
	YVector.Normalize();
	ZVector.Normalize();

	// ビュー行列の作成
	ZeroMemory(Out, sizeof(SMatrix));
	Out->M[0][0] = XVector.X; Out->M[0][1] = YVector.X; Out->M[0][2] = ZVector.X; Out->M[0][3] = 0.0f;
	Out->M[1][0] = XVector.Y; Out->M[1][1] = YVector.Y; Out->M[1][2] = ZVector.Y; Out->M[1][3] = 0.0f;
	Out->M[2][0] = XVector.Z; Out->M[2][1] = YVector.Z; Out->M[2][2] = ZVector.Z; Out->M[2][3] = 0.0f;
	Out->M[3][3] = 1.0f;

	Out->M[3][0] = -(Position.X * XVector.X + Position.Y * XVector.Y + Position.Z * XVector.Z);
	Out->M[3][1] = -(Position.X * YVector.X + Position.Y * YVector.Y + Position.Z * YVector.Z);
	Out->M[3][2] = -(Position.X * ZVector.X + Position.Y * ZVector.Y + Position.Z * ZVector.Z);	
}

void SMatrix::LookAtRH(SMatrix* Out, CRVector3 Position, CRVector3 At, CRVector3 Up)
{
	SVector3<float> AtVector, UpVector, XVector, YVector, ZVector;

	// 視点→注視点のベクトルを得る
	AtVector = Position - At;

	// 視点に垂直なベクトルを得る
	UpVector = Up;

	// 新X軸のベクトルを算出
	XVector = SVector3<float>::Cross(UpVector, AtVector);

	// 新Y軸のベクトルを算出
	YVector = SVector3<float>::Cross(AtVector, XVector);

	// 新Z軸のベクトルをセット
	ZVector = AtVector;

	// 各ベクトルの正規化
	XVector.Normalize();
	YVector.Normalize();
	ZVector.Normalize();

	// ビュー行列の作成
	ZeroMemory(Out, sizeof(SMatrix));
	Out->M[0][0] = XVector.X; Out->M[0][1] = YVector.X; Out->M[0][2] = ZVector.X; Out->M[0][3] = 0.0f;
	Out->M[1][0] = XVector.Y; Out->M[1][1] = YVector.Y; Out->M[1][2] = ZVector.Y; Out->M[1][3] = 0.0f;
	Out->M[2][0] = XVector.Z; Out->M[2][1] = YVector.Z; Out->M[2][2] = ZVector.Z; Out->M[2][3] = 0.0f;
	Out->M[3][3] = 1.0f;

	Out->M[3][0] = -(Position.X * XVector.X + Position.Y * XVector.Y + Position.Z * XVector.Z);
	Out->M[3][1] = -(Position.X * YVector.X + Position.Y * YVector.Y + Position.Z * YVector.Z);
	Out->M[3][2] = -(Position.X * ZVector.X + Position.Y * ZVector.Y + Position.Z * ZVector.Z);	
}

void SMatrix::LookRotation(SMatrix* Out, CRVector3 Eye, 
	float RotationVertical, float RotationHorizontal, float RotationTilde)
{
	SVector3<float> ZVector, YVector, XVector, TVector;

	float Sin = FMath::Sin(RotationVertical);
	float Cos = FMath::Cos(RotationVertical);
	ZVector.X = 0.0f;
	ZVector.Y = -Sin;
	ZVector.Z = Cos;

	YVector.Z = Sin;
	YVector.Y = Cos;
	YVector.X = 0.0f;

	Sin = FMath::Sin(RotationHorizontal);
	Cos = FMath::Cos(RotationHorizontal);
	ZVector.X = Sin * ZVector.Z;
	ZVector.Z *= Cos;

	YVector.X = Sin * YVector.Z;
	YVector.Z *= Cos;

	XVector.X = Cos;
	XVector.Y = 0.0f;
	XVector.Z = -Sin;

	Sin = FMath::Sin(RotationTilde);
	Cos = FMath::Cos(RotationTilde);
	TVector.X = -Sin * XVector.X + Cos * YVector.X;
	TVector.Y = -Sin * XVector.Y + Cos * YVector.Y;
	TVector.Z = -Sin * XVector.Z + Cos * YVector.Z;

	auto Target = Eye + ZVector;

	SMatrix::LookAt(Out, Eye, Target, TVector);
}

void SMatrix::LookRotationRH(SMatrix* Out, CRVector3 Eye, float RotationVertical, float RotationHorizontal, float RotationTilde)
{
	SVector3<float> ZVector, YVector, XVector, TVector;

	float Sin = FMath::Sin(RotationVertical);
	float Cos = FMath::Cos(RotationVertical);
	ZVector.X = 0.0f;
	ZVector.Y = -Sin;
	ZVector.Z = Cos;

	YVector.Z = Sin;
	YVector.Y = Cos;
	YVector.X = 0.0f;

	Sin = FMath::Sin(RotationHorizontal);
	Cos = FMath::Cos(RotationHorizontal);
	ZVector.X = Sin * ZVector.Z;
	ZVector.Z *= Cos;

	YVector.X = Sin * YVector.Z;
	YVector.Z *= Cos;

	XVector.X = Cos;
	XVector.Y = 0.0f;
	XVector.Z = -Sin;

	Sin = FMath::Sin(RotationTilde);
	Cos = FMath::Cos(RotationTilde);
	TVector.X = -Sin * XVector.X + Cos * YVector.X;
	TVector.Y = -Sin * XVector.Y + Cos * YVector.Y;
	TVector.Z = -Sin * XVector.Z + Cos * YVector.Z;

	auto Target = Eye + ZVector;

	SMatrix::LookAtRH(Out, Eye, Target, TVector);
}

void SMatrix::PerspectiveFov(SMatrix* Out, float Fov, float Aspect, float Near, float Far)
{
	float Sin = FMath::Sin(Fov / 2), Cos = FMath::Cos(Fov / 2);

	float w = Aspect * (Cos / Sin);
	float h = 1.0f * (Cos / Sin);
	float Q = Far / (Far - Near);

	ZeroMemory(Out, sizeof(SMatrix));
	Out->M[0][0] = w;
	Out->M[1][1] = h;
	Out->M[2][2] = Q;
	Out->M[3][2] = -Q * Near;
	Out->M[2][3] = 1.0f;
	Out->M[3][3] = 0.0f;	
}

void SMatrix::PerspectiveFovRH(SMatrix* Out, float Fov, float Aspect, float Near, float Far)
{
	float Sin = FMath::Sin(Fov / 2), Cos = FMath::Cos(Fov / 2);

	float w = Aspect * (Cos / Sin);
	float h = 1.0f * (Cos / Sin);
	float q = Far / (Near - Far);

	ZeroMemory(Out, sizeof(SMatrix));
	Out->M[0][0] = w;
	Out->M[1][1] = h;
	Out->M[2][2] = q;
	Out->M[3][2] = q * Near;
	Out->M[2][3] = -1.0f;
	Out->M[3][3] = 0.0f;	
}

void SMatrix::Ortho(SMatrix* Out, float Width, float Height, float Near, float Far)
{
	ZeroMemory(Out, sizeof(SMatrix));
	Out->M[0][0] = 2.0f / Width;
	Out->M[1][1] = 2.0f / Height;
	Out->M[2][2] = 1.0f / (Far - Near);
	Out->M[3][2] = Near / (Near - Far);
	Out->M[3][3] = 1.0f;	
}

void SMatrix::OrthoRH(SMatrix* Out, float Width, float Height, float Near, float Far)
{
	ZeroMemory(Out, sizeof(SMatrix));
	Out->M[0][0] = 2.0f / Width;
	Out->M[1][1] = 2.0f / Height;
	Out->M[2][2] = 1.0f / (Near - Far);
	Out->M[3][2] = Near / (Near - Far);
	Out->M[3][3] = 1.0f;	
}

void SMatrix::RotationXYZ(SMatrix* Out, float RotationX, float RotationY, float RotationZ)
{
	float SinXSinY, SinYCosX;
	float SinX = FMath::Sin(RotationX), CosX = FMath::Cos(RotationX);
	float SinY = FMath::Sin(RotationY), CosY = FMath::Cos(RotationY);
	float SinZ = FMath::Sin(RotationZ), CosZ = FMath::Cos(RotationZ);

	SinXSinY = SinX * SinY;
	SinYCosX = SinY * CosX;

	Out->M[0][0] = CosY * CosZ;
	Out->M[0][1] = CosY * SinZ;
	Out->M[0][2] = -SinY;
	Out->M[0][3] = 0.0f;
	Out->M[1][0] = SinXSinY * CosZ - CosX * SinZ;
	Out->M[1][1] = SinXSinY * SinZ + CosX * CosZ;
	Out->M[1][2] = SinX * CosY;
	Out->M[1][3] = 0.0f;
	Out->M[2][0] = SinYCosX * CosZ + SinX * SinZ;
	Out->M[2][1] = SinYCosX * SinZ - SinX * CosZ;
	Out->M[2][2] = CosX * CosY;
	Out->M[2][3] = 0.0f;
	Out->M[3][0] = 0.0f;
	Out->M[3][1] = 0.0f;
	Out->M[3][2] = 0.0f;
	Out->M[3][3] = 1.0f;	
}

void SMatrix::RotationXZY(SMatrix* Out, float RotationX, float RotationY, float RotationZ)
{
	float CosXSinZ, SinXSinZ;
	float SinX = FMath::Sin(RotationX), CosX = FMath::Cos(RotationX);
	float SinY = FMath::Sin(RotationY), CosY = FMath::Cos(RotationY);
	float SinZ = FMath::Sin(RotationZ), CosZ = FMath::Cos(RotationZ);

	CosXSinZ = CosX * SinZ;
	SinXSinZ = SinX * SinZ;

	Out->M[0][0] = CosY * CosZ;
	Out->M[0][1] = SinZ;
	Out->M[0][2] = CosZ * -SinY;
	Out->M[0][3] = 0.0f;
	Out->M[1][0] = CosXSinZ * -CosY + SinX * SinY;
	Out->M[1][1] = CosX * CosZ;
	Out->M[1][2] = CosXSinZ *  SinY + SinX * CosY;
	Out->M[1][3] = 0.0f;
	Out->M[2][0] = SinXSinZ *  CosY + CosX * SinY;
	Out->M[2][1] = -SinX * CosZ;
	Out->M[2][2] = SinXSinZ * -SinY + CosX * CosY;
	Out->M[2][3] = 0.0f;
	Out->M[3][0] = 0.0f;
	Out->M[3][1] = 0.0f;
	Out->M[3][2] = 0.0f;
	Out->M[3][3] = 1.0f;	
}

void SMatrix::RotationYXZ(SMatrix* Out, float RotationX, float RotationY, float RotationZ)
{
	float SinXSinY, CosYSinX;
	float SinX = FMath::Sin(RotationX), CosX = FMath::Cos(RotationX);
	float SinY = FMath::Sin(RotationY), CosY = FMath::Cos(RotationY);
	float SinZ = FMath::Sin(RotationZ), CosZ = FMath::Cos(RotationZ);

	SinXSinY = SinX * SinY;
	CosYSinX = CosY * SinX;

	Out->M[0][0] = CosY * CosZ - SinXSinY * SinZ;
	Out->M[0][1] = CosY * SinZ + SinXSinY * CosZ;
	Out->M[0][2] = -SinY * CosX;
	Out->M[0][3] = 0.0f;
	Out->M[1][0] = CosX * -SinZ;
	Out->M[1][1] = CosX * CosZ;
	Out->M[1][2] = SinX;
	Out->M[1][3] = 0.0f;
	Out->M[2][0] = SinY * CosZ + CosYSinX * SinZ;
	Out->M[2][1] = SinY * SinZ - CosYSinX * CosZ;
	Out->M[2][2] = CosY * CosX;
	Out->M[2][3] = 0.0f;
	Out->M[3][0] = 0.0f;
	Out->M[3][1] = 0.0f;
	Out->M[3][2] = 0.0f;
	Out->M[3][3] = 1.0f;	
}

void SMatrix::RotationYZX(SMatrix* Out, float RotationX, float RotationY, float RotationZ)
{
	float CosYSinZ, SinYSinZ;
	float SinX = FMath::Sin(RotationX), CosX = FMath::Cos(RotationX);
	float SinY = FMath::Sin(RotationY), CosY = FMath::Cos(RotationY);
	float SinZ = FMath::Sin(RotationZ), CosZ = FMath::Cos(RotationZ);

	CosYSinZ = CosY * SinZ;
	SinYSinZ = SinY * SinZ;

	Out->M[0][0] = CosY * CosZ;
	Out->M[0][1] = CosYSinZ * CosX + SinY * SinX;
	Out->M[0][2] = CosYSinZ * SinX - SinY * CosX;
	Out->M[0][3] = 0.0f;
	Out->M[1][0] = -SinZ;
	Out->M[1][1] = CosZ * CosX;
	Out->M[1][2] = CosZ * SinX;
	Out->M[1][3] = 0.0f;
	Out->M[2][0] = SinY * CosZ;
	Out->M[2][1] = SinYSinZ * CosX - CosY * SinX;
	Out->M[2][2] = SinYSinZ * SinX + CosY * CosX;
	Out->M[2][3] = 0.0f;
	Out->M[3][0] = 0.0f;
	Out->M[3][1] = 0.0f;
	Out->M[3][2] = 0.0f;
	Out->M[3][3] = 1.0f;	
}

void SMatrix::RotationZXY(SMatrix* Out, float RotationX, float RotationY, float RotationZ)
{
	float SinXSinZ, SinXCosZ;
	float SinX = FMath::Sin(RotationX), CosX = FMath::Cos(RotationX);
	float SinY = FMath::Sin(RotationY), CosY = FMath::Cos(RotationY);
	float SinZ = FMath::Sin(RotationZ), CosZ = FMath::Cos(RotationZ);

	SinXSinZ = SinX * SinZ;
	SinXCosZ = SinX * CosZ;

	Out->M[0][0] = CosY *  CosZ + SinXSinZ * SinY;
	Out->M[0][1] = CosX * SinZ;
	Out->M[0][2] = CosZ * -SinY + SinXSinZ * CosY;
	Out->M[0][3] = 0.0f;
	Out->M[1][0] = CosY * -SinZ + SinXCosZ * SinY;
	Out->M[1][1] = CosX * CosZ;
	Out->M[1][2] = SinZ *  SinY + SinXCosZ * CosY;
	Out->M[1][3] = 0.0f;
	Out->M[2][0] = CosX * SinY;
	Out->M[2][1] = -SinX;
	Out->M[2][2] = CosX * CosY;
	Out->M[2][3] = 0.0f;
	Out->M[3][0] = 0.0f;
	Out->M[3][1] = 0.0f;
	Out->M[3][2] = 0.0f;
	Out->M[3][3] = 1.0f;	
}

void SMatrix::RotationZYX(SMatrix* Out, float RotationX, float RotationY, float RotationZ)
{
	float CosZSinY, SinZSinY;
	float SinX = FMath::Sin(RotationX), CosX = FMath::Cos(RotationX);
	float SinY = FMath::Sin(RotationY), CosY = FMath::Cos(RotationY);
	float SinZ = FMath::Sin(RotationZ), CosZ = FMath::Cos(RotationZ);

	CosZSinY = CosZ * SinY;
	SinZSinY = SinZ * SinY;

	Out->M[0][0] = CosY * CosZ;
	Out->M[0][1] = SinZ * CosX + CosZSinY * SinX;
	Out->M[0][2] = SinZ * SinX - CosZSinY * CosX;
	Out->M[0][3] = 0.0f;
	Out->M[1][0] = -SinZ * CosY;
	Out->M[1][1] = CosZ * CosX - SinZSinY * SinX;
	Out->M[1][2] = CosZ * SinX + SinZSinY * CosX;
	Out->M[1][3] = 0.0f;
	Out->M[2][0] = SinY;
	Out->M[2][1] = CosY * -SinX;
	Out->M[2][2] = CosY *  CosX;
	Out->M[2][3] = 0.0f;
	Out->M[3][0] = 0.0f;
	Out->M[3][1] = 0.0f;
	Out->M[3][2] = 0.0f;
	Out->M[3][3] = 1.0f;	
}