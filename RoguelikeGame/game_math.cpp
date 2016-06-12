#include "game_math.h"
#include <cmath>

float ToRadian(float degree)
{
    return (degree / 180.0f * PI);
}
float ToDegree(float radian)
{
    return (radian / PI * 180.0f);
}

float DotProduct(const Vec3& rhs, const Vec3& lhs)
{
    return(rhs.x * lhs.x + rhs.y * lhs.y + rhs.z * lhs.z);
}

Vec3 CrossProduct(const Vec3& rhs, const Vec3& lhs)
{
    return Vec3(rhs.y * lhs.z - rhs.z * lhs.y, rhs.z * lhs.x - rhs.x * lhs.z, rhs.x * lhs.y - rhs.y * lhs.x);
}

Mat4& Mat4::operator*=(const Mat4& rhs)
{
    *this = *this * rhs;

    return *this;
}

Mat4 operator*(const Mat4& lhs, const Mat4& rhs)
{
    Mat4 result = {};

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            result.m[i * 4 + j] = lhs.m[i * 4] * rhs.m[j] + lhs.m[i * 4 + 1] * rhs.m[j + 4] + lhs.m[i * 4 + 2] * rhs.m[j + 8] + lhs.m[i * 4 + 3] * rhs.m[j + 12];

    return result;
}

Mat4 Identity()
{
    Mat4 out =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
    return out;
}

Mat4 Translation(float x, float y, float z)
{
    Mat4 out =
    {
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1,
    };

    return out;
}

Mat4 Scale(float scaleX, float scaleY, float scaleZ)
{
    Mat4 out =
    {
        scaleX, 0, 0, 0,
        0, scaleY, 0, 0,
        0, 0, scaleZ, 0,
        0, 0, 0, 1,
    };

    return out;
}


Mat4 Rotation(float radian)
{
    //float radian = degree * PI / 180.0f;
    float c = cos(radian);
    float s = sin(radian);
    Mat4 out =
    {
        c, -s, 0, 0,
        s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };

    return out;
}