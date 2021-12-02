#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>

class Material
{
public:
    Material();
    Material(float ambient[4], float diffuse[4], float specular[4], float shiny);
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shiny;
};

#endif