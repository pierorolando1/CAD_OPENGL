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

// Header files
#include <iostream>
#include <random>
#include <vector>
#include <cfloat>
#include <cmath>
#include <object.h>
#include <material.h>
#include <file.h>
#include <plane.h>
#include <PPM.h>
#include <light.h>

// Global variables

std::string text = ".txt";

// Window globals
GLint mainWindowId, saveWindowId, loadWindowId, errorWindowId;

float GLOBAL_WIDTH = 800;
float GLOBAL_HEIGHT = 800;

// Camera globals
float camera1Angle = 0;
Vec3D camera1Start = Vec3D(0,40,60);
float camera1[3] = {camera1Start.x, camera1Start.y, camera1Start.z};

float cameraDirX = 0;
float cameraDirY = 0;
float cameraDirZ = 0;

float globalMatIndex = 0;
bool saving, loading = false;
std::string fileName = "";
File save;
File load;
std::vector<Object> objects;
Vec3D mouseDir = Vec3D(0,1,0);

// Texture globals
GLubyte* stone_img;
int stoneWidth, stoneHeight, stoneMax;

GLubyte* marble_img;
int marbleWidth, marbleHeight, marbleMax;

GLubyte* wall_img;
int wallWidth, wallHeight, wallMax;

GLuint textures[3];

// Scene + camera movement globals
int sceneRotation[2] = {0,0}; //[0] is y, [1] is z

bool cameraMovementMode = false;
int cameraXRot = 0;
int cameraYRot = 0;
Vec3D camDir = Vec3D(1,0,0);

Point3D testPoint = Point3D(100,100,100);

// Plane floorPlane = Plane(Point3D(0,5,0), Vec3D(0,1,1).normalize(), 100.0, 100.0);
Plane floorPlane = Plane(Point3D(), Vec3D(0,1,0), 100.0, 80.0);

// Light globals
Light light0, light1;


float lightPos[2][4] = {
    {-30, 25, 5, 1},
    {30, 25, 5, 1}};

float lightAmb[2][4] = {
    {0.5, 0.5, 0.5, 1},
    {0.1, 0.1, 0.1, 1}};

float lightDiff[2][4] = {
    {1, 1, 1, 1},
    {1, 1, 1, 1}};

float lightSpec[2][4] = {
    {1, 1, 1, 1},
    {1, 1, 1, 1}};    

// Material globals
float materials[5][4][4] = {
    {{0.0f, 0.1f, 0.06f, 1.0f}, //cyan plastic
     {0.0f, 0.50980392f, 0.50980392f, 1.0f},
     {0.50196078f, 0.50196078f, 0.50196078f, 1.0f}},
    {{0.1745f, 0.01175f, 0.01175f, 0.55f}, //ruby
     {0.61424f, 0.04136f, 0.04136f, 0.55f},
     {0.727811f, 0.626959f, 0.626959f, 0.55f}},
    {{0.0215f, 0.1745f, 0.0215f, 0.55f}, //emerald
     {0.07568f, 0.61424f, 0.07568f, 0.55f},
     {0.633f, 0.727811f, 0.633f, 0.55f}},
    {{0.05375f, 0.05f, 0.06625f, 0.82f}, //obsidian
     {0.18275f, 0.17f, 0.22525f, 0.82f},
     {0.332741f, 0.328634f, 0.346435f, 0.82f}},
     {{0.2125f, 0.1275f, 0.054f, 1.0f}, // bronze
      {0.714f, 0.4284f, 0.18144f, 1.0f},
      {0.393548f, 0.271906f, 0.166721f, 1.0f}}
};

Material mats[5] = {
    Material(materials[0][0], materials[0][1], materials[0][2], 10),
    Material(materials[1][0], materials[1][1], materials[1][2], 10),
    Material(materials[2][0], materials[2][1], materials[2][2], 10),
    Material(materials[3][0], materials[3][1], materials[3][2], 10),
    Material(materials[4][0], materials[4][1], materials[4][2], 25.6)};   

