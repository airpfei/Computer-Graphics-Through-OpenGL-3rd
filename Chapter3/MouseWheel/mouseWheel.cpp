///////////////////////////////////////////////////////////////////////////////////         
// mouseWheel.cpp
// 
// This program enhances mouseMotion.cpp to with the capability of changing the 
// current point's size by turning the mouse wheel.
// 
// Interaction:
// Left mouse click to draw a square point, keep left button pressed to drag point, 
// right mouse click to exit, rotate wheel to change the current point's size.
// 
// Sumanta Guha.
/////////////////////////////////////////////////////////////////////////////////// 

#include <cstdlib>
#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

// Globals.
static int width, height; // OpenGL window size.
static float pointSize = 3.0; // Initial size of point.
static float tempSize = pointSize; // Storage for point size.

// Point class.
class Point
{
public:
	Point(float x, float y, float s)
	{
		xVal = x; yVal = y; size = s;
	}
	Point() {};
	void setCoords(float x, float y)
	{
		xVal = x; yVal = y;
	}
	void setSize(float s)
	{
		size = s;
	}
	void drawPoint(void); // Function to draw a point.
private:
	float xVal, yVal; // x and y co-ordinates of point.
	float size; // Size of point.
};

// Function to draw a point.
void Point::drawPoint(void)
{
	glPointSize(size);
	glBegin(GL_POINTS);
	glVertex3f(xVal, yVal, 0.0);
	glEnd();
}

// Vector of points.
std::vector<Point> points;

// Currently clicked point.
Point currentPoint;

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);

	// Loop through the points array drawing each point.
	for (auto point : points) { point.drawPoint(); }

	currentPoint.drawPoint();

	glFlush();
}

// Mouse callback routine.
void mouseControl(int button, int state, int x, int y)
{
	// Store the clicked point in the currentPoint variable when left button is pressed.
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		currentPoint = Point(x, height - y, pointSize);

	// Store the currentPoint in the points vector when left button is released.
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		points.push_back(currentPoint);

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) exit(0);

	glutPostRedisplay();
}

// Mouse motion callback routine.
void mouseMotion(int x, int y)
{
	// Update the location of the current point as the mouse moves with button pressed.
	currentPoint.setCoords(x, height - y);

	glutPostRedisplay();
}

// Mouse wheel callback routine.
void mouseWheel(int wheel, int direction, int x, int y)
{
	// Increment/decrement size of current point (at the back of vector points)
	// depending on the direction of rotation of the mouse wheel.
	(direction > 0) ? tempSize++ : tempSize--;
	points.back().setSize(tempSize);

	glutPostRedisplay();
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

	// Set viewing box dimensions equal to window dimensions.
	glOrtho(0.0, w, 0.0, h, -1.0, 1.0);

	// Pass the size of the OpenGL window to globals.
	width = w;
	height = h;

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

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Left mouse click to draw a square point, keep left button pressed to drag point," << std::endl
		<< "right mouse click to exit, rotate wheel to change the current point's size." << std::endl;
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
	glutCreateWindow("mouseWheel.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	// Register the mouse callback function.
	glutMouseFunc(mouseControl);

	// Register the mouse motion callback function.
	glutMotionFunc(mouseMotion);

	// Register the mouse wheel callback function.
	glutMouseWheelFunc(mouseWheel);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}