///////////////////////////////////////////////////////////////////////////////////
// multitexture.cpp
//
// This program demonstrates multitexturing and the interpolation combiner function
// by interpolating between night and day textures.
//
// Interaction:
// Press the left and right arrow keys to transform the sky.
//
// Sumanta Guha
//
// Texture Credits: See ExperimenterSource/Textures/TEXTURE_CREDITS.txt
///////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#include "getBMP.h"

// Globals.
static unsigned int texture[2]; // Array of texture indices.
static float alpha = 0.0; // Interpolation parameter.
static float constColor[4]; // Constant texture environment color.

// Load external textures.
void loadTextures()
{
	// Local storage for bmp image data.
	imageFile *image[2];

	// Load the texture.
	image[0] = getBMP("../../Textures/sky.bmp");
	image[1] = getBMP("../../Textures/nightSky.bmp");

	// Bind day sky image to texture[0]. 
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Bind night sky image to texture[1]
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->width, image[1]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

// Initialization routine.
void setup(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// Create texture ids.
	glGenTextures(2, texture);

	// Load external textures.
	loadTextures();

	// Select texture unit 0 as the currently active texture unit and specify its texture states.
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // Unit 0 REPLACES the current pixels. 

	// Select texture unit 1 as the currently active texture unit and specify its texture states.
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE); // Unit 1 COMBINES with unit 0 in a manner
																// specified by the combiner function.

	// The COMBINER FUNCTION is specified to be interpolation between RGB values of Arg0 and Arg1 
	// according to the formula: Arg0 * Arg2 + Arg1 * (1-Arg2)
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);

	// Set of statements specifying the interpolation combiner's arguments.
	glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE0); // Texture combiner's zeroth source's RGB are from texture unit 0. 
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR); // Arg0's RGB values are zeroth source's color.

	glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE1); // Texture combiner's first source's RGB are from texture unit 1.
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR); // Arg1's RGB values are first source's color.

	glTexEnvi(GL_TEXTURE_ENV, GL_SRC2_ALPHA, GL_CONSTANT); // Texture combiner's second source's alpha is from GL_TEXTURE_ENV_COLOR.
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA, GL_SRC_ALPHA); // Arg2 is second source's alpha.
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0.0, 0.0, 20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, constColor); // Specify the texture environment variable.
	constColor[3] = alpha; // Set the interpolation parameter, which is the alpha of environment color (i.e., Arg2 in setup).

	glBegin(GL_POLYGON);
	// glMultiTexCoord2f(GL_TEXTUREi, *, *) specifies the texture coordinates of texture unit i.
	glMultiTexCoord2f(GL_TEXTURE0, 0.0, 0.0);
	glMultiTexCoord2f(GL_TEXTURE1, 0.0, 0.0);
	glVertex3f(-20.0, -20.0, 0.0);

	glMultiTexCoord2f(GL_TEXTURE0, 1.0, 0.0);
	glMultiTexCoord2f(GL_TEXTURE1, 1.0, 0.0);
	glVertex3f(20.0, -20.0, 0.0);

	glMultiTexCoord2f(GL_TEXTURE0, 1.0, 1.0);
	glMultiTexCoord2f(GL_TEXTURE1, 1.0, 1.0);
	glVertex3f(20.0, 20.0, 0.0);

	glMultiTexCoord2f(GL_TEXTURE0, 0.0, 1.0);
	glMultiTexCoord2f(GL_TEXTURE1, 0.0, 1.0);
	glVertex3f(-20.0, 20.0, 0.0);
	glEnd();

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
	default:
		break;
	}
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
	if (key == GLUT_KEY_RIGHT)
	{
		if (alpha < 1.0) alpha += 0.01;
	}
	if (key == GLUT_KEY_LEFT)
	{
		if (alpha > 0.0) alpha -= 0.01;
	}
	glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press the left and right arrow keys to transform the sky." << std::endl;
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
	glutCreateWindow("multitexture.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
