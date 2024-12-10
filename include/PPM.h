#ifndef PPM_H_
#define PPM_H_

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


GLubyte * LoadPPM(
    const char * file,
    int * width,
    int * height,
    int * max
);

#endif
