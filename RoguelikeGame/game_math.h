#ifndef GAMEMATH_H
#define GAMEMATH_H

#define PI 3.141592f

float ToRadian(float degree);
float ToDegree(float radian);

struct Vec3
{
    float x, y, z;

    explicit
    Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f)
        : x(x), y(y), z(z) {}

    void Set(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

float DotProduct(const Vec3& rhs, const Vec3& lhs);
Vec3 CrossProduct(const Vec3& rhs, const Vec3& lhs);

struct Vec2
{
    float x, y;

    explicit
    Vec2(float x = 0.0f, float y = 0.0f)
        : x(x), y(y) {}

    void Set(float x, float y)
    {
        this->x = x;
        this->y = y;
    }
};


struct Mat4
{
    float m[16];

    Mat4& operator*=(const Mat4& rhs);
};

Mat4 operator*(const Mat4& lhs, const Mat4& rhs);
Mat4 Identity();
Mat4 Translation(float x, float y, float z);
Mat4 Scale(float scaleX, float scaleY, float scaleZ);
Mat4 Rotation(float radian);

#endif//GAMEMATH_H
