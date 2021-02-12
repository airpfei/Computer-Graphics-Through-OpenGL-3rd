//////////////////////////////////////////////////////////////////////////////////////////////
// billboard.cpp
//
// This program illustrates billboarding. Four copies of a rectangle textured with an image of
// two trees are arranged to give the illusion of a backdrop of trees. There is the option 
// to turn billboarding on and off.
//
// Interaction:
// Press the space key to toggle between billboarding on and off.
// 
// Sumanta Guha
//
// Texture Credits: See ExperimenterSource/Textures/TEXTURE_CREDITS.txt
//////////////////////////////////////////////////////////////////////////////////////////////

#include <cmath>					
#include <cstdlib>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#include "getBMP.h"

#define PI 3.14159265358979324

// Globals.
static unsigned int texture[1]; // Array of texture indices.
static unsigned int aRect; // List index.
static float b = 0.0; // Displacement of the trees image left of line of sight.
static float d = 0.0; // Distance of the trees image parallel to the line of sight.
static int isBillboard = 1; // Is billboarding on?
static long font = (long)GLUT_BITMAP_8_BY_13; // Font selection.

// Load external textures.
void loadTextures()
{
	// Local storage for bmp image data.
	imageFile *image[1];

	// Load the image.
	image[0] = getBMP("../../Textures/trees.bmp");

	// Bind trees image to texture object texture[0]. 
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string)
{
	char *c;

	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// Initialization routine.
void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	// Create texture ids.
	glGenTextures(1, texture);

	// Load external textures.
	loadTextures();

	// Specify how texture values combine with current surface color values.
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Turn on OpenGL texturing.
	glEnable(GL_TEXTURE_2D);

	aRect = glGenLists(1); // Return a list index.

	// Begin create a display list.
	glNewList(aRect, GL_COMPILE);

	// Draw a textured rectangle.
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-5.0, -2.5, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(5.0, -2.5, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(5.0, 2.5, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-5.0, 2.5, 0.0);
	glEnd();

	glEndList();
	// End create a display list.
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// Write message.
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos3f(-1.0, 4.0, -5.1);
	if (isBillboard) writeBitmapString((void*)font, "Billboarding on!");
	else writeBitmapString((void*)font, "Billboarding off!");
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);

	// Place first trees image.
	b = 15.0; d = 10.0;
	glPushMatrix();
	glTranslatef(-b, 0.0, -d);
	if (isBillboard) glRotatef(atan(b / d)*(180.0 / PI), 0.0, 1.0, 0.0); // Rotate if billboarding is on.
	glCallList(aRect);
	glPopMatrix();

	// Place second trees image.
	b = 6.0; d = 15.0;
	glPushMatrix();
	glTranslatef(-b, 0.0, -d);
	if (isBillboard) glRotatef(atan(b / d)*(180.0 / PI), 0.0, 1.0, 0.0); // Rotate if billboarding is on.
	glCallList(aRect);
	glPopMatrix();

	// Place third trees image. 
	b = -3.0; d = 15.0;
	glPushMatrix();
	glTranslatef(-b, 0.0, -d);
	if (isBillboard) glRotatef(atan(b / d)*(180.0 / PI), 0.0, 1.0, 0.0); // Rotate if billboarding is on.
	glCallList(aRect);
	glPopMatrix();

	// Place fourth trees image.
	b = -12.0; d = 10.0;
	glPushMatrix();
	glTranslatef(-b, 0.0, -d);
	if (isBillboard) glRotatef(atan(b / d)*(180.0 / PI), 0.0, 1.0, 0.0); // Rotate if billboarding is on.
	glCallList(aRect);
	glPopMatrix();

	glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-10.0, 10.0, -5.0, 5.0, 5.0, 100.0);
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
	case ' ':
		if (isBillboard) isBillboard = 0;
		else isBillboard = 1;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press the space key to toggle between billboarding on and off." << std::endl;
}

// Main routine.
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("billboard.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
