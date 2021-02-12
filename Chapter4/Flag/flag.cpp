////////////////////////////////////////////////////////////////      
// flag.cpp
//
// This program draws a fluttering flag.
//
// Interaction:
// Press space to toggle between animation on and off.
// Press the up/down arrow keys to speed up/slow down animation.
// Press the x, X, y, Y, z, Z keys to rotate the scene.
//
// Sumanta Guha.
////////////////////////////////////////////////////////////////   

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#define PI 3.14159265

// Globals.
static float s = 0.0; // Amount of shift of the piece of the sine curve.
static int p = 20; // Number of segments along the  length (i.e., sine curve section) of the flag.
static int q = 4; // Number of segments along the width of the flag.
static float *vertices = NULL; // Vertex array containing vertices on the flag.
static float Xangle = 60.0, Yangle = 0.0, Zangle = 0.0; // Angles to rotate scene.
static int isAnimate = 0; // Animated?
static int animationPeriod = 100; // Time interval between frames.

// Routine to fill the vertex array with co-ordinates of vertices of the flag.
void fillVertexArray(void)
{
	int k = 0;
	for (int j = 0; j <= q; j++)
		for (int i = 0; i <= p; i++)
		{
			vertices[k++] = -20.0 + 40.0 * (float)i / p;
			vertices[k++] = 5.0 * sin(s + (float)i / p * 1.8 * PI) - 5.0 * sin(s);
			vertices[k++] = -10.0 + 20.0 * (float)j / q;
		}
}

// Drawing routine.
void drawScene(void)
{
	int i, j;

	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -35.0);

	// Rotate scene.
	glRotatef(Zangle, 0.0, 0.0, 1.0);
	glRotatef(Yangle, 0.0, 1.0, 0.0);
	glRotatef(Xangle, 1.0, 0.0, 0.0);

	// Fill the vertex array.
	fillVertexArray();

	// Flag.
	glColor3f(0.0, 0.0, 0.0);
	for (j = 0; j < q; j++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (i = 0; i <= p; i++)
		{
			glArrayElement((j + 1)*(p + 1) + i);
			glArrayElement(j*(p + 1) + i);
		}
		glEnd();
	}

	// Flag pole.
	glLineWidth(5.0);
	glBegin(GL_LINES);
	glVertex3f(-20.0, 0.0, -10.0);
	glVertex3f(-20.0, 0.0, 20.0);
	glEnd();
	glLineWidth(1.0);


	glutSwapBuffers();
}

// Timer function.
void animate(int value)
{
	if (isAnimate)
	{
		s += 1.8*PI / p;
		if (s > 2.0*PI) s -= 2.0*PI;

		glutPostRedisplay();
		glutTimerFunc(animationPeriod, animate, 1);
	}
}

// Initialization routine.
void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST); // Enable depth testing.
	glEnableClientState(GL_VERTEX_ARRAY);

	vertices = new float[3 * (p + 1)*(q + 1)]; // Array allocation with values of p and q. 
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
	case 'x':
		Xangle += 5.0;
		if (Xangle > 360.0) Xangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'X':
		Xangle -= 5.0;
		if (Xangle < 0.0) Xangle += 360.0;
		glutPostRedisplay();
		break;
	case 'y':
		Yangle += 5.0;
		if (Yangle > 360.0) Yangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'Y':
		Yangle -= 5.0;
		if (Yangle < 0.0) Yangle += 360.0;
		glutPostRedisplay();
		break;
	case 'z':
		Zangle += 5.0;
		if (Zangle > 360.0) Zangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'Z':
		Zangle -= 5.0;
		if (Zangle < 0.0) Zangle += 360.0;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
	if (key == GLUT_KEY_DOWN) animationPeriod += 5;
	if (key == GLUT_KEY_UP) if (animationPeriod > 5) animationPeriod -= 5;
	glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press space to toggle between animation on and off." << std::endl
		<< "Press the up/down arrow keys to speed up/slow down animation." << std::endl
		<< "Press the x, X, y, Y, z, Z keys to rotate the scene." << std::endl;
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
	glutCreateWindow("flag.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}

