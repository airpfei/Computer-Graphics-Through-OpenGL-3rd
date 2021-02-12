////////////////////////////////////////////////////////////////////////////////////////      
// lightAndMaterial1.cpp
// 
// This program draws a blue ball lit by two positional lights, one white and one green,
// the location of each shown by a small wire sphere. Various material properties of the
// ball can be controlled and it can be moved up and down.
// 
// Interaction:
// Press 'a/A' to decrease/increase the ball's blue ambient reflectance.
// Press 'd/D' to decrease/increase the ball's blue diffuse reflectance.
// Press 's/S' to decrease/increase the ball's white specular reflectance.
// Press 'h/H' to decrease/increase the ball's shininess.
// Press 'e/E' to decrease/increase the ball's blue emittance.
// Press the page up/down keys to move the ball.
// Press 'r' to reset the ball to its original position.
//
// Sumanta Guha.
//////////////////////////////////////////////////////////////////////////////////////// 

#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

// Globals.
static float a = 1.0; // Blue ambient reflectance.
static float d = 1.0; // Blue diffuse reflectance.
static float s = 1.0; // White specular reflectance. 
static float h = 50.0; // Shininess.
static float e = 0.0; // Blue emittance.
static float zMove = 0.0; // z-direction component.
static char theStringBuffer[10]; // String buffer.
static long font = (long)GLUT_BITMAP_8_BY_13; // Font selection.

// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string)
{
	char *c;

	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// Routine to convert floating point to char string.
void floatToString(char * destStr, int precision, float val)
{
	sprintf(destStr, "%f", val);
	destStr[precision] = '\0';
}

// Write data.
void writeData(void)
{
	glDisable(GL_LIGHTING); // Disable lighting.
	glColor3f(1.0, 1.0, 1.0);

	floatToString(theStringBuffer, 4, a);
	glRasterPos3f(-1.0, 1.05, -2.0);
	writeBitmapString((void*)font, "Blue ambient reflectance: ");
	writeBitmapString((void*)font, theStringBuffer);

	floatToString(theStringBuffer, 4, d);
	glRasterPos3f(-1.0, 1.0, -2.0);
	writeBitmapString((void*)font, "Blue diffuse reflectance: ");
	writeBitmapString((void*)font, theStringBuffer);

	floatToString(theStringBuffer, 4, s);
	glRasterPos3f(-1.0, 0.95, -2.0);
	writeBitmapString((void*)font, "White specular reflectance: ");
	writeBitmapString((void*)font, theStringBuffer);

	floatToString(theStringBuffer, 4, h);
	glRasterPos3f(-1.0, 0.9, -2.0);
	writeBitmapString((void*)font, "Shininess: ");
	writeBitmapString((void*)font, theStringBuffer);

	floatToString(theStringBuffer, 4, e);
	glRasterPos3f(-1.0, 0.85, -2.0);
	writeBitmapString((void*)font, "Blue emittance: ");
	writeBitmapString((void*)font, theStringBuffer);

	glEnable(GL_LIGHTING); // Re-enable lighting.
}

// Initialization routine.
void setup(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST); // Enable depth testing.

	// Turn on OpenGL lighting.
	glEnable(GL_LIGHTING);

	// Light property vectors.
	float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
	float lightDifAndSpec0[] = { 1.0, 1.0, 1.0, 1.0 };
	float lightDifAndSpec1[] = { 0.0, 1.0, 0.0, 1.0 };
	float globAmb[] = { 0.2, 0.2, 0.2, 1.0 };

	// Light0 properties.
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec0);

	// Light1 properties.
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDifAndSpec1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightDifAndSpec1);

	glEnable(GL_LIGHT0); // Enable particular light source.
	glEnable(GL_LIGHT1); // Enable particular light source.
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint

	// Cull back faces.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

// Drawing routine.
void drawScene()
{
	// Light position vectors.	
	float lightPos0[] = { 0.0, 2.0, 2.5, 1.0 };
	float lightPos1[] = { 1.0, 2.0, 0.0, 1.0 };

	// Material property vectors.
	float matAmb[] = { 0.0, 0.0, a, 1.0 };
	float matDif[] = { 0.0, 0.0, d, 1.0 };
	float matSpec[] = { s, s, s, 1.0 };
	float matShine[] = { h };
	float matEmission[] = { 0.0, 0.0, e, 1.0 };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	writeData();

	gluLookAt(0.0, 3.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Draw light source spheres after disabling lighting.
	glDisable(GL_LIGHTING);

	// Light0 and its sphere positioned.
	glPushMatrix();
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	glTranslatef(lightPos0[0], lightPos0[1], lightPos0[2]);
	glColor3f(1.0, 1.0, 1.0);
	glutWireSphere(0.05, 8, 8);
	glPopMatrix();

	// Light1 and its sphere positioned.
	glPushMatrix();
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	glTranslatef(lightPos1[0], lightPos1[1], lightPos1[2]);
	glColor3f(0.0, 1.0, 0.0);
	glutWireSphere(0.05, 8, 8);
	glPopMatrix();

	glEnable(GL_LIGHTING);

	// Material properties of ball.
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);

	// Ball.
	glTranslatef(0.0, 0.0, zMove); // Move the ball.
	glutSolidSphere(1.5, 200, 200);

	glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
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
	case 27:
		exit(0);
		break;
	case 'a':
		if (a > 0.0) a -= 0.05;
		glutPostRedisplay();
		break;
	case 'A':
		if (a < 1.0) a += 0.05;
		glutPostRedisplay();
		break;
	case 'd':
		if (d > 0.0) d -= 0.05;
		glutPostRedisplay();
		break;
	case 'D':
		if (d < 1.0) d += 0.05;
		glutPostRedisplay();
		break;
	case 's':
		if (s > 0.0) s -= 0.05;
		glutPostRedisplay();
		break;
	case 'S':
		if (s < 1.0) s += 0.05;
		glutPostRedisplay();
		break;
	case 'e':
		if (e > 0.0) e -= 0.05;
		glutPostRedisplay();
		break;
	case 'E':
		if (e < 1.0) e += 0.05;
		glutPostRedisplay();
		break;
	case 'h':
		if (h > 0.0) h -= 1.0;
		glutPostRedisplay();
		break;
	case 'H':
		if (h < 128.0) h += 1.0;
		glutPostRedisplay();
		break;
	case 'r':
		zMove = 0.0; 
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
	if (key == GLUT_KEY_PAGE_UP) zMove -= 0.1;
	if (key == GLUT_KEY_PAGE_DOWN) if (zMove < 3.0) zMove += 0.1;
	glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press 'a/A' to decrease/increase the ball's blue ambient reflectance." << std::endl
		<< "Press 'd/D to decrease/increase the ball's blue diffuse reflectance." << std::endl
		<< "Press 's/S' to decrease/increase the ball's white specular reflectance." << std::endl
		<< "Press 'h/H' to decrease/increase the ball's shininess." << std::endl
		<< "Press 'e/E' to decrease/increase the ball's blue emittance." << std::endl
		<< "Press the page up/down keys to move the ball." << std::endl
		<< "Press 'r' to reset the ball to its original position." << std::endl;
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
	glutCreateWindow("lightAndMaterial1.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
