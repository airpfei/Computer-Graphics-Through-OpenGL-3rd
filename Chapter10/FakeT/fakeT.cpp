//////////////////////////////////////////////////////////////
// fakeT.cpp
//
// This program draws a simulated T-pipe using GLU cylinders -
// one cylinder sticks into the other.
//
// Interaction:. 
// Press space to toggle between wireframe and lit.
//
// Sumanta Guha.
//////////////////////////////////////////////////////////////

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

// Globals
static GLUquadricObj *qobj; // Create a pointer to a quadric object.
static char *objName; // Pointer to object name.
static int isLit = 1; // Is lit?

					  // Initialization routine.
void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST); // Enable depth testing.

							 // Light property vectors.
	float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
	float lightDifAndSpec[] = { 0.7, 0.7, 0.7, 1.0 };
	float lightPos[] = { 2.0, 1.0, 0.5, 1.0 };
	float globAmb[] = { 0.2, 0.2, 0.2, 1.0 };

	// Light properties.
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0); // Enable particular light source.
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Enable two-sided lighting.
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.

														 // Create the new quadric object.
	qobj = gluNewQuadric();

	glColor3f(0.0, 0.0, 0.0);
}

// Drawing routine.
void drawScene(void)
{
	// Material property vectors.
	float matAmbAndDif[] = { 0.0, 0.5, 0.8, 1.0 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	float matShine[] = { 50.0 };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (isLit)
	{
		glEnable(GL_LIGHTING); // Turn on OpenGL lighting.
		gluQuadricDrawStyle(qobj, GLU_FILL); // Specify that quadrics are drawn in wireframe.
	}
	else
	{
		glDisable(GL_LIGHTING); // Turn on OpenGL lighting.
		gluQuadricDrawStyle(qobj, GLU_LINE); // Specify that quadrics are drawn in wireframe.
	}

	glLoadIdentity();

	// Position the cylinders for viewing.
	gluLookAt(2.0, 2.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Material properties of the cylinders.
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);

	// First cylinder.
	gluCylinder(qobj, 0.4, 0.4, 2.0, 15, 5);

	// Second cylinder sticking into the first.
	glTranslatef(0.0, 0.0, 1.0);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	gluCylinder(qobj, 0.2, 0.2, 2.0, 15, 5);

	glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w / (float)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		if (isLit) isLit = 0;
		else isLit = 1;
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
	std::cout << "Press space to toggle between wireframe and lit." << std::endl;
}

// Main routine.
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("fakeT.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
