#include <material.h>
#include <iostream>

Material::Material()
{
    float a[4] = {0,0,0,1};    
    for(int i = 0; i < 4; i++)
    {
        ambient[i] = a[i];
        diffuse[i] = a[i];
        specular[i] = a[i];
    }
    shiny = 0;

}

Material::Material(float a[4], float d[4], float s[4], float shiny)
{     
    for(int i = 0; i < 4; i++)
    {
        ambient[i] = a[i];
        diffuse[i] = d[i];
        specular[i] = s[i];
    }
    this->shiny = shiny;
}