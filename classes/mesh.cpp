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

#include <fstream>
#include <iostream>
#include <mesh.h>
#include <face.h>
#include <meshPoint.h>

Mesh::Mesh()
{
    VBO = std::vector<Point3D>();
    NBO = std::vector<Point3D>();
    faces = std::vector<Face>();
    name = "";
}

Mesh::Mesh(std::vector<Point3D> VBO, std::vector<Point3D> NBO, std::vector<Face> faces)
{
    this->VBO = VBO;
    this->NBO = NBO;
    this->faces = faces;
    this->name = "";
}

void Mesh::draw(bool showNormals)
{
    for (size_t i = 0; i < faces.size(); i++)
    {   
        glBegin(GL_POLYGON);
        for (size_t j = 0; j < faces.at(i).meshPoints.size(); j++)
        {
            MeshPoint mp = faces.at(i).meshPoints.at(j);

            Point3D v = VBO.at(mp.vi);
            Point3D n = NBO.at(mp.ni);
            Vec3D normal = Vec3D::createVector(Point3D(0,0,0), n).normalize();
            glNormal3f(normal.x, normal.y, normal.z);
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
    }

    // draw normals for debug purposes if enabled (disabled by default)
    if (showNormals)
    {
        for (size_t i = 0; i < faces.size(); i++)
        {
            for (size_t j = 0; j < faces.at(i).meshPoints.size(); j++)
            {
                MeshPoint mp = faces.at(i).meshPoints.at(j);

                Point3D v = VBO.at(mp.vi);
                Point3D n = NBO.at(mp.ni);
                Vec3D normal = Vec3D::createVector(Point3D(0,0,0), n).normalize();
                Point3D end = normal.movePoint(v);

                glBegin(GL_LINES);
                glVertex3f(v.x, v.y, v.z);
                glVertex3f(end.x, end.y, end.z);
                glEnd();

            }
        }
    }
}

/*
https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
Used to split string by spaces
*/
Mesh Mesh::createFromOBJ(std::string filePath)
{
    std::vector<Point3D> verts;
    std::vector<Point3D> norms;
    std::vector<Face> faces;

    std::string line;
    std::ifstream file(filePath);
    if (file.is_open())
    {
        std::getline(file, line);
        while (std::getline(file, line))
        {
            long long unsigned int pos = 0;
            std::vector<std::string> tokens;
            std::string token;
            while ((pos = line.find(" ")) != std::string::npos)
            {
                token = line.substr(0, pos);
                tokens.push_back(token);
                line.erase(0, pos + 1);
            }
            tokens.push_back(line);

            std::string first = tokens.at(0);
            if (first.compare("v") == 0)
            {
                float x = std::stof(tokens.at(1));
                float y = std::stof(tokens.at(2));
                float z = std::stof(tokens.at(3));
                verts.push_back(Point3D(x, y, z));
            }
            else if (first.compare("vn") == 0)
            {
                float x = std::stof(tokens.at(1));
                float y = std::stof(tokens.at(2));
                float z = std::stof(tokens.at(3));
                norms.push_back(Point3D(x, y, z));
            }
            else if (first.compare("f") == 0)
            {
                std::vector<MeshPoint> meshPoints;
                for (long long unsigned int i = 1; i < tokens.size(); i++)
                {
                    long long unsigned int facePos = 0;
                    std::vector<std::string> faceTokens;
                    std::string faceToken;
                    std::string faceData = tokens.at(i);
                    while ((facePos = faceData.find("/")) != std::string::npos)
                    {
                        faceToken = faceData.substr(0, facePos);
                        faceTokens.push_back(faceToken);
                        faceData.erase(0, facePos + 1);
                    }
                    faceTokens.push_back(faceData);

                    int vi = std::stoi(faceTokens.at(0))-1;
                    int ni = std::stoi(faceTokens.at(2))-1;
                    meshPoints.push_back(MeshPoint(vi, ni));
                }
                faces.push_back(Face(meshPoints));
            }
        }
    }

    file.close();
    return Mesh(verts, norms, faces);
}