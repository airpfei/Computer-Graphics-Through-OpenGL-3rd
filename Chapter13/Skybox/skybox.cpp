///////////////////////////////////////////////////////////////////////////////        
// skybox.cpp
//
// This program creates a skybox using cube map textures. Additionally, there is
// a black ball in the scene.
//
// Interaction:
// Press the up/down arrow keys to move the viewpoint, the left/right arrow and 
// page up/page down keys to turn it.
//
// Sumanta Guha.
/////////////////////////////////////////////////////////////////////////////// 

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#define PI 3.14159265

#include "getBMP.h"

// Globals.
static unsigned int textureCube;
static float zVal = 0.0; // z-displacement of eye.
static float longAngle = 0.0, latAngle = 0.0; // Angles of point of view.

// Load external textures.
void loadTextures()
{
	// Local storage for bmp image data.
	imageFile *imageCube[6];

	// Load the six cube map images.
	imageCube[0] = getBMP("../../Textures/IceRiver/posx.bmp");
	imageCube[1] = getBMP("../../Textures/IceRiver/negx.bmp");
	imageCube[2] = getBMP("../../Textures/IceRiver/posy.bmp");
	imageCube[3] = getBMP("../../Textures/IceRiver/negy.bmp");
	imageCube[4] = getBMP("../../Textures/IceRiver/posz.bmp");
	imageCube[5] = getBMP("../../Textures/IceRiver/negz.bmp");

	// Bind the cube map texture and define its 6 component textures.
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureCube);
	for (int face = 0; face < 6; face++)
	{
		int target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;
		glTexImage2D(target, 0, GL_RGBA, imageCube[0]->width, imageCube[0]->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageCube[face]->data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	// Create texture ids.
	glGenTextures(1, &textureCube);

	// Load external texture. 
	loadTextures();

	// Specify how texture values combine with current surface color values.
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Turn on OpenGL cube map texturing.
	glEnable(GL_TEXTURE_CUBE_MAP);
}

void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glLoadIdentity();

	// Translation/rotation of POV.
	glRotatef(longAngle, 0.0, 1.0, 0.0);
	glRotatef(latAngle, 1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.0, zVal);

	// Enter texture matrix mode, apply POV rotations.
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glRotatef(longAngle, 0.0, 1.0, 0.0);
	glRotatef(latAngle, 1.0, 0.0, 0.0);

	// Disable depth buffer.
	glDepthMask(GL_FALSE);

	// Draw a square textured with cubemap after reversing POV rotations.
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glRotatef(-latAngle, 1.0, 0.0, 0.0);
	glRotatef(-longAngle, 0.0, 1.0, 0.0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureCube);
	glBegin(GL_POLYGON);
	glTexCoord3f(-1.0, 1.0, 1.0); glVertex3f(-50.0, -50.0, -25.0);
	glTexCoord3f(1.0, 1.0, 1.0); glVertex3f(50.0, -50.0, -25.0);
	glTexCoord3f(1.0, -1.0, 1.0); glVertex3f(50.0, 50.0, -25.0);
	glTexCoord3f(-1.0, -1.0, 1.0); glVertex3f(-50.0, 50.0, -25.0);
	glEnd();
	glPopMatrix();

	// Enable depth buffer.
	glDepthMask(GL_TRUE);

	// Draw sphere.
	glDisable(GL_TEXTURE_CUBE_MAP);
	glTranslatef(0.0, 0.0, -25.0);
	glColor3f(0.0, 0.0, 0.0);
	glutWireSphere(5.0, 20, 20);

	glutSwapBuffers();
}

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
	// Compute next eye disposition.
	if (key == GLUT_KEY_LEFT) longAngle -= 1.0;
	if (key == GLUT_KEY_RIGHT) longAngle += 1.0;
	if (key == GLUT_KEY_PAGE_DOWN) latAngle -= 1.0;
	if (key == GLUT_KEY_PAGE_UP) latAngle += 1.0;
	if (key == GLUT_KEY_UP) zVal += 0.5;
	if (key == GLUT_KEY_DOWN) zVal -= 0.5;

	// Keep angles within [0, 360].
	if (longAngle > 360.0) longAngle -= 360.0;
	if (longAngle < 0.0) longAngle += 360.0;
	if (latAngle > 360.0) latAngle -= 360.0;
	if (latAngle < 0.0) latAngle += 360.0;

	// Clamp eye location to within a limited foreground region.
	if (zVal > 12.0) zVal = 12.0;
	if (zVal < -12.0) zVal = -12.0;

	glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press the up/down arrow keys to move the viewpoint, the left/right arrow and page up/page down keys to turn it." << std::endl;
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
	glutCreateWindow("skybox.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