// Draws floor in scene
void drawFloor()
{
    float scaleWidth = floorPlane.width/2;
    float scaleHeight = floorPlane.height/2;
    glPushMatrix();
    glTranslatef(floorPlane.pos.x, floorPlane.pos.y, floorPlane.pos.z);
    glNormal3f(0,1,0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glBegin(GL_POLYGON);
    glColor3f(0.3, 0.3, 1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mats[3].ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mats[3].diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mats[3].specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SPECULAR, mats[3].shiny);
    glTexCoord2f(0,0);
    glVertex3f(-scaleWidth,0,-scaleHeight);
    glTexCoord2f(1,1);
    glVertex3f(-scaleWidth,0,scaleHeight);
    glTexCoord2f(1,0);
    glVertex3f(scaleWidth,0,scaleHeight);
    glTexCoord2f(0,1);
    glVertex3f(scaleWidth,0,-scaleHeight);
    glEnd();

    glPopMatrix();
}

// Draws walls in scene
void drawWalls()
{
    Plane xPlane = Plane(Point3D(), Vec3D(1,0,0), floorPlane.height, 25.0);
    Plane zPlane = Plane(Point3D(), Vec3D(0,0,1), floorPlane.width, 25.0);

    float xFloorOffest = floorPlane.width/2.0;
    float zFloorOffest = floorPlane.height/2.0;

    glPushMatrix();
    glNormal3f(xPlane.normal.x, xPlane.normal.y, xPlane.normal.z);
    glTranslatef(-xFloorOffest,xPlane.height/2.0,0);
    glRotatef(-90, 0,0,1);
    glRotatef(90, 0,1,0);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
    glVertex3f(-xPlane.width/2.0, 0, -xPlane.height/2.0);
    glTexCoord2f(1,0);
    glVertex3f(-xPlane.width/2.0, 0, xPlane.height/2.0);
    glTexCoord2f(0,1);
    glVertex3f(xPlane.width/2.0, 0, xPlane.height/2.0);
    glTexCoord2f(1,1);
    glVertex3f(xPlane.width/2.0,0, -xPlane.height/2.0);
    glEnd();
    glPopMatrix();

    xPlane.normal = xPlane.normal.multiply(-1);
    glPushMatrix();
    glNormal3f(xPlane.normal.x, xPlane.normal.y, xPlane.normal.z);
    glTranslatef(xFloorOffest,xPlane.height/2.0,0);
    glRotatef(90, 0,0,1);
    glRotatef(90, 0,1,0);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
    glVertex3f(-xPlane.width/2.0, 0, -xPlane.height/2.0);
    glTexCoord2f(1,0);
    glVertex3f(-xPlane.width/2.0, 0, xPlane.height/2.0);
    glTexCoord2f(0,1);
    glVertex3f(xPlane.width/2.0, 0, xPlane.height/2.0);
    glTexCoord2f(1,1);
    glVertex3f(xPlane.width/2.0,0, -xPlane.height/2.0);
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glNormal3f(zPlane.normal.x, zPlane.normal.y, zPlane.normal.z);
    glTranslatef(0,zPlane.height/2.0,-zFloorOffest);
    glRotatef(90, 1,0,0);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
    glVertex3f(-zPlane.width/2.0, 0, -zPlane.height/2.0);
    glTexCoord2f(1,0);
    glVertex3f(-zPlane.width/2.0, 0, zPlane.height/2.0);
    glTexCoord2f(0,1);
    glVertex3f(zPlane.width/2.0, 0, zPlane.height/2.0);
    glTexCoord2f(1,1);
    glVertex3f(zPlane.width/2.0,0, -zPlane.height/2.0);
    glEnd();
    glPopMatrix();
    
    zPlane.normal = zPlane.normal.multiply(-1);
    glPushMatrix();
    glNormal3f(zPlane.normal.x, zPlane.normal.y, zPlane.normal.z);
    glTranslatef(0,zPlane.height/2.0,zFloorOffest);
    glRotatef(-90, 1,0,0);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
    glVertex3f(-zPlane.width/2.0, 0, -zPlane.height/2.0);
    glTexCoord2f(1,0);
    glVertex3f(-zPlane.width/2.0, 0, zPlane.height/2.0);
    glTexCoord2f(0,1);
    glVertex3f(zPlane.width/2.0, 0, zPlane.height/2.0);
    glTexCoord2f(1,1);
    glVertex3f(zPlane.width/2.0,0, -zPlane.height/2.0);
    glEnd();
    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, textures[1]); 
}

