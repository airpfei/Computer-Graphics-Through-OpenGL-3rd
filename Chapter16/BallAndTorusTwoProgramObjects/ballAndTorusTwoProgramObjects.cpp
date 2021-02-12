////////////////////////////////////////////////////////////////////////////          
// ballAndTorusTwoProgramObjects.cpp
//
// Forward-compatible core GL 4.3 modification of ballAndTorusShaderized.cpp
// demonstrating the use of two program objects to switch control between
// drawing sphere and torus.
//
// Interaction:
// Press space to toggle between animation on and off.
// Press the up/down arrow keys to speed up/slow down animation.
// Press the x, X, y, Y, z, Z keys to rotate the scene.
//
// Sumanta Guha
//////////////////////////////////////////////////////////////////////////// 

#include <cmath>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "prepShader.h"
#include "sphere.h"
#include "torus.h"

using namespace glm;

static enum object { SPHERE, TORUS }; // VAO ids.
static enum buffer { SPH_VERTICES, SPH_INDICES, TOR_VERTICES, TOR_INDICES }; // VBO ids.

// Globals.
static float latAngle = 0.0; // Latitudinal angle.
static float longAngle = 0.0; // Longitudinal angle.
static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0; // Angles to rotate scene.
static int isAnimate = 0; // Animated?
static int animationPeriod = 100; // Time interval between frames.

// Sphere data.
static Vertex sphVertices[(SPH_LONGS + 1) * (SPH_LATS + 1)];
static unsigned int sphIndices[SPH_LATS][2 * (SPH_LONGS + 1)];
static int sphCounts[SPH_LATS];
static void* sphOffsets[SPH_LATS];
static vec4 sphColors = vec4(SPH_COLORS);

// Torus data.
static Vertex torVertices[(TOR_LONGS + 1) * (TOR_LATS + 1)];
static unsigned int torIndices[TOR_LATS][2 * (TOR_LONGS + 1)];
static int torCounts[TOR_LATS];
static void* torOffsets[TOR_LATS];
static vec4 torColors = vec4(TOR_COLORS);

static mat4 modelViewMat, projMat;

static unsigned int
programId[2], // Array of 2 program ids.
vertexShaderId[2], // Array of 2 vertex shader ids.
fragmentShaderId[2], // Array of 2 fragment shader ids.
modelViewMatLoc,
projMatLoc,
sphColorLoc,
torColorLoc,
buffer[4],
vao[2];

