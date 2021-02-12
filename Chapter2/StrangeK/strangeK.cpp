/////////////////////////////////////////////////////////////////////////////////////////        
// strangeK.cpp
//
// This program draws the letter K as one triangle strip. However, a few of the component
// triangles turn out to be degenerate.
//
// Interaction:
// Press the space bar to toggle between wirefrime and filled.
// 
// Contributed by Wuttinan Sereethavekul.
///////////////////////////////////////////////////////////////////////////////////////// 

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

// Globals.
static int isWire = 0; // Is wireframe?

// Drawing routine.
void drawScene(void)
{
	int i;

	glClear(GL_COLOR_BUFFER_BIT);
								
	if (isWire) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.0, 0.0, 0.0);
	
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(10.0, 10.0, 0.0);
	glVertex3f(20.0, 10.0, 0.0);
	glVertex3f(10.0, 50.0, 0.0);
	glVertex3f(20.0, 50.0, 0.0);
	glVertex3f(30.0, 50.0, 0.0);
	glVertex3f(50.0, 10.0, 0.0);
	glVertex3f(60.0, 10.0, 0.0);
	glVertex3f(30.0, 50.0, 0.0);
	glVertex3f(30.0, 50.0, 0.0);
	glVertex3f(20.0, 50.0, 0.0);
	glVertex3f(60.0, 90.0, 0.0);
	glVertex3f(50.0, 90.0, 0.0);
	glVertex3f(20.0, 50.0, 0.0);
	glVertex3f(20.0, 50.0, 0.0);
	glVertex3f(10.0, 50.0, 0.0);
	glVertex3f(20.0, 90.0, 0.0);
	glVertex3f(10.0, 90.0, 0.0);
	glEnd();

	glFlush();
}

// Initialization routine.
void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		if (isWire == 0) isWire = 1;
		else isWire = 0;
		glutPostRedisplay();
		break;
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
	std::cout << "Press the space bar to toggle between wirefrime and filled." << std::endl;
}

// Main routine.
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("strangeK");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
