//////////////////////////////////////////////////////////////////////////////////////////////////
// ballAndTorusShadowMapped.cpp
//
// This program applies shadow mapping to a scene with a single local light source (except for the
// light source the scene is identical to that of BallAndTorusLitProjectivelyShadowed.cpp).
//
// Interaction:
// Press space to toggle between animation on and off.
// Press the up/down arrow keys to speed up/slow down animation.
//
// Sumanta Guha
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

// Begin globals. 
static float latAngle = 0.0; // Latitudinal angle.
static float longAngle = 0.0; // Longitudinal angle.
static int isAnimate = 0; // Animated?
static int animationPeriod = 100; // Time interval between frames.

static int winWth, winHgt; // OpenGL window sizes.

static unsigned int shadowMap; // Shadow map.
static int shadowMapWth = 512; // Shadow map width.
static int shadowMapHgt = 512; // Shadow map height.

// Camera properties.
static float cameraFovy = 90.0; // gluPerspective from camera: field of view angle.
static float cameraNearPlane = 5.0; // gluPerspective from camera: near plane.
static float cameraFarPlane = 100.0; // gluPerspective from camera: far plane.
static float cameraPos[] = { 0.0, 36.0, 7.0 }; // Camera position.
static float cameraLookAt[] = { 0.0, 0.0, 0.0 }; // gluLookAt from camera: point looked at (i.e., center).
static float cameraUp[] = { 0.0, 1.0, 0.0 }; // gluLookAt from camear: up direction.

// Light properties.
static float lightFovy = 120.0; // gluPerspective from light: field of view angle.
static float lightAspect = 1.0; // gluPerspective from light: aspect ratio.
static float lightNearPlane = 1.0; // gluPerspective from light: near plane.
static float lightFarPlane = 35.0; // gluPerspective from light: far plane.
static float lightPos[] = { 0.0, 30.0, 0.0 }; // Light position.
static float lightLookAt[] = { 0.0, 0.0, 0.0 }; // gluLookAt from light: point looked at (i.e., center).
static float lightUp[] = { 1.0, 0.0, 0.0 }; // gluLookAt from light: up direction.
static float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 }; // Light ambient values.
static float lightDifAndSpec[] = { 1.0, 1.0, 1.0, 1.0 }; // Light diffuse and specular values.
static float globAmb[] = { 0.3, 0.3, 0.3, 1.0 }; // Global ambient values.

// Material properties.
float matSpec[] = { 1.0, 1.0, 1.0, 1.0 }; // Material specular values.
float matShine[] = { 50.0 }; // Material shininess.

// Matrices (4x4 matrices each written in a 1-dim array in column-major order).
static float cameraProjMat[16]; // Camera's projection transformation matrix.  
static float cameraViewMat[16]; // Camera's viewing transformation matrix.  
static float lightProjMat[16]; // Light's projection transformation matrix.
static float lightViewMat[16]; // Light's viewing transformation matrix.  
static float interMat[16]; // Intermediate matrix.
//End globals.

// Draw ball flying around torus.
void drawFlyingBallAndTorus(void)
{
	glShadeModel(GL_SMOOTH);

	glPushMatrix();

	glTranslatef(0.0, 10.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);

	// Fixed torus.
	glColor3f(0.0, 1.0, 0.0);
	glutSolidTorus(2.0, 12.0, 80, 80);

	// Begin revolving ball.
	glRotatef(longAngle, 0.0, 0.0, 1.0);

	glTranslatef(12.0, 0.0, 0.0);
	glRotatef(latAngle, 0.0, 1.0, 0.0);
	glTranslatef(-12.0, 0.0, 0.0);

	glTranslatef(20.0, 0.0, 0.0);

	glColor3f(0.0, 0.0, 1.0);
	glutSolidSphere(2.0, 20, 20);
	// End revolving ball.

	glPopMatrix();
}

// Draw checkered floor.
void drawCheckeredFloor(void)
{
	glShadeModel(GL_FLAT); // Flat shading to get the checkered pattern.
	int i = 0;

	glPushMatrix();

	for (float z = 50.0; z > -50.0; z -= 5.0)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (float x = -50.0; x < 50.0; x += 5.0)
		{
			if (i % 2) glColor3f(0.0, 0.5, 0.5);
			else glColor3f(1.0, 1.0, 1.0);
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(x, 0.0, z - 5.0);
			glVertex3f(x, 0.0, z);
			i++;
		}
		glEnd();
		i++;
	}

	glPopMatrix();
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

// Initialization routine.
void setup(void)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(1.0, 1.0, 1.0, 0.0);

	// Need to set the depth test to <=, rather than the default <, so that the light-source lit regions 
	// in the third drawing pass can overwrite their ambiently lit versions from the second.
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	//Create the shadow map (depth texture).
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWth, shadowMapHgt, 0,
		GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	// Local light source properties.
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	// Global ambient light.
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);

	// Material properties.
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);

	// Enable color material mode.
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Enable texturing.
	glEnable(GL_TEXTURE_2D);

	// Enable face culling.
	glEnable(GL_CULL_FACE);

	// Compute and save matrices.
	// All computations, even projection matrix, are done in the modelview matrix stack.
	glPushMatrix();

	// Camera's projection transformation matrix.  
	glLoadIdentity();
	gluPerspective(cameraFovy, (float)winWth / winHgt, cameraNearPlane, cameraFarPlane);
	glGetFloatv(GL_MODELVIEW_MATRIX, cameraProjMat);

	// Camera's viewing transformation matrix.  
	glLoadIdentity();
	gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2], cameraLookAt[0], cameraLookAt[1], cameraLookAt[2],
		      cameraUp[0], cameraUp[1], cameraUp[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX, cameraViewMat);

	// Light's projection transformation matrix.  
	glLoadIdentity();
	gluPerspective(lightFovy, lightAspect, lightNearPlane, lightFarPlane);
	glGetFloatv(GL_MODELVIEW_MATRIX, lightProjMat);

	// Light's viewing transformation matrix.  
	glLoadIdentity();
	gluLookAt(lightPos[0], lightPos[1], lightPos[2], lightLookAt[0], lightLookAt[1], lightLookAt[2],
		      lightUp[0], lightUp[1], lightUp[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX, lightViewMat);

	glPopMatrix();
}