void drawRay(Point3D pos, Vec3D dir, float size)
{
    Point3D end = dir.multiply(size).movePoint(pos);
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glLineWidth(10);
    glBegin(GL_LINES);
    glColor3f(0,0,1);
    glVertex3f(pos.x, pos.y, pos.z);
    glColor3f(1,0,0);
    glVertex3f(end.x, end.y, end.z);
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawLights()
{   
    float clearEmis[4] = {0, 0, 0, 1};
    float lightEmis[4] = {1, 1, 1, 1};
    // light 0
    glLightfv(GL_LIGHT0, GL_POSITION, light0.pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0.amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0.diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0.spec);
    glColor3f(1, 1, 1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, lightEmis);
    glPushMatrix();
    glTranslatef(light0.pos[0], light0.pos[1], light0.pos[2]);
    glutSolidSphere(1, 16, 16);    
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, clearEmis);
    if (light0.selected)
    {
        glDisable(GL_LIGHTING);
        glColor3f(0,1,0);
        glutWireSphere(1,16,16);
        glEnable(GL_LIGHTING);

    }
    glPopMatrix();

    // light 1
    glLightfv(GL_LIGHT1, GL_POSITION, light1.pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1.amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1.diff);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1.spec);
    glColor3f(1, 1, 1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, lightEmis);
    glPushMatrix();
    glTranslatef(light1.pos[0], light1.pos[1], light1.pos[2]);
    glutSolidSphere(1, 16, 16);    
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, clearEmis);
    if (light1.selected)
    {
        glDisable(GL_LIGHTING);
        glColor3f(0,1,0);
        glutWireSphere(1,16,16);
        glEnable(GL_LIGHTING);

    }
    glPopMatrix();
}

// Draw individual object with rotation, position, material, selected information
void drawObject(Object object)
{
    glPushMatrix();
    glTranslatef(object.pos.x, object.pos.y, object.pos.z);
    glRotatef(object.rot.x, 1,0,0);
    glRotatef(object.rot.y, 0,1,0);
    glRotatef(object.rot.z, 0,0,1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mats[object.matIndex].ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mats[object.matIndex].diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mats[object.matIndex].specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mats[object.matIndex].shiny);
    object.draw();
    glPopMatrix();
    if(object.selected)
    {
        object.drawBounds();
    }
}

// Loop through all objects in the scene and draw them
void drawObjects()
{
    for(long long unsigned int i = 0; i < objects.size(); i++)
    {
        drawObject(objects.at(i));
    }
}

// Function to select newly drawn object (deselects all other objects in scene)
void selectNewObject()
{
    for(long long unsigned int i = 0; i < objects.size(); i++)
    {
        objects.at(i).selected = false;
    }
}

// Function to apply global material to the selected object
void applyGlobalMaterial()
{
    for(long long unsigned int i = 0; i < objects.size(); i++)
    {
        if(objects.at(i).selected == true)
        {
            objects.at(i).matIndex = globalMatIndex;
        }
    }
}

// Function to translate the selected object on the specified axis
void translateSelectedObject(unsigned char key)
{
    for(long long unsigned int i = 0; i < objects.size(); i++)
    {
        Object obj = objects[i];
        if(objects[i].selected == true)
        {
            switch(key)
            {
                case 'x':
                    if(obj.pos.x < floorPlane.width/2)
                    {
                        objects[i].pos.x += 1;
                    }
                    break;
                case 'X':
                    if(obj.pos.x > -floorPlane.width/2)
                    {
                        objects[i].pos.x -= 1;
                    }                    
                    break;
                case 'y':
                    objects[i].pos.y += 1;
                    break;
                case 'Y':
                    if(obj.pos.y > floorPlane.pos.y)
                    {
                        objects[i].pos.y -= 1;
                    }
                    break;
                case 'z':
                    if(obj.pos.z < floorPlane.height/2)
                    {
                        objects[i].pos.z += 1;
                    }
                    break;
                case 'Z':
                    if(obj.pos.z > -floorPlane.height/2)
                    {
                        objects[i].pos.z -= 1;
                    }   
                    break;                                        
            }
        }
    }

    Light light = light0;
    for(int i = 0; i < 2; i++)
    {
        if(light.selected == true)
        {
            switch(key)
            {
                case 'x':
                    if(light.pos[0] < floorPlane.width/2)
                    {
                        light.pos[0] += 1;
                    }
                    break;
                case 'X':
                    if(light.pos[0] > -floorPlane.width/2)
                    {
                        light.pos[0] -= 1;
                    }                    
                    break;
                case 'y':
                    light.pos[1] += 1;
                    break;
                case 'Y':
                    if(light.pos[1] > floorPlane.pos.y)
                    {
                        light.pos[1] -= 1;
                    }
                    break;
                case 'z':
                    if(light.pos[2] < floorPlane.height/2)
                    {
                        light.pos[2] += 1;
                    }
                    break;
                case 'Z':
                    if(light.pos[2] > -floorPlane.height/2)
                    {
                        light.pos[2] -= 1;
                    }   
                    break;                                        
            }
        }
        light = light1;
    }
}

