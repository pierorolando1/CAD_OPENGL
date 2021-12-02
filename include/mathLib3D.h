#ifndef MATHLIB_3D_H
#define MATHLIB_3D_H

#include <string>

class Point3D
{
public:
    Point3D();
    Point3D(float x, float y, float z);
    float x;
    float y;
    float z;

    float distanceTo(Point3D other);
    float fastDistanceTo(Point3D other);
    std::string toString();
};

class Vec3D
{
public:
    Vec3D();
    Vec3D(float x, float y, float z);
    float x;
    float y;
    float z;
    float length();
    Vec3D normalize();
    Vec3D multiply(float scalar);
    Vec3D add(Vec3D vec);
    Vec3D rotate(Vec3D rot);
    Vec3D setRotation(Vec3D rot);
    Vec3D calcRotation();
    Point3D movePoint(Point3D source);
    std::string toString();
    static Vec3D createVector(Point3D p1, Point3D p2);
    static float dot(Vec3D v1, Vec3D v2);
    static Vec3D cross(Vec3D v1, Vec3D v2);
    static float toRadians(float degrees);
    static float toDegrees(float radians);
};

#endif