#ifndef PLANE_H
#define PLANE_H

#include <mathLib3D.h>
#include <vector>

class Plane
{
public:
    Plane(Point3D pos, Vec3D normal, float width, float height);

    Point3D pos;
    Vec3D normal;
    float width;
    float height;

    std::vector<Point3D> rayIntersects(Vec3D dir, Point3D rayO);
};

#endif