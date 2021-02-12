/////////////////////////////////////////////////////////////////////////
// occlusion.cpp
//
// This program implements an occlusion query to check if the bounding
// box of a sphere is visible.
//
// Interaction:
// Press the arrow keys to move the sphere and its bounding box.
// Press the space bar to toggle between showing/hiding the bounding box.
//
// Sumanta Guha.
/////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

// Globals.
static float Xvalue = 0.0, Yvalue = 0.0; // Co-ordinates of the sphere.
static long font = (long)GLUT_BITMAP_8_BY_13; // Font selection.
static unsigned int query; // Query id. 
static unsigned int result = 0; // Result of query = number of fragments passing depth test.
static unsigned int resultAvailable = 0; // Is the query result available?
static unsigned int boxDrawn = 1; // Is the bounding box visible?

// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string)
{
	char *c;

	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();

	// Draw a fixed obscuring green rectangle.
	glPushMatrix();
	glColor3f(0.0, 1.0, 0.0);
	glTranslatef(0.0, 0.0, -3.0);
	glRectf(-0.5, -0.5, 0.5, 0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(Xvalue, Yvalue, -5.0);

	// Draw a red cube, which bounds a sphere, with an occlusion query active. 
	// If boxVisible is true, then the cube is rendered hiding the sphere; if false, 
	// (a) the cube is not displayed, and 
	// (b) it does not alter depth values.
	glBeginQuery(GL_SAMPLES_PASSED, query); // Begin query operation, i.e., start counting fragments which pass the depth test.

	if (!boxDrawn)
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Make color buffer not writable.
		glDepthMask(GL_FALSE); // Make depth buffer not writable.
	}

	glColor3f(1.0, 0.0, 0.0);
	glutSolidCube(1.0);

	if (!boxDrawn)
	{
		glDepthMask(GL_TRUE); // Make depth buffer writable.
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Make color buffer writable.
	}

	glEndQuery(GL_SAMPLES_PASSED); // End query operation, i.e., stop counting fragments.

								   // Spin in a loop until the query result is ready. 
	while (!resultAvailable) glGetQueryObjectuiv(query, GL_QUERY_RESULT_AVAILABLE, &resultAvailable);

	// Retreive the result of the query, place in result.
	glGetQueryObjectuiv(query, GL_QUERY_RESULT, &result);

	// If fragments of its bounding box are visible, draw the sphere;  
	if (result)
	{
		glColor3f(0.0, 0.0, 1.0);
		glutWireSphere(0.5, 16, 10);
	}

	glPopMatrix();

	// Write status messages.
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos3f(-1.2, 1.2, -2.0);
	if (boxDrawn) writeBitmapString((void*)font, "Bounding box is drawn.");
	else writeBitmapString((void*)font, "Bounding box is not drawn.");

	glRasterPos3f(-1.2, 1.1, -2.0);
	if (result) writeBitmapString((void*)font, "Bounding box is visible even though it may not be drawn.");
	else writeBitmapString((void*)font, "Bounding box is obscured whether drawn or not.");

	glutSwapBuffers();
}

// Initialization routine.
void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glGenQueries(1, &query); // Generate a query object, putting its id in query.
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	glMatrixMode(GL_MODELVIEW);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		if (boxDrawn) boxDrawn = 0; else boxDrawn = 1;
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
	if (key == GLUT_KEY_UP) Yvalue += 0.1;
	if (key == GLUT_KEY_DOWN) Yvalue -= 0.1;
	if (key == GLUT_KEY_LEFT) Xvalue -= 0.1;
	if (key == GLUT_KEY_RIGHT) Xvalue += 0.1;
	glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press the arrow keys to move the sphere and its bounding box." << std::endl
		<< "Press the space bar to toggle between showing/hiding the bounding box." << std::endl;
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
	glutCreateWindow("occlusion.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