// Function to scale the selected object on the specified axis
void scaleSelectedObject(unsigned char key)
{
    for(long long unsigned int i = 0; i < objects.size(); i++)
    {
        if(objects.at(i).selected == true)
        {
            switch(key)
            {
                case 24:
                    if(!(glutGetModifiers() & GLUT_ACTIVE_SHIFT))
                    {
                        objects.at(i).scale.x += 1;
                    }
                    if(glutGetModifiers() & GLUT_ACTIVE_SHIFT)
                    {
                        objects.at(i).scale.x -= 1;
                    }
                    break;
                case 25:
                    if(!(glutGetModifiers() & GLUT_ACTIVE_SHIFT))
                    {
                        objects.at(i).scale.y += 1;
                    }
                    if(glutGetModifiers() & GLUT_ACTIVE_SHIFT)
                    {
                        objects.at(i).scale.y -= 1;
                    }                  
                    break;
                case 26:
                    if(!(glutGetModifiers() & GLUT_ACTIVE_SHIFT))
                    {
                        objects.at(i).scale.z += 1;
                    }
                    if(glutGetModifiers() & GLUT_ACTIVE_SHIFT)
                    {
                        objects.at(i).scale.z -= 1;
                    }
                    break;                                      
            }
        }
    }
}

// Function to rotate the selected object on the specified axis
void rotateSelectedObject(unsigned char key)
{
    for(long long unsigned int i = 0; i < objects.size(); i++)
    {
        if(objects.at(i).selected == true)
        {
            switch(key)
            {
                case 'x':
                    if(!(glutGetModifiers() & GLUT_ACTIVE_SHIFT))
                    {
                        objects.at(i).rot.x += 10;
                    }
                    break;
                case 'X':
                    if((glutGetModifiers() & GLUT_ACTIVE_SHIFT))
                    {
                        objects.at(i).rot.x -= 10;
                    }
                    break;
                case 'y':
                    if(!(glutGetModifiers() & GLUT_ACTIVE_SHIFT))
                    {
                        objects.at(i).rot.y += 10;
                    }
                    break;
                case 'Y':
                    if((glutGetModifiers() & GLUT_ACTIVE_SHIFT))
                    {
                        objects.at(i).rot.y -= 10;
                    }
                    break;        
                case 'z':
                    if(!(glutGetModifiers() & GLUT_ACTIVE_SHIFT))
                    {
                        objects.at(i).rot.z += 10;
                    }
                    break;
                case 'Z':
                    if((glutGetModifiers() & GLUT_ACTIVE_SHIFT))
                    {
                        objects.at(i).rot.z -= 10;
                    }
                    break;                                                   
            }
        }
    }
}
// Display function
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(cameraXRot, 1, 0, 0);
    glRotatef(cameraYRot, 0, 1, 0);    
    gluLookAt(camera1[0], camera1[1], camera1[2], cameraDirX, cameraDirY, cameraDirZ, 0, 1, 0);

    glPushMatrix();
    glRotatef(sceneRotation[0], 0, 1, 0);
    glRotatef(sceneRotation[1], 0, 0, 1);    

    drawLights();

    drawFloor();
    drawWalls();
    drawObjects();

    glPopMatrix();

    glutSwapBuffers();
}

// Function that renders text onto the screen as a bitmap at the given (x,y) position in the given font
void renderText(float x, float y, void *font, const char *string) 
{  
    const char *character;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glRasterPos2f(x,y);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (character=string; *character != '\0'; character++) 
    {
        glutBitmapCharacter(font, *character);
    }

    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// Display function for the save screen
void saveDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();       
    glColor3f(1,1,1);
    renderText(-0.70, 0.70, GLUT_BITMAP_TIMES_ROMAN_24, "Enter the name of the file");
    renderText(-0.70, 0.60, GLUT_BITMAP_TIMES_ROMAN_24, "you would like to save to:");
    renderText(-0.50, -0.40, GLUT_BITMAP_TIMES_ROMAN_24, fileName.c_str());    
    glPopMatrix();

    glutSwapBuffers();
}

// Display function for the load screen
void loadDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glColor3f(1,1,1);    
    renderText(-0.70, 0.70, GLUT_BITMAP_TIMES_ROMAN_24, "Enter the name of the file you");
    renderText(-0.70, 0.60, GLUT_BITMAP_TIMES_ROMAN_24, "you would like to load:");
    renderText(-0.50, -0.40, GLUT_BITMAP_TIMES_ROMAN_24, fileName.c_str());    
    glPopMatrix();

    glutSwapBuffers();
}

