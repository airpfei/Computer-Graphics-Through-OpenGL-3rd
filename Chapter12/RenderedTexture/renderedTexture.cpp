/////////////////////////////////////////////////////////////////////////////
// renderedTexture.cpp
//
// This programs modifies litTexturedCylinder.cpp by replacing the can label
// with a texture rendered with the animation of ballAndTorus.cpp. 
// Additional changes: lighting has been removed from litTexturedCylinder.cpp 
// and the torus of ballAndTorus.cpp has been replaced with a long cylinder.
//
// Interaction:
// Press x, X, y, Y, z, Z to turn the cylinder.
// Press space to toggle between animation on and off.
// 
// Sumanta Guha.
//
// Texture Credits: See ExperimenterSource/Textures/TEXTURE_CREDITS.txt
/////////////////////////////////////////////////////////////////////////////
 
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#include "getBMP.h"

#define PI 3.14159265358979324

// Globals.

// litTexturedCylinder.cpp globals (normals excluded).
static int p = 30; // Number of grid columns.
static int q = 10; // Number of grid rows
static float *vertices = NULL; // Vertex array of the mapped sample on the cylinder.
static float *textureCoords = NULL; // Texture co-ordinates array of the mapped sample on the cylinder.
static float Xangle = 210.0, Yangle = 230.0, Zangle = 0.0; // Angles to rotate cylinder.
static unsigned int texture[2]; // Array of texture ids.

// ballAndTorus.cpp globals (longAngle replaced with dist because ball's longitudinal rotation around
// torus has been replaced with translation parallel to cylinder); scene rotation variables excluded; 
// quadric object id variable included to make cylinder).
static float latAngle = 0.0; // Latitudinal angle.
static float dist = 0.0; // Translation amount.
static GLUquadricObj *qobj; // Pointer to a quadric object.
static int isAnimate = 0; // Animated?
static int animationPeriod = 100; // Time interval between frames.

// Additional globals to manage FBO and renderbuffer and store window size.
static unsigned int framebuffer; // FBO id.
static unsigned int framebufferWidth = 512; // FBO width.
static unsigned int framebufferHeight = 512; // FBO height.
static unsigned int renderbuffer; // Id of renderbuffer to be used as a depth buffer.
static unsigned int windowWidth, windowHeight; // Width, height of OpenGL window.

// Load external textures.
void loadTextures()			
{
   // Local storage for bmp image data.
   imageFile *image[1]; 
   
   // Load the textures.
   image[0] = getBMP("../../Textures/canTop.bmp");     

   // Bind can top image to texture[1].
   glBindTexture(GL_TEXTURE_2D, texture[1]);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0, 
	            GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

// Fuctions to map the grid vertex (u_i,v_j) to the mesh vertex (f(u_i,v_j), g(u_i,v_j), h(u_i,v_j)) on the can cylinder.
float f(int i, int j)
{
   return ( cos( (-1 + 2*(float)i/p) * PI ) );
}

float g(int i, int j)
{
   return ( sin( (-1 + 2*(float)i/p) * PI ) );
}

float h(int i, int j)
{
   return ( -1 + 2*(float)j/q );
}

// Routine to fill the vertex array with co-ordinates of the mapped sample points.
void fillVertexArray(void)
{
   int i, j, k;

   k = 0;
   for (j = 0; j <= q; j++)
      for (i = 0; i <= p; i++)
      {
         vertices[k++] = f(i,j);
         vertices[k++] = g(i,j);
         vertices[k++] = h(i,j);
      }
}

// Routine to fill the texture co-ordinates array with the texture co-ordinate values at the mapped sample points.
void fillTextureCoordArray(void)
{
   int i, j, k;

   k = 0;
   for (j = 0; j <= q; j++)
      for (i = 0; i <= p; i++)
      {
         textureCoords[k++] = (float)i/p;
         textureCoords[k++] = (float)j/q;
      }
}

// Initialization routine.
void setup(void)
{
   glEnable(GL_DEPTH_TEST);  

   // Enable vertex and texture coordinate arrays for the can.
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   // Allocate space for vertex and texture coordinates arrays.
   vertices = new float[3*(p+1)*(q+1)];  
   textureCoords = new float[2*(p+1)*(q+1)]; 

   // Set the array pointers.
   glVertexPointer(3, GL_FLOAT, 0, vertices);
   glTexCoordPointer(2, GL_FLOAT, 0, textureCoords);  

   // Fill the vertex and texture co-ordinates arrays.
   fillVertexArray();
   fillTextureCoordArray();

   // Create texture ids. 
   glGenTextures(2, texture);

   // Load external textures.
   loadTextures();

   // Specify how texture values combine with current surface color values.
   // GL_MODULATE of litTexturedCylinder.cpp has been replaced with GL_REPLACE
   // because there is no color/light.
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 

   // Turn on OpenGL texturing.
   glEnable(GL_TEXTURE_2D); 

   // Create new quadric object.
   qobj = gluNewQuadric();

   // Specify that quadrics are drawn in wireframe.
   gluQuadricDrawStyle(qobj, GLU_LINE); 

   // Bind texture to be rendered.
   glBindTexture(GL_TEXTURE_2D, texture[0]);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebufferWidth, framebufferHeight, 0, 
	            GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   // Create id for renderbuffer, bind it and specify storage for depth values.
   glGenRenderbuffers(1, &renderbuffer);
   glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebufferWidth, framebufferHeight);

   // Create id for framebuffer object, bind it and attach texture[0] as the color buffer and renderbuffer as the depth buffer.
   glGenFramebuffers(1, &framebuffer);
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture[0], 0);
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
}