// Initialization routine.
void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	// Create shader program executable to draw the sphere.
	vertexShaderId[SPHERE] = setShader("vertex", "Shaders/vertexShaderSphere.glsl");
	fragmentShaderId[SPHERE] = setShader("fragment", "Shaders/fragmentShaderSphere.glsl");
	programId[SPHERE] = glCreateProgram();
	glAttachShader(programId[SPHERE], vertexShaderId[SPHERE]);
	glAttachShader(programId[SPHERE], fragmentShaderId[SPHERE]);
	glLinkProgram(programId[SPHERE]);

	// Create shader program executable to draw the torus.
	vertexShaderId[TORUS] = setShader("vertex", "Shaders/vertexShaderTorus.glsl");
	fragmentShaderId[TORUS] = setShader("fragment", "Shaders/fragmentShaderTorus.glsl");
	programId[TORUS] = glCreateProgram();
	glAttachShader(programId[TORUS], vertexShaderId[TORUS]);
	glAttachShader(programId[TORUS], fragmentShaderId[TORUS]);
	glLinkProgram(programId[TORUS]);

	// Initialize shpere and torus.
	fillSphere(sphVertices, sphIndices, sphCounts, sphOffsets);
	fillTorus(torVertices, torIndices, torCounts, torOffsets);

	// Create VAOs and VBOs... 
	glGenVertexArrays(2, vao);
	glGenBuffers(4, buffer);

	// ...and associate sphere data with vertex shader.
	glBindVertexArray(vao[SPHERE]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[SPH_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphVertices), sphVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[SPH_INDICES]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphIndices), sphIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(0);

	// ...and associate torus data with vertex shader.
	glBindVertexArray(vao[TORUS]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[TOR_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(torVertices), torVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[TOR_INDICES]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(torIndices), torIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);

	// START specifying the sphere-drawing program variables.
	glUseProgram(programId[SPHERE]);

	// Obtain projection matrix uniform location and set value.
	projMatLoc = glGetUniformLocation(programId[SPHERE], "projMat");
	projMat = frustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

	// Obtain color uniform locations and set values.
	sphColorLoc = glGetUniformLocation(programId[SPHERE], "sphColor");
	glUniform4fv(sphColorLoc, 1, &sphColors[0]);

	// Obtain modelview matrix uniform and object uniform locations.
	modelViewMatLoc = glGetUniformLocation(programId[SPHERE], "modelViewMat");
	// END specifying the sphere-drawing program variables.

	// START specifying the torus-drawing program variables.
	glUseProgram(programId[TORUS]);

	// Obtain projection matrix uniform location and set value.
	projMatLoc = glGetUniformLocation(programId[TORUS], "projMat");
	projMat = frustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

	// Obtain color uniform locations and set values.
	torColorLoc = glGetUniformLocation(programId[TORUS], "torColor");
	glUniform4fv(torColorLoc, 1, &torColors[0]);

	// Obtain modelview matrix uniform and object uniform locations.
	modelViewMatLoc = glGetUniformLocation(programId[TORUS], "modelViewMat");
	// END specifying the torus-drawing program variables.

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Calculate modelview matrix.
	modelViewMat = mat4(1.0);
	modelViewMat = translate(modelViewMat, vec3(0.0, 0.0, -25.0));
	modelViewMat = rotate(modelViewMat, radians(Zangle), vec3(0.0, 0.0, 1.0));
	modelViewMat = rotate(modelViewMat, radians(Yangle), vec3(0.0, 1.0, 0.0));
	modelViewMat = rotate(modelViewMat, radians(Xangle), vec3(1.0, 0.0, 0.0));

	// Draw torus.
	glUseProgram(programId[TORUS]); // Activate torus-drawing program.
	glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat)); // Update modelview matrix.
	glBindVertexArray(vao[TORUS]);
	glMultiDrawElements(GL_TRIANGLE_STRIP, torCounts, GL_UNSIGNED_INT, (const void **)torOffsets, TOR_LATS);

	// Calculate modelview matrix.
	modelViewMat = rotate(modelViewMat, radians(longAngle), vec3(0.0, 0.0, 1.0));
	modelViewMat = translate(modelViewMat, vec3(12.0, 0.0, 0.0));
	modelViewMat = rotate(modelViewMat, radians(latAngle), vec3(0.0, 1.0, 0.0));
	modelViewMat = translate(modelViewMat, vec3(-12.0, 0.0, 0.0));
	modelViewMat = translate(modelViewMat, vec3(20.0, 0.0, 0.0));

	// Draw ball.
	glUseProgram(programId[SPHERE]); // Activate sphere-drawing program.
	glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat)); // Update modelview matrix.
	glBindVertexArray(vao[SPHERE]);
	glMultiDrawElements(GL_TRIANGLE_STRIP, sphCounts, GL_UNSIGNED_INT, (const void **)sphOffsets, SPH_LATS);

	glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
}

// Timer function.
void animate(int value)
{
	if (isAnimate)
	{
		latAngle += 5.0;
		if (latAngle > 360.0) latAngle -= 360.0;
		longAngle += 1.0;
		if (longAngle > 360.0) longAngle -= 360.0;

		glutPostRedisplay();
		glutTimerFunc(animationPeriod, animate, 1);
	}
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
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("ballAndTorusTwoProgramObjects.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}

