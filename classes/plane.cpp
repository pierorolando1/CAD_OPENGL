#include <plane.h>
#include <cfloat>
#include <iostream>

Plane::Plane(Point3D pos, Vec3D normal, float width, float height)
{
    this->pos = pos;
    this->normal = normal;
    this->width = width;
    this->height = height;
}

std::vector<Point3D> Plane::rayIntersects(Vec3D dir, Point3D rayO)
{
    std::vector<Point3D> hits;    
    float dot = Vec3D::dot(this->normal, dir);    
    if (std::abs(dot) <= FLT_EPSILON)
    {            
        return hits;
    }
    Vec3D rayOrigin = Vec3D::createVector(Point3D(), rayO);    
    Vec3D rayDir = dir;
    Plane planeT = Plane(this->pos, this->normal, this->width, this->height);    

    Vec3D inverseTranslation = Vec3D::createVector(planeT.pos, Point3D());
    planeT.pos = Point3D(0,0,0);
    rayOrigin = rayOrigin.add(inverseTranslation);

    Vec3D inverseRotation = planeT.normal.calcRotation().multiply(-1.0);
    planeT.normal = planeT.normal.rotate(inverseRotation);
    rayOrigin = rayOrigin.rotate(inverseRotation);
    rayDir = rayDir.rotate(inverseRotation);

    float distance = planeT.pos.distanceTo(Point3D());
    float t = (-1.0*(Vec3D::dot(planeT.normal, rayOrigin) + distance))/(Vec3D::dot(planeT.normal, rayDir));
    Vec3D hit = rayOrigin.add(rayDir.multiply(t));

    if ((hit.x >= planeT.width/-2.0 && hit.x <= planeT.width/2.0)&&(hit.z >= planeT.height/-2.0 && hit.z <= planeT.height/2.0))
    {
        Vec3D p = hit;
        p = p.rotate(inverseRotation.multiply(-1));
        p = p.add(inverseTranslation.multiply(-1));
        hits.push_back(Point3D(p.x, p.y, p.z));
        return hits;
    }
    return hits;
}