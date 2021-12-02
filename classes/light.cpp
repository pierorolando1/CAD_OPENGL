#include <light.h>

Light::Light()
{
    pos = nullptr;
    amb = nullptr;
    diff = nullptr;
    spec = nullptr;
    selected = false;
}

Light::Light(float pos[4], float amb[4], float diff[4], float spec[4])
{
    this->pos = pos;
    this->amb = amb;
    this->diff = diff;
    this->spec = spec;
    this->selected = false;
}

bool Light::rayIntersects(Vec3D dir, Point3D origin)
{
    Vec3D o = Vec3D::createVector(Point3D(), origin);
    Vec3D pc = Vec3D::createVector(Point3D(), Point3D(this->pos[0], this->pos[1], this->pos[2]));
    Vec3D opc = pc.multiply(-1).add(o);

    float a = Vec3D::dot(dir,dir);
    float b = Vec3D::dot(opc, dir) * 2.0;
    float c = (Vec3D::dot(opc, opc)) - 1;

    if (b*b - 4.0*a*c < 0)
    {
        return false;
    }
    return true;
}