// Display function for the error screen
void errorDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glColor3f(1,1,1);    
    renderText(-0.70, 0.70, GLUT_BITMAP_TIMES_ROMAN_24, "The file name you have entered");
    renderText(-0.70, 0.60, GLUT_BITMAP_TIMES_ROMAN_24, "cannot be found. Please try");
    renderText(-0.70, 0.50, GLUT_BITMAP_TIMES_ROMAN_24, "loading another file name");
    renderText(-0.70, 0.40, GLUT_BITMAP_TIMES_ROMAN_24, "(Press ENTER to continue)");     
    glPopMatrix();

    glutSwapBuffers();
}

// Timer function for main screen
void timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(17, timer, 0);
}

// Function that saves the scene to a file
void saveFile()
{  
    save = File(fileName);
    save.saveToFile(objects);
}

// Function that loads scene from a file
void loadFile()
{    
    load = File(fileName);
    std::tuple<std::vector<Object>, bool> loadedObjects = load.loadFromFile(); 
    bool fileError = std::get<1>(loadedObjects);
    if(fileError)
    {
        glutSetWindow(errorWindowId);
        glutShowWindow();
    }
    else 
    {
        objects = std::get<0>(loadedObjects);
    }
}

// Timer function for save screen
void saveTimer(int value)
{    
    glutPostRedisplay();
    glutTimerFunc(17, saveTimer, 0);
}

// Keyboard function for save screen
void saveKeyboard(unsigned char key, int x, int y) 
{
    if(saving)
    {        
        if(key == BACKSPACE_KEY) {            
            if(fileName.length() != 0)
            {
                fileName.pop_back();
            }
            
        } else {
            fileName += key;
        }        
        glutPostRedisplay();
    }

    switch(key)
    {
        case 13:
            glutHideWindow();
            glutSetWindow(mainWindowId);
            std::cout << fileName << std::endl;
            fileName += text;
            std::cout << fileName << std::endl;
            saveFile();
            saving = false;            
            fileName = "";
    }
}

// Timer function for load screen
void loadTimer(int value)
{    
    glutPostRedisplay();
    glutTimerFunc(17, loadTimer, 0);
}

// Keyboard function for load screen
void loadKeyboard(unsigned char key, int x, int y) 
{
    if(loading)
    {
        if(key == BACKSPACE_KEY) {            
            if(fileName.length() != 0)
            {
                fileName.pop_back();
            }
        } else {
            fileName += key;
        }  
    }

    std::string text = ".txt";
    switch(key)
    {
        case 13:
            glutHideWindow();
            glutSetWindow(mainWindowId);
            fileName += text;
            loadFile();
            loading = false;
            fileName = "";
    }
}

// Timer function for error screen
void errorTimer(int value)
{    
    glutPostRedisplay();
    glutTimerFunc(17, errorTimer, 0);
}

// Keyboard function for error screen
void errorKeyboard(unsigned char key, int x, int y) 
{
    switch(key)
    {
        case 13:
            glutHideWindow();
            glutSetWindow(mainWindowId);
            std::cout << objects.size() << "\n";
            loading = false;
            fileName = "";
    }
}

