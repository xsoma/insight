#include "Math.hpp"
#include "intrincics.h"
namespace Math
{
	 float Rad2Deg (float val) {
		return val * (180.f / pi);
	}
	 double Deg2Rad(double degrees) {
		 return degrees * 4.0 * atan(1.0) / 180.0;
	 }

	float Magnitude(Vector a)
	{
		return sqrt((a.x * a.x) + (a.y * a.y));
	}
	Vector Normalize(Vector value)
	{
		float num = Magnitude(value);
		if (num != 0.f)
			return value / num;
		return Vector(0.f, 0.f, 0.f);
	}
	Vector ClampMagnitude(Vector vector, float maxLength)
	{
		if (Magnitude(vector) > maxLength)
			return Vector(Normalize(vector).x * maxLength, Normalize(vector).y * maxLength, 0);
		return vector;
	}
	void MatrixCopy(const matrix3x4_t& in, matrix3x4_t& out) {
		// Mutiny paste
		memcpy(out.Base(), in.Base(), sizeof(float) * 3 * 4);
	}

	void AngleMatrix(const QAngle &angles, matrix3x4_t& matrix) {
#ifdef _VPROF_MATHLIB
		VPROF_BUDGET("AngleMatrix", "Mathlib");
#endif
		//Assert(s_bMathlibInitialized);

		float sr, sp, sy, cr, cp, cy;

#ifdef _X360
		fltx4 radians, scale, sine, cosine;
		radians = LoadUnaligned3SIMD(angles.Base());
		scale = ReplicateX4(M_PI_F / 180.f);
		radians = MulSIMD(radians, scale);
		SinCos3SIMD(sine, cosine, radians);

		sp = SubFloat(sine, 0);	sy = SubFloat(sine, 1);	sr = SubFloat(sine, 2);
		cp = SubFloat(cosine, 0);	cy = SubFloat(cosine, 1);	cr = SubFloat(cosine, 2);
#else
		SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
		SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);
		SinCos(DEG2RAD(angles[ROLL]), &sr, &cr);
#endif

		// matrix = (YAW * PITCH) * ROLL
		matrix[0][0] = cp * cy;
		matrix[1][0] = cp * sy;
		matrix[2][0] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		matrix[0][1] = sp * srcy - crsy;
		matrix[1][1] = sp * srsy + crcy;
		matrix[2][1] = sr * cp;

		matrix[0][2] = (sp*crcy + srsy);
		matrix[1][2] = (sp*crsy - srcy);
		matrix[2][2] = cr * cp;

