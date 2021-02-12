/////////////////////////////////////////////////////////////
// shear.cpp
// This program animates a box sheared on impact by a ball.
// The shear matrix is explicitly computed.
// 
// Interaction: 
// Press the left and right arrow key to move the ball.
// Press r to reset.
//
// Sumanta Guha
/////////////////////////////////////////////////////////////

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

/* Globals */
static float dist = 0.0; // Distance traveled by ball.
static float boxShearLength = 0.0; // Length of shear at top of box.
static float matrixdata[16] = // Shear matrix.
{
	1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0
};

// Initialization routine.
void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glColor3f(0.0, 0.0, 0.0);

	// Moving ball.
	glPushMatrix();
	glTranslatef(-20.0 + dist, 8.0, 0.0);
	glutWireSphere(1.0, 10, 8);
	glPopMatrix();

	// Box.
	// Translate box (after contact with ball) to compensate for the shear -
	// so that the base of the box remains stationary.
	if (dist >= 17.0) glTranslatef(boxShearLength, 0.0, 0.0);

	glMultMatrixf(matrixdata); // Apply the shear matrix.
	glScalef(4.0, 16.0, 4.0);
	glutWireCube(1.0);

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
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'r':
		dist = 0.0;
		boxShearLength = 0.0;
		matrixdata[4] = 0.0;
		glutPostRedisplay();
		break;
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
	if (key == GLUT_KEY_RIGHT) dist += 1.0;
	if (key == GLUT_KEY_LEFT) dist -= 1.0;
	if (dist >= 17.0)
	{
		boxShearLength = (dist - 17.0) / 2;
		matrixdata[4] = boxShearLength / 8.0; // Tan of the angle of shear = 
											  // boxShearLength/y-value of the ball.
	}
	glutPostRedisplay();
}

// Keyboard input processing routine.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press the left and right arrow key to move the ball." << std::endl
		<< "Press r to reset." << std::endl;
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
	glutCreateWindow("shear.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}

