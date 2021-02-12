////////////////////////////////////////////////////////////////////////////
// manipulateProjectionMatrix.cpp
//
// This program accesses the projection matrix stack. It is modified from
// manipulateModelviewMatrix.cpp. Matrix data is written to the C++ window.
// 
// Sumanta Guha
////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

// Array to store the current projection matrix in column-major order.
static float projectionMatrixData[16];
// End globals.

// Print the matrix in conventional rectangular form.
void displayProjectionMatrix(void)
{
	std::cout << "The current projection matrix is:" << std::endl;
	std::cout << projectionMatrixData[0] << "   ";
	std::cout << projectionMatrixData[4] << "   ";
	std::cout << projectionMatrixData[8] << "   ";
	std::cout << projectionMatrixData[12] << std::endl;
	std::cout << projectionMatrixData[1] << "   ";
	std::cout << projectionMatrixData[5] << "   ";
	std::cout << projectionMatrixData[9] << "   ";
	std::cout << projectionMatrixData[13] << std::endl;
	std::cout << projectionMatrixData[2] << "   ";
	std::cout << projectionMatrixData[6] << "   ";
	std::cout << projectionMatrixData[10] << "   ";
	std::cout << projectionMatrixData[14] << std::endl;
	std::cout << projectionMatrixData[3] << "   ";
	std::cout << projectionMatrixData[7] << "   ";
	std::cout << projectionMatrixData[11] << "   ";
	std::cout << projectionMatrixData[15] << std::endl << std::endl;
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);
	glLoadIdentity();

	gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glScalef(1.0, 2.0, 1.0);

	glutWireCube(5.0);
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

	// Read and print the current projection matrix.
	glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrixData);
	displayProjectionMatrix();

	glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);

	// Read and print the current projection matrix.
	glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrixData);
	displayProjectionMatrix();

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
	default:
		break;
	}
}

// Main routine.
int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("manipulateProjectionMatrix.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