// Keyboard function for main screen
void keyboard(unsigned char key, int x, int y)
{
    Object obj;
    Object obj1;
    switch (key)
    {
    case 'q':
    case 'Q':
        exit(0);
        break;    
    case '-':        
        cameraMovementMode = !cameraMovementMode;
        break;
    case '1':
        globalMatIndex = 0;
        break;
    case '2':
        globalMatIndex = 1;
        break;        
    case '3':
        globalMatIndex = 2;
        break;
    case '4':
        globalMatIndex = 3;
        break;
    case '5':
        globalMatIndex = 4;
        break;                               
    case 'r':
        objects.clear();
        break;
    case 'v':
        obj = Object(Point3D(0,10,0), Vec3D(0,0,0), Object::CUBE, Vec3D(3,3,3), Point3D(-1,-1,-1), Point3D(1,1,1), globalMatIndex);
        selectNewObject();
        obj.selected = true;
        objects.push_back(obj);
        break;
    case 'b':
        obj = Object(Point3D(0,10,0), Vec3D(0,0,0), Object::SPHERE, Vec3D(3,3,3), Point3D(-1,-1,-1), Point3D(1,1,1), globalMatIndex);
        selectNewObject();
        obj.selected = true;
        objects.push_back(obj);
        break;   
    case 'n':    
        obj = Object(Point3D(0,10,0), Vec3D(0,0,0), Object::CONE, Vec3D(3,3,3), Point3D(-1,-1,-1), Point3D(1,1,1), globalMatIndex);
        selectNewObject();
        obj.selected = true;
        objects.push_back(obj);
        break;      
    case 'm':
        obj = Object(Point3D(0,10,0), Vec3D(0,0,0), Object::TEAPOT, Vec3D(3,3,3), Point3D(-1,-1,-1), Point3D(1,1,1), globalMatIndex);
        selectNewObject();
        obj.selected = true;                   
        objects.push_back(obj);        
        break;   
    case ',':
        obj = Object(Point3D(0,10,0), Vec3D(0,0,0), Object::TETRAHEDRON, Vec3D(3,3,3), Point3D(-1,-1,-1), Point3D(1,1,1), globalMatIndex);
        selectNewObject();
        obj.selected = true;            
        objects.push_back(obj);
        break;          
    case '.': // bed
        obj1 = Object(Point3D(0,10,0), Vec3D(0,0,0), Object::BED, Vec3D(3,3,3), Point3D(-1,-1,-1), Point3D(1,1,1), globalMatIndex);
        selectNewObject();
        obj1.selected = true;
        objects.push_back(obj1);
        break;
    case '/':
        obj = Object(Point3D(0,10,0), Vec3D(0,0,0), Object::CUSTOM, Vec3D(3,3,3), Point3D(-1,-1,-1), Point3D(1,1,1), globalMatIndex);
        selectNewObject();
        obj.selected = true;        
        objects.push_back(obj);
        break;
    case 's':
        saving = true;
        glutSetWindow(saveWindowId);
        glutShowWindow();
        break;
    case 'l':
        loading = true;
        glutSetWindow(loadWindowId);
        glutShowWindow();
        break;
    case 'i':
        if(cameraMovementMode) {
            camera1[0] -= 1;
            break;
        }
    case 'o':
        if(cameraMovementMode) {
            camera1[0] += 1;
            break;
        }        
    case 'p':
        if(cameraMovementMode) {
            camera1[1] -= 1;
            break;
        }        
    case '[':
        if(cameraMovementMode) {
            camera1[1] += 1;
            break;
        }        
    case ']':
        if(cameraMovementMode) {
            camera1[2] -= 1;
            break;
        }
    case 92:
        if(cameraMovementMode) {
            camera1[2] += 1;
            break;
        }     
    case 'M':
        applyGlobalMaterial();        
    case 'x':
    case 'X':
    case 'y':
    case 'Y':
    case 'z':
    case 'Z':
        if(glutGetModifiers() & GLUT_ACTIVE_ALT)
        {
            rotateSelectedObject(key);
        } else {
            translateSelectedObject(key);
        }
        break;  
    case 24:
    case 25:
    case 26:
        scaleSelectedObject(key);
        break;
    }
}

// Special keyboard function
void special(int key, int x, int y)
{
    Vec3D newPos;
    switch (key)
    {
        case GLUT_KEY_RIGHT:
            camera1Angle+=2;
            newPos = camera1Start.rotate(Vec3D(0,camera1Angle,0));
            camera1[0] = newPos.x;
            camera1[1] = newPos.y;
            camera1[2] = newPos.z;      
            break;
        case GLUT_KEY_LEFT:
            camera1Angle-=2;
            newPos = camera1Start.rotate(Vec3D(0,camera1Angle,0));
            camera1[0] = newPos.x;
            camera1[1] = newPos.y;
            camera1[2] = newPos.z;
            break;            
        case GLUT_KEY_UP:
            sceneRotation[1] -= 5;
            sceneRotation[1] = sceneRotation[1] % 360;    
            break;
        case GLUT_KEY_DOWN:
            sceneRotation[1] += 5;
            sceneRotation[1] = sceneRotation[1] % 360;        
            break;
    }
}