		matrix[0][3] = 0.0f;
		matrix[1][3] = 0.0f;
		matrix[2][3] = 0.0f;
	}








	void ConcatTransforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out) {
		//Assert(s_bMathlibInitialized);
		if (&in1 == &out)
		{
			matrix3x4_t in1b;
			MatrixCopy(in1, in1b);
			ConcatTransforms(in1b, in2, out);
			return;
		}
		if (&in2 == &out)
		{
			matrix3x4_t in2b;
			MatrixCopy(in2, in2b);
			ConcatTransforms(in1, in2b, out);
			return;
		}
		out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
			in1[0][2] * in2[2][0];
		out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
			in1[0][2] * in2[2][1];
		out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
			in1[0][2] * in2[2][2];
		out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
			in1[0][2] * in2[2][3] + in1[0][3];
		out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
			in1[1][2] * in2[2][0];
		out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
			in1[1][2] * in2[2][1];
		out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
			in1[1][2] * in2[2][2];
		out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
			in1[1][2] * in2[2][3] + in1[1][3];
		out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
			in1[2][2] * in2[2][0];
		out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
			in1[2][2] * in2[2][1];
		out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
			in1[2][2] * in2[2][2];
		out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
			in1[2][2] * in2[2][3] + in1[2][3];
	}



	float Segment2Segment(const Vector s1, const Vector s2, const Vector k1, const Vector k2)
	{
		static auto constexpr epsilon = 0.00000001;

		auto u = s2 - s1;
		auto v = k2 - k1;
		const auto w = s1 - k1;

		const auto a = u.Dot(u);
		const auto b = u.Dot(v);
		const auto c = v.Dot(v);
		const auto d = u.Dot(w);
		const auto e = v.Dot(w);
		const auto D = a * c - b * b;
		float sn, sd = D;
		float tn, td = D;

		if (D < epsilon) {
			sn = 0.0;
			sd = 1.0;
			tn = e;
			td = c;
		}
		else {
			sn = b * e - c * d;
			tn = a * e - b * d;

			if (sn < 0.0) {
				sn = 0.0;
				tn = e;
				td = c;
			}
			else if (sn > sd) {
				sn = sd;
				tn = e + b;
				td = c;
			}
		}

		if (tn < 0.0) {
			tn = 0.0;

			if (-d < 0.0)
				sn = 0.0;
			else if (-d > a)
				sn = sd;
			else {
				sn = -d;
				sd = a;
			}
		}
		else if (tn > td) {
			tn = td;

			if (-d + b < 0.0)
				sn = 0;
			else if (-d + b > a)
				sn = sd;
			else {
				sn = -d + b;
				sd = a;
			}
		}

		const float sc = abs(sn) < epsilon ? 0.0 : sn / sd;
		const float tc = abs(tn) < epsilon ? 0.0 : tn / td;

		m128 n;
		auto dp = w + u * sc - v * tc;
		n.f[0] = dp.Dot(dp);
		const auto calc = sqrt_ps(n.v);
		return reinterpret_cast<const m128*>(&calc)->f[0];
	}
	void AngleMatrix(const Vector &angles, matrix3x4_t &matrix)
	{
		float sr, sp, sy, cr, cp, cy;

		SinCos(DEG2RAD(angles[1]), &sy, &cy);
		SinCos(DEG2RAD(angles[0]), &sp, &cp);
		SinCos(DEG2RAD(angles[2]), &sr, &cr);

		// matrix = (YAW * PITCH) * ROLL
		matrix[0][0] = cp * cy;
		matrix[1][0] = cp * sy;
		matrix[2][0] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		matrix[0][1] = sp * srcy - crsy;
		matrix[1][1] = sp * srsy + crcy;
		matrix[2][1] = sr * cp;

		matrix[0][2] = (sp*crcy + srsy);
		matrix[1][2] = (sp*crsy - srcy);
		matrix[2][2] = cr * cp;

		matrix[0][3] = 0.0f;
		matrix[1][3] = 0.0f;
		matrix[2][3] = 0.0f;
	}
	Vector vector_rotate(Vector& in1, matrix3x4_t& in2)
	{
		return Vector(in1.Dot(in2[0]), in1.Dot(in2[1]), in1.Dot(in2[2]));
	}

	Vector VectorRotate(Vector& in1, Vector& in2)
	{
		matrix3x4_t m;
		AngleMatrix(in2, m);
		return vector_rotate(in1, m);
	}

	void vector_i_transform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
	{
		out.x = (in1.x - in2[0][3]) * in2[0][0] + (in1.y - in2[1][3]) * in2[1][0] + (in1.z - in2[2][3]) * in2[2][0];
		out.y = (in1.x - in2[0][3]) * in2[0][1] + (in1.y - in2[1][3]) * in2[1][1] + (in1.z - in2[2][3]) * in2[2][1];
		out.z = (in1.x - in2[0][3]) * in2[0][2] + (in1.y - in2[1][3]) * in2[1][2] + (in1.z - in2[2][3]) * in2[2][2];
	}

	void vector_i_rotate(Vector in1, matrix3x4_t in2, Vector& out)
	{
		out.x = in1.x * in2[0][0] + in1.y * in2[1][0] + in1.z * in2[2][0];
		out.y = in1.x * in2[0][1] + in1.y * in2[1][1] + in1.z * in2[2][1];
		out.z = in1.x * in2[0][2] + in1.y * in2[1][2] + in1.z * in2[2][2];
	}

	bool intersect_line_with_bb(Vector& start, Vector& end, Vector& min, Vector& max) {
		float d1, d2, f;
		auto start_solid = true;
		auto t1 = -1.0f, t2 = 1.0f;

		const float s[3] = { start.x, start.y, start.z };
		const float e[3] = { end.x, end.y, end.z };
		const float mi[3] = { min.x, min.y, min.z };
		const float ma[3] = { max.x, max.y, max.z };

		for (auto i = 0; i < 6; i++) {
			if (i >= 3) {
				const auto j = i - 3;

				d1 = s[j] - ma[j];
				d2 = d1 + e[j];
			}
			else {
				d1 = -s[i] + mi[i];
				d2 = d1 - e[i];
			}

			if (d1 > 0.0f && d2 > 0.0f)
				return false;

			if (d1 <= 0.0f && d2 <= 0.0f)
				continue;

			if (d1 > 0)
				start_solid = false;

			if (d1 > d2) {
				f = d1;
				if (f < 0.0f)
					f = 0.0f;

				f /= d1 - d2;
				if (f > t1)
					t1 = f;
			}
			else {
				f = d1 / (d1 - d2);
				if (f < t2)
					t2 = f;
			}
		}

		return start_solid || (t1 < t2 && t1 >= 0.0f);
	}




	float DistanceToRay(const Vector &pos, const Vector &rayStart, const Vector &rayEnd, float *along, Vector *pointOnRay)
	{
		Vector to = pos - rayStart;
		Vector dir = rayEnd - rayStart;
		float length = dir.Normalize();

		float rangeAlong = dir.Dot(to);
		if (along)
			*along = rangeAlong;

		float range;

		if (rangeAlong < 0.0f)
		{
			range = -(pos - rayStart).Length();

			if (pointOnRay)
				*pointOnRay = rayStart;
		}
		else if (rangeAlong > length)
		{
			range = -(pos - rayEnd).Length();

			if (pointOnRay)
				*pointOnRay = rayEnd;
		}
		else
		{
			Vector onRay = rayStart + rangeAlong * dir;
			range = (pos - onRay).Length();

			if (pointOnRay)
				*pointOnRay = onRay;
		}

		return range;
	}
	//-------------------------------------------------------------------------------
	float DotProduct(const Vector & a, const Vector & b) {
		return (a.x * b.x
			+ a.y * b.y
			+ a.z * b.z);
	}

	//--------------------------------------------------------------------------------
	float VectorDistance(const Vector& v1, const Vector& v2)
	{
		return FASTSQRT(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
	}
	//--------------------------------------------------------------------------------

	void NewVector(const Vector& forward, Vector& angles) {
		float tmp, yaw, pitch;

		if (forward[1] == 0 && forward[0] == 0) {
			yaw = 0;
			if (forward[2] > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else {
			yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
			if (yaw < 0)
				yaw += 360;

			tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
			pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
			if (pitch < 0)
				pitch += 360;
		}

		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0;
	}
	void NormalizeAnglesV(Vector& angles)
	{
		for (auto i = 0; i < 3; i++) {
			while (angles[i] < -180.0f) angles[i] += 360.0f;
			while (angles[i] > 180.0f) angles[i] -= 360.0f;
		}
	}
	Vector CalculateAngle(const Vector& src, const Vector& dst) {
		Vector ret;
		NewVector(dst - src, ret);
		return ret;
	}

	//
	QAngle CalcAngle(const Vector& src, const Vector& dst)
	{
		QAngle vAngle;
		Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
		double hyp = sqrt(delta.x*delta.x + delta.y*delta.y);

		vAngle.pitch = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
		vAngle.yaw = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
		vAngle.roll = 0.0f;

		if (delta.x >= 0.0)
			vAngle.yaw += 180.0f;

		return vAngle;
	}
	Vector CalcAngle2(const Vector& src, const Vector& dst)
	{
		Vector vAngle;
		Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
		double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

		vAngle.x = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
		vAngle.y = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);

		if (delta.x >= 0.0)
			vAngle.y += 180.0f;

		return vAngle;
	}
	void AngleVectors_2(const Vector& angles, Vector* forward)
	{
		Assert(s_bMathlibInitialized);
		Assert(forward);

		float	sp, sy, cp, cy;

		sy = std::sin(DEG2RAD(angles[1]));
		cy = std::cos(DEG2RAD(angles[1]));

		sp = std::sin(DEG2RAD(angles[0]));
		cp = std::cos(DEG2RAD(angles[0]));

		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}
	//--------------------------------------------------------------------------------
	float GetFOV(const QAngle& viewAngle, const QAngle& aimAngle)
	{
		Vector ang, aim;

		AngleVectors(viewAngle, aim);
		AngleVectors(aimAngle, ang);

		auto res = RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
		if (std::isnan(res))
			res = 0.f;
		return res;
	}



	//--------------------------------------------------------------------------------
	float NormalizeYaw(float yaw)
	{
		if (yaw > 180)
			yaw -= (round(yaw / 360) * 360.f);
		else if (yaw < -180)
			yaw += (round(yaw / 360) * -360.f);

		return yaw;
	}

	void NormalizeAngles(QAngle& angles)
	{
		for (auto i = 0; i < 3; i++)
		{
			while (angles[i] < -180.0f)
			{
				angles[i] += 360.0f;
			}
			while (angles[i] > 180.0f)
			{
				angles[i] -= 360.0f;
			}
		}
	}


	//--------------------------------------------------------------------------------
	void ClampAngles(QAngle& angles)
	{
		if (angles.pitch > 89.0f) angles.pitch = 89.0f;
		else if (angles.pitch < -89.0f) angles.pitch = -89.0f;

		if (angles.yaw > 180.0f) angles.yaw = 180.0f;
		else if (angles.yaw < -180.0f) angles.yaw = -180.0f;

		angles.roll = 0;
	}
	//--------------------------------------------------------------------------------

	Vector Vector_Transform(Vector vector, matrix3x4_t matrix) {
		return Vector(vector.Dot(matrix[0]) + matrix[0][3], vector.Dot(matrix[1]) + matrix[1][3], vector.Dot(matrix[2]) + matrix[2][3]);
	}
	void VectorTransform2(const float *in1, const matrix3x4_t& in2, float *out)
	{
		out[0] = DotProduct(in1, in2[0]) + in2[0][3];
		out[1] = DotProduct(in1, in2[1]) + in2[1][3];
		out[2] = DotProduct(in1, in2[2]) + in2[2][3];
	};
	void VectorTransformWrapper(const Vector& in1, const matrix3x4_t &in2, Vector &out)
	{
		VectorTransform2(&in1.x, in2, &out.x);
	};
	void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
	{
		auto DotProduct = [](const Vector& a, const Vector& b) {
			return (a.x * b.x + a.y * b.y + a.z * b.z);
		};

		out[0] = DotProduct(in1, Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
		out[1] = DotProduct(in1, Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
		out[2] = DotProduct(in1, Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];
	}
	//--------------------------------------------------------------------------------
	void AngleVectors(const QAngle &angles, Vector& forward)
	{
		float	sp, sy, cp, cy;

		DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
		DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}
	//---------------------------------------------------------------------------------


	bool IntersectionBoundingBox(const Vector& src, const Vector& dir, const Vector& min, const Vector& max, Vector* hit_point) {

		// credits : @Soufiw 

		constexpr auto NUMDIM = 3;
		constexpr auto RIGHT = 0;
		constexpr auto LEFT = 1;
		constexpr auto MIDDLE = 2;

		bool inside = true;
		char quadrant[NUMDIM];
		int i;

		// Rind candidate planes; this loop can be avoided if
		// rays cast all from the eye(assume perpsective view)
		Vector candidatePlane;
		for (i = 0; i < NUMDIM; i++) {
			if (src[i] < min[i]) {
				quadrant[i] = LEFT;
				candidatePlane[i] = min[i];
				inside = false;
			}
			else if (src[i] > max[i]) {
				quadrant[i] = RIGHT;
				candidatePlane[i] = max[i];
				inside = false;
			}
			else {
				quadrant[i] = MIDDLE;
			}
		}

		// Ray origin inside bounding box
		if (inside) {
			if (hit_point)
				*hit_point = src;
			return true;
		}

		// Calculate T distances to candidate planes
		Vector maxT;
		for (i = 0; i < NUMDIM; i++) {
			if (quadrant[i] != MIDDLE && dir[i] != 0.f)
				maxT[i] = (candidatePlane[i] - src[i]) / dir[i];
			else
				maxT[i] = -1.f;
		}

		// Get largest of the maxT's for final choice of intersection
		int whichPlane = 0;
		for (i = 1; i < NUMDIM; i++) {
			if (maxT[whichPlane] < maxT[i])
				whichPlane = i;
		}

		// Check final candidate actually inside box
		if (maxT[whichPlane] < 0.f)
			return false;

		for (i = 0; i < NUMDIM; i++) {
			if (whichPlane != i) {
				float temp = src[i] + maxT[whichPlane] * dir[i];
				if (temp < min[i] || temp > max[i]) {
					return false;
				}
				else if (hit_point) {
					(*hit_point)[i] = temp;
				}
			}
			else if (hit_point) {
				(*hit_point)[i] = candidatePlane[i];
			}
		}

		// ray hits box
		return true;
	}

	inline void FastSqrt(float a, float* out)
	{
		const auto xx = _mm_load_ss(&a);
		auto xr = _mm_rsqrt_ss(xx);
		auto xt = _mm_mul_ss(xr, xr);
		xt = _mm_mul_ss(xt, xx);
		xt = _mm_sub_ss(_mm_set_ss(3.f), xt);
		xt = _mm_mul_ss(xt, _mm_set_ss(0.5f));
		xr = _mm_mul_ss(xr, xt);
		_mm_store_ss(out, xr);
	}
	//--------------------------------------------------------------------------------
	float FastVecNormalize(Vector& vec)
	{
		const auto sqrlen = vec.LengthSqr() + 1.0e-10f;
		float invlen;
		FastSqrt(sqrlen, &invlen);
		vec.x *= invlen;
		vec.y *= invlen;
		vec.z *= invlen;
		return sqrlen * invlen;
	}
	float VectorNormalize(Vector& v)
	{
		Assert(v.IsValid());
		float l = v.Length();
		if (l != 0.0f)
		{
			v /= l;
		}
		else
		{
			// FIXME:
			// Just copying the existing implemenation; shouldn't res.z == 0?
			v.x = v.y = 0.0f; v.z = 1.0f;
		}
		return l;
	}
	//------------------------------------------------------------------------------
	void FixVectors(const QAngle &angles, Vector *forward, Vector *right, Vector *up) {
		float sr, sp, sy, cr, cp, cy;

		SinCos(DEG2RAD(angles.pitch), &sp, &cp);
		SinCos(DEG2RAD(angles.yaw), &sy, &cy);
		SinCos(DEG2RAD(angles.roll), &sr, &cr);

		if (forward)
		{
			forward->x = cp * cy;
			forward->z = cp * sy;
			forward->z = -sp;
		}

		if (right)
		{
			right->x = (-1 * sr*sp*cy + -1 * cr*-sy);
			right->y = (-1 * sr*sp*sy + -1 * cr*cy);
			right->z = -1 * sr*cp;
		}

		if (up)
		{
			up->x = (cr*sp*cy + -sr * -sy);
			up->y = (cr*sp*sy + -sr * cy);
			up->z = cr * cp;
		}
	}
    void AngleVectors(const QAngle &angles, Vector& forward, Vector& right, Vector& up)
    {
        float sr, sp, sy, cr, cp, cy;

        DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
        DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
        DirectX::XMScalarSinCos(&sr, &cr, DEG2RAD(angles[2]));

        forward.x = (cp * cy);
        forward.y = (cp * sy);
        forward.z = (-sp);
        right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
        right.y = (-1 * sr * sp * sy + -1 * cr *  cy);
        right.z = (-1 * sr * cp);
        up.x = (cr * sp * cy + -sr*-sy);
        up.y = (cr * sp * sy + -sr*cy);
        up.z = (cr * cp);
    }
	void AngleVectorsQangle(const QAngle& angles, QAngle& forward, Vector& right, Vector& up)
	{
		float sr, sp, sy, cr, cp, cy;

		DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
		DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
		DirectX::XMScalarSinCos(&sr, &cr, DEG2RAD(angles[2]));

		forward.pitch = (cp * cy);
		forward.yaw = (cp * sy);
		forward.roll = (-sp);
		right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right.y = (-1 * sr * sp * sy + -1 * cr * cy);
		right.z = (-1 * sr * cp);
		up.x = (cr * sp * cy + -sr * -sy);
		up.y = (cr * sp * sy + -sr * cy);
		up.z = (cr * cp);
	}
	
    //--------------------------------------------------------------------------------
	Vector CrossProduct(const Vector &a, const Vector &b)
	{
		return Vector(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
	}
	void VectorAngles2(const Vector& forward, Vector& up, QAngle& angles)
	{
		Vector left = CrossProduct(up, forward);
		left.NormalizeInPlace();

		float forwardDist = forward.Length2D();

		if (forwardDist > 0.001f)
		{
			angles.pitch = atan2f(-forward.z, forwardDist) * 180 / PI_F;
			angles.yaw = atan2f(forward.y, forward.x) * 180 / PI_F;

			float upZ = (left.y * forward.x) - (left.x * forward.y);
			angles.roll = atan2f(left.z, upZ) * 180 / PI_F;
		}
		else
		{
			angles.pitch = atan2f(-forward.z, forwardDist) * 180 / PI_F;
			angles.yaw = atan2f(-left.x, left.y) * 180 / PI_F;
			angles.roll = 0;
		}
	}
	//--------------------------------------------------------------------------


	void VectorAngles2(const Vector& forward, Vector& angles)
	{
		float tmp, yaw, pitch;

		if (forward[1] == 0 && forward[0] == 0)
		{
			yaw = 0;
			if (forward[2] > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else
		{
			yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
			if (yaw < 0)
				yaw += 360;

			tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
			pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
			if (pitch < 0)
				pitch += 360;
		}

		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0;
	}

	void Vector_Angles (const Vector& forward, Vector& up, Vector& angles) {

		Vector left = CrossProduct(up, forward);
		left.NormalizeInPlace();

		float forwardDist = forward.Length2D();

		if (forwardDist > 0.001f)
		{
			angles.x = atan2f(-forward.z, forwardDist) * 180 / M_PI;
			angles.y = atan2f(forward.y, forward.x) * 180 / M_PI;

			float upZ = (left.y * forward.x) - (left.x * forward.y);
			angles.z = atan2f(left.z, upZ) * 180 / M_PI;
		}
		else
		{
			angles.x = atan2f(-forward.z, forwardDist) * 180 / M_PI;
			angles.y = atan2f(-left.x, left.y) * 180 / M_PI;
			angles.z = 0;
		}

	}



    void VectorAngles(const Vector& forward, QAngle& angles)
    {
        float	tmp, yaw, pitch;

        if(forward[1] == 0 && forward[0] == 0) {
            yaw = 0;
            if(forward[2] > 0)
                pitch = 270;
            else
                pitch = 90;
        } else {
            yaw = (atan2(forward[1], forward[0]) * 180 / DirectX::XM_PI);
            if(yaw < 0)
                yaw += 360;

            tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
            pitch = (atan2(-forward[2], tmp) * 180 / DirectX::XM_PI);
            if(pitch < 0)
                pitch += 360;
        }

        angles[0] = pitch;
        angles[1] = yaw;
        angles[2] = 0;
    }
    //--------------------------------------------------------------------------------
    static bool screen_transform(const Vector& in, Vector& out)
    {
        static auto& w2sMatrix = g_EngineClient->WorldToScreenMatrix();

        out.x = w2sMatrix.m[0][0] * in.x + w2sMatrix.m[0][1] * in.y + w2sMatrix.m[0][2] * in.z + w2sMatrix.m[0][3];
        out.y = w2sMatrix.m[1][0] * in.x + w2sMatrix.m[1][1] * in.y + w2sMatrix.m[1][2] * in.z + w2sMatrix.m[1][3];
        out.z = 0.0f;

        float w = w2sMatrix.m[3][0] * in.x + w2sMatrix.m[3][1] * in.y + w2sMatrix.m[3][2] * in.z + w2sMatrix.m[3][3];

        if(w < 0.001f) {
            out.x *= 100000;
            out.y *= 100000;
            return false;
        }

        out.x /= w;
        out.y /= w;

        return true;
    }
    //--------------------------------------------------------------------------------
    bool WorldToScreen(const Vector& in, Vector& out)
    {
        if(screen_transform(in, out)) {
            int w, h;
            g_EngineClient->GetScreenSize(w, h);

            out.x = (w / 2.0f) + (out.x * w) / 2.0f;
            out.y = (h / 2.0f) - (out.y * h) / 2.0f;

            return true;
        }
        return false;
    }
	//---------------------------------------------------------------------------------------
	float NormalizePitch(float pitch) {
		while (pitch > 89.0f)
			pitch -= 180.0f;

		while (pitch < -89.0f)
			pitch += 180.0f;

		return pitch;
	}

	float RandomFloat(float min, float max)
	{
		static auto ranFloat = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandleW(L"vstdlib.dll"), "RandomFloat"));
		if (ranFloat)
		{
			return ranFloat(min, max);
		}
		else
		{
			return 0.f;
		}
	}
	int RandomInt(int min, int max)
	{
		static auto ranInt = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandleW(L"vstdlib.dll"), "RandomInt"));
		if (ranInt)
		{
			return ranInt(min, max);
		}
		else
		{
			return 0;
		}
	}

	void FixAngles(QAngle & angle)
	{
	
			Normalize3(angle);
			ClampAngles(angle);
		
	}

	double De2Rad (double deg) {

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/mathlib/mathlib_base.cpp#L3498
		return deg * M_PI / 180.;
	}

	void inline SinCos (float radians, float* sine, float* cosine) {

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/mathlib/mathlib_base.cpp#L3498
		*sine   = sin (radians);
		*cosine = cos (radians);

	}

	


	void AngleVectorS (const Vector& angles, Vector* forward, Vector* right, Vector* up) {

		float sr, sp, sy, cr, cp, cy;

		SinCos((angles[1]), &sy, &cy);
		SinCos(De2Rad(angles[0]), &sp, &cp);
		SinCos(De2Rad(angles[2]), &sr, &cr);

		if (forward) {
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if (right) {
			right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
			right->y = (-1 * sr * sp * sy + -1 * cr * cy);
			right->z = -1 * sr * cp;
		}

		if (up) {
			up->x = (cr * sp * cy + -sr * -sy);
			up->y = (cr * sp * sy + -sr * cy);
			up->z = cr * cp;
		}

	}
	void AngleVectors2(const Vector& angles, Vector& forward)
	{
		Assert(s_bMathlibInitialized);
		Assert(forward);

		float sp, sy, cp, cy;

		sy = sin(DEG2RAD(angles[1]));
		cy = cos(DEG2RAD(angles[1]));

		sp = sin(DEG2RAD(angles[0]));
		cp = cos(DEG2RAD(angles[0]));

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}


	void AngleQ(const QAngle& angles, Vector* forward, Vector* right, Vector* up) {

		float sr, sp, sy, cr, cp, cy;

		SinCos((angles[1]), &sy, &cy);
		SinCos(De2Rad(angles[0]), &sp, &cp);
		SinCos(De2Rad(angles[2]), &sr, &cr);

		if (forward) {
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if (right) {
			right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
			right->y = (-1 * sr * sp * sy + -1 * cr * cy);
			right->z = -1 * sr * cp;
		}

		if (up) {
			up->x = (cr * sp * cy + -sr * -sy);
			up->y = (cr * sp * sy + -sr * cy);
			up->z = cr * cp;
		}

	}

	QAngle NormalizeAng(QAngle angs)
	{
		while (angs.yaw < -180.0f)
			angs.yaw += 360.0f;
		while (angs.yaw > 180.0f)
			angs.yaw -= 360.0f;
		if (angs.pitch > 89.0f)
			angs.pitch = 89.0f;
		if (angs.pitch < -89.0f)
			angs.pitch = -89.0f;
		angs.roll = 0;
		return angs;
	}
	//--------------------------------------------------------------------------------
}
