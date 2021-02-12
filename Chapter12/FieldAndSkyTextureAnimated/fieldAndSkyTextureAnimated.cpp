///////////////////////////////////////////////////////////////////////////////////////////// 
// fieldAndSkyTextureAnimated.cpp
//
// This program enhances fieldAndSky.cpp to animate the sky texture by applying a translation 
// to the current texture matrix.
//
// Interaction:
// Press space to toggle between animation on and off.
//
// Sumanta Guha
//
// Texture Credits: See ExperimenterSource/Textures/TEXTURE_CREDITS.txt
/////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#include "getBMP.h"

// Globals.
static unsigned int texture[2]; // Array of texture indices.
static float angle = 0.0; // Angle of rotation of texture coordinates.

static int isAnimate = 0; // Animated?
static int animationPeriod = 100; // Time interval between frames.

// Load external textures.
void loadTextures()
{
	// Local storage for bmp image data.
	imageFile *image[2];

	// Load the images.
	image[0] = getBMP("../../Textures/grass.bmp");
	image[1] = getBMP("../../Textures/sky.bmp");

	// Bind grass image to texture object texture[0]. 
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Bind sky image to texture object texture[1]
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->width, image[1]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

// Timer function.
void animate(int value)
{
	if (isAnimate)
	{
		angle += 0.01;
		if (angle > 360.0) angle -= 360.0;
		glutPostRedisplay();
		glutTimerFunc(animationPeriod, animate, 1);
	}
}

// Initialization routine.
void setup(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	// Create texture ids.
	glGenTextures(2, texture);

	// Load external textures.
	loadTextures();

	// Specify how texture values combine with current surface color values.
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Turn on OpenGL texturing.
	glEnable(GL_TEXTURE_2D);
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0.0, 10.0, 15.0, 0.0, 10.0, 0.0, 0.0, 1.0, 0.0);

	// Enter texture mode and load identity.
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	// Map the grass texture onto a rectangle along the xz-plane.
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, 0.0, 100.0);
	glTexCoord2f(8.0, 0.0); glVertex3f(100.0, 0.0, 100.0);
	glTexCoord2f(8.0, 8.0); glVertex3f(100.0, 0.0, -100.0);
	glTexCoord2f(0.0, 8.0); glVertex3f(-100.0, 0.0, -100.0);
	glEnd();

	// Apply translation to texture coordinates in a circular manner.
	glTranslatef(0.1 * cos(angle), 0.1 * sin(angle), 0.0);

	// Map the sky texture onto a rectangle parallel to the xy-plane.
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.1, 0.1); glVertex3f(-100.0, 0.0, -70.0);
	glTexCoord2f(0.9, 0.1); glVertex3f(100.0, 0.0, -70.0);
	glTexCoord2f(0.9, 0.9); glVertex3f(100.0, 120.0, -70.0);
	glTexCoord2f(0.1, 0.9); glVertex3f(-100.0, 120.0, -70.0);
	glEnd();

	// Reenter modelview mode.
	glMatrixMode(GL_MODELVIEW);

	glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
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
		if (isAnimate) isAnimate = 0;
		else
		{
			isAnimate = 1;
			animate(1);
		}
		break;
	default:
		break;
	}
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press space to toggle between animation on and off." << std::endl;
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
	glutInitWindowPosition(100, 100);
	glutCreateWindow("fieldAndSkyTextureAnimated.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
