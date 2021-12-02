#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

#include <object.h>
#include <material.h>
#include <cmath>
#include <iostream>
#include <mesh.h>

Object::Object()
{
    pos = Point3D();
    rot = Vec3D();
    scale = Vec3D(1,1,1);
    shape = Object::CUBE;
    boundMin = Point3D(-1,-1,-1);
    boundMax = Point3D(1,1,1);
    selected = false;
}

Object::Object(Point3D pos, Vec3D rot, Shape shape, Vec3D scale, Point3D boundMin, Point3D boundMax, int matIndex)
{
    this->pos = pos;
    this->rot = rot;
    this->matIndex = matIndex;    
    this->shape = shape;
    this->boundMin = boundMin;
    this->boundMax = boundMax;
    this->scale = scale;
    selected = false;
}

// to be called in object local space (ie. after tranformations into correct space -- [0,0,0] is object origin)
void Object::draw()
{
    glPushMatrix();
    glScalef(scale.x, scale.y, scale.z);

    switch(shape)
    {
        case Object::CUBE:            
            glutSolidCube(1);
            break;
        case Object::SPHERE:
            glutSolidSphere(0.5, 16, 16);
            break;
        case Object::CONE:
            glutSolidCone(1, 2, 16, 16);
            break;
        case Object::TEAPOT:
            glFrontFace(GL_CW);            
            glTexCoord2f(0,0);
            glutSolidTeapot(1);
            glFrontFace(GL_CCW);
            break;
        case Object::TETRAHEDRON:
            glutSolidTetrahedron();
            break;
        case Object::ICOSAHEDRON:
            glutSolidIcosahedron();
            break;
        case Object::CUSTOM:
            Mesh snowman = Mesh::createFromOBJ("obj/snowman.obj");
            snowman.draw();
    }

    glPopMatrix();
}

// to be called in world space (ie. no transformations -- [0,0,0] is at the world origin)
void Object::drawBounds()
{
    std::vector<Plane> planes = calculatePlanes();
    glPointSize(5);
    glDisable(GL_LIGHTING);
    glPushMatrix();

    for(long long unsigned int i = 0; i < planes.size(); i++)
    {
        Point3D pos = planes[i].pos;
        glColor3f(0,1,0);
        glBegin(GL_POINTS);
        glVertex3f(pos.x,pos.y,pos.z);
        glEnd();
    }

    glPushMatrix();
    glTranslatef(this->pos.x, this->pos.y, this->pos.z);
    glRotatef(this->rot.x, 1,0,0);
    glRotatef(this->rot.y, 0,1,0);
    glRotatef(this->rot.z, 0,0,1);
    glScalef(this->scale.x, this->scale.y, this->scale.z);
    glutWireCube(2);
    glPopMatrix();

    glPopMatrix();
    glEnable(GL_LIGHTING);
}

std::vector<Plane> Object::calculatePlanes()
{
    std::vector<Plane> planes;
    Vec3D dir;
    Point3D pos;

    dir = Vec3D(1,0,0).rotate(Vec3D(this->rot.x,0,0)).rotate(Vec3D(0,this->rot.y,0)).rotate(Vec3D(0,0,this->rot.z));
    pos = dir.multiply(boundMax.x * scale.x).movePoint(this->pos);
    planes.push_back(Plane(pos, dir, (boundMax.z-boundMin.z) * scale.z, (boundMax.y-boundMin.y) * scale.y));
    pos = dir.multiply(boundMin.x * scale.x).movePoint(this->pos);
    planes.push_back(Plane(pos, dir.multiply(-1.0), (boundMax.z-boundMin.z) * scale.z, (boundMax.y-boundMin.y) * scale.y));

    dir = Vec3D(0,1,0).rotate(Vec3D(this->rot.x,0,0)).rotate(Vec3D(0,this->rot.y,0)).rotate(Vec3D(0,0,this->rot.z));
    pos = dir.multiply(boundMax.y * scale.y).movePoint(this->pos);
    planes.push_back(Plane(pos, dir, (boundMax.x-boundMin.x) * scale.x, (boundMax.z-boundMin.z) * scale.z));
    pos = dir.multiply(boundMin.y * scale.y).movePoint(this->pos);
    planes.push_back(Plane(pos, dir.multiply(-1.0), (boundMax.x-boundMin.x) * scale.x, (boundMax.z-boundMin.z) * scale.z));

    dir = Vec3D(0,0,1).rotate(Vec3D(this->rot.x,0,0)).rotate(Vec3D(0,this->rot.y,0)).rotate(Vec3D(0,0,this->rot.z));
    pos = dir.multiply(boundMax.z * scale.z).movePoint(this->pos);
    planes.push_back(Plane(pos, dir, (boundMax.x-boundMin.x) * scale.x, (boundMax.y-boundMin.y) * scale.y));
    pos = dir.multiply(boundMin.z * scale.z).movePoint(this->pos);
    planes.push_back(Plane(pos, dir.multiply(-1.0), (boundMax.x-boundMin.x) * scale.x, (boundMax.y-boundMin.y) * scale.y));

    return planes;
}

float Object::rayIntersections(Vec3D dir, Point3D origin)
{
    std::vector<Plane> planes = calculatePlanes();
    float minDistance = INFINITY;

    for(long long unsigned int i = 0; i < planes.size(); i++)
    {   
        std::vector<Point3D> hit = planes[i].rayIntersects(dir, origin);
        if (hit.size() != 0)
        {
            minDistance = std::min(this->pos.distanceTo(hit[0]), minDistance);
        }
    }

    return minDistance;

}