#ifndef OBJECT_H
#define OBJECT_H

#include <mathLib3D.h>
#include <material.h>
#include <plane.h>
#include <vector>

class Object
{
public:
    enum Shape {CUBE, SPHERE, CONE, TEAPOT, TETRAHEDRON, ICOSAHEDRON, CUSTOM};
    // static Shape shape;

    Object();
    Object(Point3D position, Vec3D rotation, Shape shape, Vec3D scale, Point3D boundMin, Point3D boundMax, int matIndex);

    Point3D pos;
    Vec3D rot;
    Vec3D scale;
    Shape shape;
    Point3D boundMin;
    Point3D boundMax;
    int matIndex;    
    bool selected;

    void draw();
    void drawBounds();
    std::vector<Plane> calculatePlanes();
    float rayIntersections(Vec3D dir, Point3D origin);
};

#endif