// Drawing routine.
void drawScene(void)
{
	/***************************************************************/
	// FIRST PASS: Draw the scene from the light's viewpoint and capture the z-buffer in the shadow map.
	/***************************************************************/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load the light's projection transformation matrix.
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(lightProjMat);

	// Load the light's viewing transformation matrix.
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(lightViewMat);

	// Create viewport same size as the shadow map.
	glViewport(0, 0, shadowMapWth, shadowMapHgt);

	// Disable writes to the color buffer so that the scene is not rendered to the window.
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// Cull front faces so that only depths of back faces are written to the z-buffer
	// as only back (from the light's viewpoint) faces should determine shadows.
	glCullFace(GL_FRONT);

	// Draw the scene.
	drawCheckeredFloor();
	drawFlyingBallAndTorus();

	// Capture the depth buffer in the shadow map.
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, shadowMapWth, shadowMapHgt, 0);

	/***************************************************************/
	// SECOND PASS: Draw the scene from the camera's viewpoint using only global ambient light,
	// being the light which should illuminate all regions, shadowed or not.
	/***************************************************************/
	glClear(GL_DEPTH_BUFFER_BIT);

	// Load the camera's projection transformation matrix.
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cameraProjMat);

	// Load the camera's viewing transformation matrix.
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(cameraViewMat);

	// Create viewport same size as the OpenGL window.
	glViewport(0, 0, winWth, winHgt);

	// Cull back faces (not visible to the camera).
	glCullFace(GL_BACK);

	// Allow the scene to be rendered.
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Enable lighting. This enables ONLY global ambient light as LIGHT0 is NOT enabled.
	glEnable(GL_LIGHTING);

	// Draw the scene.
	drawCheckeredFloor();
	drawFlyingBallAndTorus();

	/***************************************************************/
	// THIRD PASS: Draw the scene from the camera's viewpoint, turn on
	// the light source, but draw only non-shadowed fragments.
	/***************************************************************/
	glEnable(GL_LIGHT0); // LIGHT0 is the local light source.

	// Begin computing the intermediate matrix. Computations are done in the modelview matrix stack.
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// These two transformations generate the bias matrix B.
	glTranslatef(0.5, 0.5, 0.5);
	glScalef(0.5, 0.5, 0.5); // MV matrix value = B

	glMultMatrixf(lightProjMat); // MV matrix value = B x P_L
	glMultMatrixf(lightViewMat); // MV matrix value = B x P_L x V_L

	// Save the current modelview matrix as intermediate matrix.
	glGetFloatv(GL_MODELVIEW_MATRIX, interMat);

	glPopMatrix();

	// List the rows of the intermediate matrix noting that it is stored in interMat in column-major order.
	float interMatRow0[] = { interMat[0], interMat[4], interMat[8], interMat[12] };
	float interMatRow1[] = { interMat[1], interMat[5], interMat[9], interMat[13] };
	float interMatRow2[] = { interMat[2], interMat[6], interMat[10], interMat[14] };
	float interMatRow3[] = { interMat[3], interMat[7], interMat[11], interMat[15] };

	// Generate texture coordinates equal to B x P_L x V_L x (V_C)^{-1} x vertex.
	glEnable(GL_TEXTURE_GEN_S);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_S, GL_EYE_PLANE, interMatRow0);

	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_T, GL_EYE_PLANE, interMatRow1);

	glEnable(GL_TEXTURE_GEN_R);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_R, GL_EYE_PLANE, interMatRow2);

	glEnable(GL_TEXTURE_GEN_Q);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_Q, GL_EYE_PLANE, interMatRow3);

	// Activate the shadow map.
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	// Compare the R-coordinate to the corresponding value stored in the shadow map.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	// Comparison should be true if r <= texture, i.e., if the fragment is not shadowed.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// Set shadow comparison to generate an alpha value (0 for failed comparisons, 1 for successful).
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_ALPHA);

	// Set alpha test to discard failed comparisons, i.e., shadowed fragments.
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);

	// Draw the scene.
	drawCheckeredFloor();
	drawFlyingBallAndTorus();

	// Restore states.
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	/***************************************************************/
	// FOURTH PASS: Draw a red sphere at the light's position. 
	/***************************************************************/
	glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
	glColor3f(1.0, 0.0, 0.0);
	glutWireSphere(0.2, 10, 10);

	glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	// Update the new window dimensions.
	winWth = w;
	winHgt = h;

	// Update the camera's projection matrix by recomputing it (using the modelview matrix stack).
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(cameraFovy, (float)winWth / winHgt, cameraNearPlane, cameraFarPlane);
	glGetFloatv(GL_MODELVIEW_MATRIX, cameraProjMat);
	glPopMatrix();
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
	case ' ':
		if (isAnimate) isAnimate = 0;
		else
		{
			isAnimate = 1;
			animate(1);
		}
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
		<< "Press the up/down arrow keys to speed up/slow down animation." << std::endl;
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
	glutCreateWindow("ballAndTorusShadowMapped.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