// Mouse function
void mouse(int button, int state, int x, int y)
{
    y = glutGet(GLUT_WINDOW_HEIGHT) - y;
    if (state == GLUT_UP && button == GLUT_LEFT_BUTTON)
    {
        double endX, endY, endZ;
        double model[16], proj[16];
        int view[4];
        glGetDoublev(GL_MODELVIEW_MATRIX, model);
        glGetDoublev(GL_PROJECTION_MATRIX, proj);
        glGetIntegerv(GL_VIEWPORT, view);

        gluUnProject(x, y, 1.0, model, proj, view, &endX, &endY, &endZ);
        mouseDir = Vec3D::createVector(Point3D(camera1[0], camera1[1], camera1[2]), Point3D(endX, endY, endZ)).normalize();

        int minIndex = -1;
        int light = -1;
        float minDistance = INFINITY;

        for(long long unsigned int i = 0; i < objects.size(); i++)
        {
            objects[i].selected = false;
            float dist = objects[i].rayIntersections(mouseDir, Point3D(camera1[0], camera1[1], camera1[2]));
            if (dist < minDistance)
            {
                minDistance = dist;
                minIndex = i;
            }
        }

        light0.selected = false;
        if (light0.rayIntersects(mouseDir, Point3D(camera1[0], camera1[1], camera1[2])))
        {
            float d = Point3D(camera1[0], camera1[1], camera1[2]).distanceTo(Point3D(light0.pos[0],light0.pos[1],light0.pos[2]));
            if (d < minDistance)
            {
                minDistance = d;
                minIndex = -1;
                light = 0;
            }
        }

        light1.selected = false;
        if (light1.rayIntersects(mouseDir, Point3D(camera1[0], camera1[1], camera1[2])))
        {
            float d = Point3D(camera1[0], camera1[1], camera1[2]).distanceTo(Point3D(light1.pos[0],light1.pos[1],light1.pos[2]));
            if (d < minDistance)
            {
                minDistance = d;
                minIndex = -1;
                light = 1;
            }
        }

        if (minIndex != -1)
        {
            objects[minIndex].selected = true;
        }
        if (light == 0)
        {
            light0.selected = true;
        }
        else if (light == 1)
        {
            light1.selected = true;
        }

    }
    
    else if (state == GLUT_UP && button == GLUT_RIGHT_BUTTON)
    {
        double endX, endY, endZ;
        double model[16], proj[16];
        int view[4];
        glGetDoublev(GL_MODELVIEW_MATRIX, model);
        glGetDoublev(GL_PROJECTION_MATRIX, proj);
        glGetIntegerv(GL_VIEWPORT, view);

        gluUnProject(x, y, 1.0, model, proj, view, &endX, &endY, &endZ);
        mouseDir = Vec3D::createVector(Point3D(camera1[0], camera1[1], camera1[2]), Point3D(endX, endY, endZ)).normalize();

        int minIndex = -1;
        float minDistance = INFINITY;

        for(long long unsigned int i = 0; i < objects.size(); i++)
        {
            float dist = objects[i].rayIntersections(mouseDir, Point3D(camera1[0], camera1[1], camera1[2]));
            if (dist < minDistance)
            {
                minDistance = dist;
                minIndex = i;
            }
        }

        if (minIndex != -1)
        {
            objects.erase(objects.begin() + minIndex);
        }
    }
}

// Passive mouse function
void passiveMouse(int x, int y)
{
    y = GLOBAL_HEIGHT - y;
    if(cameraMovementMode)
    {
        sceneRotation[0] = x;
        sceneRotation[1] = y;
    }
}

// Reshape function
void reshape(int w, int h)
{
    GLOBAL_WIDTH = w;
    GLOBAL_HEIGHT = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)w / h, 1, 500);

    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);
}

// Initialize glut 
void initGlut()
{

    glutInitWindowSize(400, 400);  
    glutInitWindowPosition(2000, 50);
    errorWindowId = glutCreateWindow("File Error");
    glutSetWindow(errorWindowId);
    glutHideWindow();
    glutDisplayFunc(errorDisplay);
    glutKeyboardFunc(errorKeyboard);
    glutTimerFunc(17, errorTimer, 0);

    glutInitWindowSize(400, 400);  
    glutInitWindowPosition(2000, 50);
    loadWindowId = glutCreateWindow("Load File");
    glutSetWindow(loadWindowId);
    glutHideWindow();
    glutDisplayFunc(loadDisplay);
    glutKeyboardFunc(loadKeyboard);
    glutTimerFunc(17, loadTimer, 0);

    glutInitWindowSize(400, 400);  
    glutInitWindowPosition(2000, 50);
    saveWindowId = glutCreateWindow("Save File");
    glutSetWindow(saveWindowId);
    glutHideWindow();
    glutDisplayFunc(saveDisplay);
    glutKeyboardFunc(saveKeyboard);
    glutTimerFunc(17, saveTimer, 0);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(GLOBAL_WIDTH, GLOBAL_HEIGHT);    
    mainWindowId = glutCreateWindow("3D CAD System");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(passiveMouse);
    glutTimerFunc(17, timer, 0);
    glutFullScreen();     
}

