////////////////////////////////////////////////////////////////////////////
// imageManipulationFBO.cpp
//
// This program shows image manipulation, particularly a texture image which
// can be moved around the OpenGL window, by blitting from an FBO.
//
// Interaction:
// Click and drag the mouse to move the texture image.
//
// Sumanta Guha
//
// Texture Credits: See ExperimenterSource/Textures/TEXTURE_CREDITS.txt
/////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#include "getBMP.h"

// Globals.
static unsigned int windowWidth, windowHeight; // OpenGL window size.
static unsigned int rasterX = 0; // Raster x-coordinate
static unsigned int rasterY = 0; // Raster y-coordinate
static unsigned int framebuffer; // FBO id.
static unsigned int texture; // Id of texture to be rendered.
static unsigned int textureWidth, textureHeight; // Texture size.

// Initialization routine.
void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);

	// Load the texture image into a BitMapFile.
	imageFile *image[1];
	image[0] = getBMP("../../Textures/number1.bmp");

	// Create id for texture to be rendered and bind it.
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Create id for framebuffer object, bind it and attach rendered texture as the color buffer.
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	textureWidth = image[0]->width;
	textureHeight = image[0]->height;
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);

	// Specify the from and to buffers.
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, NULL);

	// Copy from the FBO to the default framebuffer.
	glBlitFramebuffer(0, 0, textureWidth, textureHeight, rasterX, rasterY, rasterX + textureWidth,
		              rasterY + textureHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glutSwapBuffers();
}

// Mouse callback routine.
void mouseControl(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// Current raster position is set at the mouse click.
		rasterX = x;
		rasterY = windowHeight - y;
	}

	glutPostRedisplay();
}

// Mouse motion callback routine.
void mouseMotion(int x, int y)
{
	// Current raster position is set at the mouse position.
	rasterX = x;
	rasterY = windowHeight - y;

	glutPostRedisplay();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set viewing box dimensions equal to window dimensions.
	glOrtho(0.0, (float)w, 0.0, (float)h, -1.0, 1.0);

	// Pass the size of the OpenGL window to globals.
	windowWidth = w;
	windowHeight = h;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Click and drag the mouse to move the texture image." << std::endl;
}

// Main routine.
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("imageManipulationFBO.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutMouseFunc(mouseControl);
	glutMotionFunc(mouseMotion);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
