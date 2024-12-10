#ifdef _APPLE_
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#define BACKSPACE_KEY 127
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#define BACKSPACE_KEY 8
#else // Linux
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#define BACKSPACE_KEY 8
#endif

#include <file.h>
#include <string>
#include <vector>
#include <object.h>
#include <iostream>
#include <fstream>
#include <tuple>

File::File()
{
    fileName = "";
}

File::File(std::string fileName)
{
    this->fileName = fileName;
}

void File::saveToFile(std::vector<Object> objects)
{
    std::ofstream file (fileName);
    for(long long unsigned int i = 0; i < objects.size(); i++)
    {
        Object obj = objects.at(i);

        file << obj.pos.x << "," << obj.pos.y << "," << obj.pos.z << "|";
        file << obj.rot.x << "," << obj.rot.y << "," << obj.rot.z << "|";
        file << obj.shape << "|";
        file << obj.scale.x << "," << obj.scale.y << "," << obj.scale.z << "|";
        file << obj.boundMin.x << "," << obj.boundMin.y << "," << obj.boundMin.z << "|";
        file << obj.boundMax.x << "," << obj.boundMax.y << "," << obj.boundMax.z << "|";
        file << obj.matIndex << "|";
        file << "\n";
    }

    file.close();
}

std::vector<std::string> split(const char *str, char c = ' ')
{
    std::vector<std::string> result;
    do
    {
        const char *begin = str;

        while(*str != c && *str)
            str++;

        result.push_back(std::string(begin, str));
    } while (0 != *str++);

    return result;
}

std::tuple<std::vector<Object>, bool> File::loadFromFile()
{
    std::vector<Object> objects;
    std::string line;      
    std::ifstream file (fileName);   
    if(file.fail())  
    { 
        return std::make_tuple(objects, true);
    }  else {        
        if(file.is_open())
        {
            while(getline(file, line))
            {                            
                std::vector<std::string> info = split(line.c_str(), '|');
                std::vector<std::string> pos = split(info.at(0).c_str(), ',');
                std::vector<std::string> rot = split(info.at(1).c_str(), ',');
                std::string shape = info.at(2);
                std::vector<std::string> scale = split(info.at(3).c_str(), ',');
                std::vector<std::string> boundMin = split(info.at(4).c_str(), ',');
                std::vector<std::string> boundMax = split(info.at(5).c_str(), ',');
                std::string matIndex = info.at(6);

                Point3D posPoint = Point3D(atoi(pos.at(0).c_str()), atoi(pos.at(1).c_str()), atoi(pos.at(2).c_str()));
                Vec3D rotVec = Vec3D(atoi(rot.at(0).c_str()), atoi(rot.at(1).c_str()), atoi(rot.at(2).c_str()));      
                int shapeIndex = atoi(shape.c_str());
                Vec3D scaleVec = Vec3D(atoi(scale.at(0).c_str()), atoi(scale.at(1).c_str()), atoi(scale.at(2).c_str()));      
                Point3D boundMinPoint = Point3D(atoi(boundMin.at(0).c_str()), atoi(boundMin.at(1).c_str()), atoi(boundMin.at(2).c_str()));
                Point3D boundMaxPoint = Point3D(atoi(boundMax.at(0).c_str()), atoi(boundMax.at(1).c_str()), atoi(boundMax.at(2).c_str()));      
                int matIndexInt = atoi(matIndex.c_str());            

                Object obj = Object(posPoint, rotVec, Object::Shape(shapeIndex), scaleVec, boundMinPoint, boundMaxPoint, matIndexInt);            
                objects.push_back(obj);
            }        
            file.close();
        }         
    }
    return std::make_tuple(objects, false);
}