#ifndef LIGHT_H
#define LIGHT_H

#include <mathLib3D.h>

class Light
{
public:
    Light();
    Light(float pos[4], float amb[4], float diff[4], float spec[4]);
    float* pos;
    float* amb;
    float* diff;
    float* spec;
    bool selected;

    bool rayIntersects(Vec3D dir, Point3D origin);
};

#endif