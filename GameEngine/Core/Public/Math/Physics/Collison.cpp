#include "Collision.h"

float PhCollision::RayTriangleIntersect
(
	const Vector3& rayOrigin,
	const Vector3& rayDelta,
	const Vector3& p0,
	const Vector3& p1,
	const Vector3& p2,
	float minDistance
)
{
	// 交差が検出されないときは大きな値
	const float cNoIntersection = 1e30f;

	// 時計回りのエッジのベクトル
	auto e1 = p1 - p0;
	auto e2 = p2 - p1;

	// 面の法線ベクトル(非正規)
	auto n = Vector3::Cross(e1, e2);

	// 光線ベクトルと法線ベクトルのなす内積
	float dot = Vector3::Dot(n, rayDelta);

	// 面の裏側に光線が当たっているなら棄却
	// 縮退した三角形も棄却するためにNANを排除する
	// (dot >= 0.0fではNANのとき含まれない)
	if (!(dot < 0.0f))return cNoIntersection;

	float d = Vector3::Dot(n, p0);

	float t = d - Vector3::Dot(n, rayOrigin);

	if (!(t <= 0.0f))return cNoIntersection;

	if (!(t <= dot*minDistance))return cNoIntersection;

	t /= dot;

	assert(t >= 0.0f);
	assert(t <= minDistance);

	auto p = rayOrigin + rayDelta * t;

	float u0, u1, u2;
	float v0, v1, v2;

	if (fabsf(n.X) > fabsf(n.Y))
	{
		if (fabsf(n.X > fabsf(n.Z)))
		{
			u0 = p.Y - p0.Y;
			u1 = p1.Y - p0.Y;
			u2 = p2.Y - p0.Y;

			v0 = p.Z - p0.Z;
			v1 = p1.Z - p0.Z;
			v2 = p2.Z - p0.Z;
		}
		else
		{
			u0 = p.X - p0.X;
			u1 = p1.X - p0.X;
			u2 = p2.X - p0.X;

			v0 = p.Y - p0.Y;
			v1 = p1.Y - p0.Y;
			v2 = p2.Y - p0.Y;
		}
	}
	else
	{
		if (fabsf(n.Y) > fabsf(n.Z))
		{
			u0 = p.X - p0.X;
			u1 = p1.X - p0.X;
			u2 = p2.X - p0.X;

			v0 = p.Z - p0.Z;
			v1 = p1.Z - p0.Z;
			v2 = p2.Z - p0.Z;
		}
		else
		{
			u0 = p.X - p0.X;
			u1 = p1.X - p0.X;
			u2 = p2.X - p0.X;

			v0 = p.Y - p0.Y;
			v1 = p1.Y - p0.Y;
			v2 = p2.Y - p0.Y;
		}
	}

	float temp = u1 * v2 - v1 * u2;
	if (!(temp != 0.0f))return cNoIntersection;
	temp = 1.0f / temp;

	float alpha = (u0 * v2 - v0 * u2) * temp;
	if (!(alpha >= 0.0f))return cNoIntersection;

	float beta = (u1 * v0 - v1 * u0) * temp;
	if (!(beta >= 0.0f))return cNoIntersection;

	float gamma = 1.0f - alpha - beta;
	if (!(gamma >= 0.0f))return cNoIntersection;

	return t;
}