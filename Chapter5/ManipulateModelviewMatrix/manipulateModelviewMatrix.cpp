////////////////////////////////////////////////////////////////////////////
// manipulateModelviewMatrix.cpp
//
// This program shows access and manipulation of the modelview matrix stack.
// Matrix data is written to the C++ window.
// 
// Sumanta Guha
////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#define X 0.707107 /* one divided by root two */

// Begin globals.
// Matrix corresponding to rotation by 45 degrees about the z-axis - 
// stored in an array in column-major order.
static float matrixData[16] =
{
	X, X, 0.0, 0.0, -X, X, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0
};

// Array to store the current modelview matrix in column-major order.
static float modelviewMatrixData[16];
// End globals.

// Output the current modelview matrix in conventional rectangular form.
void displayModelviewMatrix(void)
{
	glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrixData);

	std::cout << "the current modelview matrix is:" << std::endl;
	std::cout << modelviewMatrixData[0] << "   ";
	std::cout << modelviewMatrixData[4] << "   ";
	std::cout << modelviewMatrixData[8] << "   ";
	std::cout << modelviewMatrixData[12] << std::endl;
	std::cout << modelviewMatrixData[1] << "   ";
	std::cout << modelviewMatrixData[5] << "   ";
	std::cout << modelviewMatrixData[9] << "   ";
	std::cout << modelviewMatrixData[13] << std::endl;
	std::cout << modelviewMatrixData[2] << "   ";
	std::cout << modelviewMatrixData[6] << "   ";
	std::cout << modelviewMatrixData[10] << "   ";
	std::cout << modelviewMatrixData[14] << std::endl;
	std::cout << modelviewMatrixData[3] << "   ";
	std::cout << modelviewMatrixData[7] << "   ";
	std::cout << modelviewMatrixData[11] << "   ";
	std::cout << modelviewMatrixData[15] << std::endl << std::endl;
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);
	glLoadIdentity();

	// Output the current modelview matrix.
	std::cout << "After the initial glLoadIdentity() statement" << std::endl;
	displayModelviewMatrix();

	gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Output the current modelview matrix.
	std::cout << "After the gluLookAt() statement" << std::endl;
	displayModelviewMatrix();

	// Multiply the current modelview matrix on the right by the
	// matrix corresponding to rotation by 45 degrees about the z-axis.
	glMultMatrixf(matrixData);

	// Output the current modelview matrix.
	std::cout << "After the glMultMatrixf() statement" << std::endl;
	displayModelviewMatrix();

	glScalef(1.0, 2.0, 1.0);

	// Output the current modelview matrix.
	std::cout << "After the glScalef() statement" << std::endl;
	displayModelviewMatrix();

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
	glutCreateWindow("manipulateModelviewMatrix.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