void drawTextureScene(void)
{
   glDisable(GL_TEXTURE_2D); // Want color in this scene, not texture.
   glClearColor(1.0, 1.0, 0.0, 0.0); // Clearing color for the texture scene is different from that of the OpenGL window.

   // ballAndTorus.cpp's resize() routine.
   glViewport(0, 0, framebufferWidth, framebufferHeight);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
   glMatrixMode(GL_MODELVIEW);

   // Rest of the code is ballAndTorus.cpp's (modified) drawScene() routine.
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();
   glTranslatef(0.0, 0.0, -25.0);

   // Fixed cylinder (instead of torus).
   glPushMatrix();
   glTranslatef(-25.0, 0.0, 0.0);
   glRotatef(90.0, 0.0, 1.0, 0.0);
   glColor3f(0.0, 1.0, 0.0);
   gluCylinder(qobj, 4.0, 4.0, 50.0, 15, 15);
   glPopMatrix();

   // Begin revolving ball (which translates parallel to the cylinder as it rotates about it).
   glPushMatrix();
   glTranslatef(dist, 0.0, 0.0);
   glRotatef(latAngle, 1.0, 0.0, 0.0);
   glTranslatef(0.0, -15.0, 0.0);
   glColor3f(0.0, 0.0, 1.0);
   gluSphere(qobj, 2.0, 10, 10);
   glPopMatrix();
   // End revolving ball.

   glEnable(GL_TEXTURE_2D); // Restore texturing.
}

// Drawing routine.
void drawScene(void)
{
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); // Bind FBO.
   drawTextureScene(); // Draw into FBO.

   glBindFramebuffer(GL_FRAMEBUFFER, NULL); //Restore default framebuffer.
   
   glClearColor(1.0, 1.0, 1.0, 0.0); // Clearing color for the OpenGL window.

   // litTexturedCylinder.cpp's resize() routine.
   glViewport(0, 0, windowWidth, windowHeight); 
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60.0, (float)windowWidth/(float)windowHeight, 1.0, 50.0);
   glMatrixMode(GL_MODELVIEW);

   // Rest of the code is litTexturedCylinder.cpp's drawScene() routine except texture[0]
   // is now rendered instead of canLabel, there are no normal calls.
   int  i, j;

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glLoadIdentity();
   gluLookAt(0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

   // Commands to turn the cylinder.
   glRotatef(Zangle, 0.0, 0.0, 1.0);
   glRotatef(Yangle, 0.0, 1.0, 0.0);
   glRotatef(Xangle, 1.0, 0.0, 0.0);

   // Map rendered texture onto the cylinder.
   glBindTexture(GL_TEXTURE_2D, texture[0]);
   for(j = 0; j < q; j++)
   {
      glBegin(GL_TRIANGLE_STRIP);
      for(i = 0; i <= p; i++)
      {
         glArrayElement( (j+1)*(p+1) + i );
         glArrayElement( j*(p+1) + i );
	  }
      glEnd();
   }

   // Map the can top texture onto a disc at the top of the cylinder.
   glBindTexture(GL_TEXTURE_2D, texture[1]);
   glBegin(GL_POLYGON);
   glTexCoord2f(0.0, 0.0);
   for(i = 0; i <= p; i++)
   {
	  glTexCoord2f( 0.5 + 0.5*cos( (-1 + 2*(float)i/p) * PI ), 0.5 + 0.5*sin( (-1 + 2*(float)i/p) * PI ) );
      glVertex3f( cos( (-1 + 2*(float)i/p) * PI ), sin( (-1 + 2*(float)i/p) * PI ), 1.0 );   
   }
   glEnd();

   glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
   // drawScene() has its own copy of (litTexturedCylinder.cpp's) resize() routine so
   // only function of this routine is to supply window dimensions to globals.
   windowWidth = w;
   windowHeight = h;
}

// Timer function (modified from ballAndTorus.cpp replacing longAngle with dist).
void animate(int value)
{
   if (isAnimate) 
   {
      latAngle += 5.0;
	  if (latAngle > 360.0) latAngle -= 360.0;
      dist += 0.1;
	  if (dist > 29.0) dist -= 50.0;

	  glutPostRedisplay();
      glutTimerFunc(animationPeriod, animate, 1);
   }
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
   switch(key) 
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

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
   std::cout << "Interaction:" << std::endl;
   std::cout << "Press x, X, y, Y, z, Z to turn the cylinder." << std::endl
	    << "Press space to toggle between animation on and off." << std::endl;
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
   glutCreateWindow("renderedTexture.cpp");
   glutDisplayFunc(drawScene);
   glutReshapeFunc(resize);
   glutKeyboardFunc(keyInput);

   glewExperimental = GL_TRUE;
   glewInit();

   setup(); 

   glutMainLoop();
}

