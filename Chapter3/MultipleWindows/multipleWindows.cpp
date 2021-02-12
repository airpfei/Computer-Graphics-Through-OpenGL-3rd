/////////////////////////////////////////////////////         
// multipleWindows.cpp
//
// This program creates two top-level OpenGL windows.
//
// Sumanta Guha.
///////////////////////////////////////////////////// 

#include <cmath>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#define PI 3.14159265

// Globals.
static int id1, id2; // Window identifiers.

// Drawing routine for first window.
void drawScene1(void)
{
   glClear(GL_COLOR_BUFFER_BIT);

   // A red square.
   glColor3f(1.0, 0.0, 0.0);
   glBegin(GL_POLYGON);
   glVertex3f(10.0, 10.0, 0.0);
   glVertex3f(40.0, 10.0, 0.0);
   glVertex3f(40.0, 40.0, 0.0);
   glVertex3f(10.0, 40.0, 0.0);
   glEnd();

   glFlush();
}

// Drawing routine for second window.
void drawScene2(void)
{
   glClear(GL_COLOR_BUFFER_BIT);

   // A blue circle.
   glColor3f(0.0, 0.0, 1.0);
   float angle;
   int i;
   glBegin(GL_LINE_LOOP);
   for (i = 0; i < 30; ++i)
   {
	  angle = 2 * PI * i / 30;
	  glVertex2f(50.0 + cos(angle) * 40.0, 50.0 + sin(angle) * 40.0);
   }
   glEnd();

   glFlush();
}

// Initialization routine for first window.
void setup1(void)
{
   glClearColor(1.0, 1.0, 1.0, 0.0);
}

// Initialization routine for second window.
void setup2(void)
{
   glClearColor(1.0, 1.0, 1.0, 0.0);
}

// Reshape routine for first window.
void resize1(int w, int h)
{
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

// Reshape routine for second window.
void resize2(int w, int h)
{
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

// Keyboard input processing routine shared by both windows.
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

   // First top-level window specs.
   glutInitWindowSize(250, 500);
   glutInitWindowPosition(100, 100);

   // Create the first window and return id.
   glutCreateWindow("window 1");

   // Initialization, display, resize and keyboard routines of the first window. 
   setup1();
   glutDisplayFunc(drawScene1);
   glutReshapeFunc(resize1);
   glutKeyboardFunc(keyInput); // Routine is shared by both windows.

   // Second top-level window specs.
   glutInitWindowSize(250, 500);
   glutInitWindowPosition(400, 100);

   // Create the second window and return id.
   glutCreateWindow("window 2");

   // Initialization, display, resize and keyboard routines of the second window. 
   setup2();
   glutDisplayFunc(drawScene2);
   glutReshapeFunc(resize2);
   glutKeyboardFunc(keyInput); // Routine is shared by both windows.

   glewExperimental = GL_TRUE;
   glewInit();

   glutMainLoop();
}
