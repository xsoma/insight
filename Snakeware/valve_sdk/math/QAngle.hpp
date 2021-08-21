#pragma once

#define M_PI 3.14159265358979323846
class QAngle
{
public:

	auto Clear() -> void
	{
		pitch = 0.f;
		yaw = 0.f;
		roll = 0.f;
	}
    QAngle(void)
    {
        Init();
    }
    QAngle(float X, float Y, float Z)
    {
        Init(X, Y, Z);
    }
    QAngle(const float* clr)
    {
        Init(clr[0], clr[1], clr[2]);
    }

    void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
    {
        pitch = ix;
        yaw = iy;
        roll = iz;
    }

    float operator[](int i) const
    {
        return ((float*)this)[i];
    }
    float& operator[](int i)
    {
        return ((float*)this)[i];
    }

    QAngle& operator+=(const QAngle& v)
    {
        pitch += v.pitch; yaw += v.yaw; roll += v.roll;
        return *this;
    }
    QAngle& operator-=(const QAngle& v)
    {
        pitch -= v.pitch; yaw -= v.yaw; roll -= v.roll;
        return *this;
    }
    QAngle& operator*=(float fl)
    {
        pitch *= fl;
        yaw *= fl;
        roll *= fl;
        return *this;
    }
    QAngle& operator*=(const QAngle& v)
    {
        pitch *= v.pitch;
        yaw *= v.yaw;
        roll *= v.roll;
        return *this;
    }
    QAngle& operator/=(const QAngle& v)
    {
        pitch /= v.pitch;
        yaw /= v.yaw;
        roll /= v.roll;
        return *this;
    }
    QAngle& operator+=(float fl)
    {
        pitch += fl;
        yaw += fl;
        roll += fl;
        return *this;
    }
    QAngle& operator/=(float fl)
    {
        pitch /= fl;
        yaw /= fl;
        roll /= fl;
        return *this;
    }
    QAngle& operator-=(float fl)
    {
        pitch -= fl;
        yaw -= fl;
        roll -= fl;
        return *this;
    }

    QAngle& operator=(const QAngle &vOther)
    {
        pitch = vOther.pitch; yaw = vOther.yaw; roll = vOther.roll;
		
        return *this;
    }

	QAngle& operator==(const QAngle &vOther)
	{
		pitch == vOther.pitch; yaw == vOther.yaw; roll == vOther.roll;

		return *this;
	}
    QAngle operator-(void) const
    {
        return QAngle(-pitch, -yaw, -roll);
    }
    QAngle operator+(const QAngle& v) const
    {
        return QAngle(pitch + v.pitch, yaw + v.yaw, roll + v.roll);
    }
    QAngle operator-(const QAngle& v) const
    {
        return QAngle(pitch - v.pitch, yaw - v.yaw, roll - v.roll);
    }
    QAngle operator*(float fl) const
    {
        return QAngle(pitch * fl, yaw * fl, roll * fl);
    }
    QAngle operator*(const QAngle& v) const
    {
        return QAngle(pitch * v.pitch, yaw * v.yaw, roll * v.roll);
    }
    QAngle operator/(float fl) const
    {
        return QAngle(pitch / fl, yaw / fl, roll / fl);
    }
    QAngle operator/(const QAngle& v) const
    {
        return QAngle(pitch / v.pitch, yaw / v.yaw, roll / v.roll);
    }

    float Length() const
    {
        return sqrt(pitch*pitch + yaw*yaw + roll*roll);
    }
    float LengthSqr(void) const
    {
        return (pitch*pitch + yaw*yaw + roll*roll);
    }
    bool IsZero(float tolerance = 0.01f) const
    {
        return (pitch > -tolerance && pitch < tolerance &&
            yaw > -tolerance && yaw < tolerance &&
            roll > -tolerance && roll < tolerance);
    }
	auto Normalized() const -> QAngle
	{
		auto vec = *this;
		vec.Normalize();
		return vec;
	}
	float Normalize() const
	{
		QAngle res = *this;
		float l = res.Length();
		if (l != 0.0f)
		{
			res /= l;
		}
		else
		{
			res[0] = res[1] = res[2] = 0.0f;
		}
		return l;
	}

    float pitch;
    float yaw;
    float roll;
	QAngle Clamp();
	QAngle NormalizeNoClamp();
	void sincos(float a, float* s, float* c) {
		*s = sinf(a);
		*c = cosf(a);
	}
	float rad(float deg) {
		return deg * M_PI / 180.f;
	}

	Vector vector () {
		float sp, sy, cp, cy;
		sincos(rad(yaw), &sy, &cy);
		sincos(rad(pitch), &sp, &cp);
		Vector forward;
		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;

		return forward;
	}
};

inline QAngle operator*(float lhs, const QAngle& rhs)
{
    return rhs * lhs;
}
inline QAngle operator/(float lhs, const QAngle& rhs)
{
    return rhs / lhs;
}
inline QAngle QAngle::Clamp()
{
	

	if (this->pitch < -89.0f)
		this->pitch = -89.0f;

	if (this->pitch > 89.0f)
		this->pitch = 89.0f;

	while (this->yaw < -180.0f)
		this->yaw += 360.0f;

	while (this->yaw > 180.0f)
		this->yaw -= 360.0f;

	this->roll = 0.0f;

	return *this;
}




inline QAngle QAngle::NormalizeNoClamp()
{
	

	this->pitch -= floorf(this->pitch / 360.0f + 0.5f) * 360.0f;

	this->yaw -= floorf(this->yaw / 360.0f + 0.5f) * 360.0f;

	this->roll -= floorf(this->roll / 360.0f + 0.5f) * 360.0f;

	return *this;
}