// Print all keyboard commands
void printCommands () {
    std::cout << "q and Q - Quit Program" << "\n";
    std::cout << "'-' - Toggle camera movement mode on and off" << "\n";
    std::cout << "1 - Change global material to material 1" << "\n";
    std::cout << "2 - Change global material to material 2" << "\n";
    std::cout << "3 - Change global material to material 3" << "\n";
    std::cout << "4 - Change global material to material 4" << "\n";
    std::cout << "5- Change global material to material 5" << "\n";
    std::cout << "r - Reset the scene" << "\n";
    std::cout << "v - Draw a new cube at the origin" << "\n";
    std::cout << "b - Draw a new sphere at the origin" << "\n";
    std::cout << "n - Draw a new cone at the origin" << "\n";
    std::cout << "m - Draw a new teapot at the origin" << "\n";
    std::cout << ", - Draw a new tetrahedron at the origin" << "\n";
    std::cout << ". - Draw a new icosahedron at the origin" << "\n";
    std::cout << "/ - Draw a new snowman at the origin (bonus feature)" << "\n";
    std::cout << "s - Save the current scene to a file" << "\n";
    std::cout << "l - Load a scene from a previously saved file" << "\n";
    std::cout << "i - Move camera in the negative X direction (if camera movement mode enabled)" << "\n";
    std::cout << "o - Move camera in the positive X direction (if camera movement mode enabled)" << "\n";
    std::cout << "p - Move camera in the negative Y direction (if camera movement mode enabled)" << "\n";
    std::cout << "[ - Move camera in the positive Y direction (if camera movement mode enabled)" << "\n";
    std::cout << "] - Move camera in the negative Z direction (if camera movement mode enabled)" << "\n";
    std::cout << "\\ - Move camera in the positive Z direction (if camera movement mode enabled)" << "\n";
    std::cout << "M - Apply global material to selected object" << "\n";
    std::cout << "x - Move selected object in positive X direction" << "\n";
    std::cout << "X - Move selected object in negative X direction" << "\n";
    std::cout << "y - Move selected object in positive Y direction" << "\n";
    std::cout << "Y - Move selected object in negative Y direction" << "\n";
    std::cout << "z - Move selected object in positive Z direction" << "\n";
    std::cout << "Z - Move selected object in negative Z direction" << "\n";
    std::cout << "Ctrl + x - Scale selected object in positive X direction" << "\n";
    std::cout << "Ctrl + Shift + x - Scale selected object in negative X direction" << "\n";
    std::cout << "Ctrl + y - Scale selected object in positive Y direction" << "\n";
    std::cout << "Ctrl + Shift + y - Scale selected object in negative Y direction" << "\n";
    std::cout << "Ctrl + z - Scale selected object in positive Z direction" << "\n";
    std::cout << "Ctrl + Shift + z - Scale selected object in negative Z direction" << "\n";
    std::cout << "Alt + x - Rotate selected object in positive X direction" << "\n";
    std::cout << "Alt + Shift + x - Rotate selected object in negative X direction" << "\n";
    std::cout << "Alt + y - Rotate selected object in positive Y direction" << "\n";
    std::cout << "Alt + Shift + y - Rotate selected object in negative Y direction" << "\n";
    std::cout << "Alt + z - Rotate selected object in positive Z direction" << "\n";
    std::cout << "Alt + Shift + z - Rotate selected object in negative Z direction" << "\n";
    std::cout << "UP_ARROW - Rotate entire scene negative 5 units on the Z axis" << "\n";
    std::cout << "DOWN_ARROW - Rotate entire scene positive 5 units on the Z axis" << "\n";
    std::cout << "RIGHT_ARROW - Rotate entire scene negative 5 units on the Y axis" << "\n";
    std::cout << "LEFT_ARROW - Rotate entire scene positive 5 units on the Y axis" << "\n";
}

// Initialize program
void init()
{
    printCommands();
    initGlut();
    glClearColor(0.6, 0.6, 0.6, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);      
    
    light0 = Light(lightPos[0], lightAmb[0], lightDiff[0], lightSpec[0]);
    light1 = Light(lightPos[1], lightAmb[1], lightDiff[1], lightSpec[1]);

    glEnable(GL_TEXTURE_2D);
    glGenTextures(2, textures);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    stone_img = LoadPPM("textures/stone.ppm", &stoneWidth, &stoneHeight, &stoneMax);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, stoneWidth, stoneHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, stone_img);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);      

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    marble_img = LoadPPM("textures/marble.ppm", &marbleWidth, &marbleHeight, &marbleMax);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, marbleWidth, marbleHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, marble_img);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   

    glBindTexture(GL_TEXTURE_2D, textures[2]);
    wall_img = LoadPPM("textures/brick.ppm", &wallWidth, &wallHeight, &wallMax);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wallWidth, wallHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, wall_img);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);        
}

// main function - entry of the program
int main(int argc, char **argv)
{  
    glutInit(&argc, argv);
    init();    
    glutMainLoop();    
    return (0);
}
