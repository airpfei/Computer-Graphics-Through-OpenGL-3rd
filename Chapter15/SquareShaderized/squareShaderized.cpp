////////////////////////////////////////////////////////          
// squareShaderized.cpp
//
// Forward-compatible core GL 4.3 version of square.cpp.
// 
// Sumanta Guha
////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

struct Vertex
{
	float coords[4];
	float colors[4];
};

struct Matrix4x4
{
	float entries[16];
};

static const Matrix4x4 IDENTITY_MATRIX4x4 =
{
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	}
};

static enum buffer { SQUARE_VERTICES };
static enum object { SQUARE };

// Globals
static Vertex squareVertices[] =
{
	{ { 20.0, 20.0, 0.0, 1.0 },{ 0.0, 0.0, 0.0, 1.0 } },
	{ { 80.0, 20.0, 0.0, 1.0 },{ 0.0, 0.0, 0.0, 1.0 } },
	{ { 20.0, 80.0, 0.0, 1.0 },{ 0.0, 0.0, 0.0, 1.0 } },
	{ { 80.0, 80.0, 0.0, 1.0 },{ 0.0, 0.0, 0.0, 1.0 } }
};

static Matrix4x4
modelViewMat,
projMat;

static unsigned int
programId,
vertexShaderId,
fragmentShaderId,
modelViewMatLoc,
projMatLoc,
buffer[1],
vao[1];

// Function to read external shader file.
char* readShader(std::string fileName)
{
	// Initialize input stream.
	std::ifstream inFile(fileName.c_str(), std::ios::binary);

	// Determine shader file length and reserve space to read it in.
	inFile.seekg(0, std::ios::end);
	int fileLength = inFile.tellg();
	char *fileContent = (char*)malloc((fileLength + 1) * sizeof(char));

	// Read in shader file, set last character to NULL, close input stream.
	inFile.seekg(0, std::ios::beg);
	inFile.read(fileContent, fileLength);
	fileContent[fileLength] = '\0';
	inFile.close();

	return fileContent;
}

// Initialization routine.
void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);

	// Create vertex shader object.
	char* vertexShader = readShader("Shaders/vertexShader.glsl");
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, (const char**)&vertexShader, NULL);
	glCompileShader(vertexShaderId);

	// Create fragment shader object.
	char* fragmentShader = readShader("Shaders/fragmentShader.glsl");
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, (const char**)&fragmentShader, NULL);
	glCompileShader(fragmentShaderId);

	// Create shader program executable.
	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	glUseProgram(programId);

	// Create VAO and VBO, associate data...
	glGenVertexArrays(1, vao);
	glGenBuffers(1, buffer);
	glBindVertexArray(vao[SQUARE]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[SQUARE_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);

	// ...and link to vertex shader locations.
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colors));
	glEnableVertexAttribArray(1);

	// Obtain modelview matrix uniform location and set value.
	Matrix4x4 modelViewMat = IDENTITY_MATRIX4x4;
	modelViewMatLoc = glGetUniformLocation(programId, "modelViewMat");
	glUniformMatrix4fv(modelViewMatLoc, 1, GL_TRUE, modelViewMat.entries);

	// Obtain projection matrix uniform location and set value.
	Matrix4x4 projMat =
	{
		{
			0.02, 0.0,  0.0, -1.0,
			0.0,  0.02, 0.0, -1.0,
			0.0,  0.0, -1.0,  0.0,
			0.0,  0.0,  0.0,  1.0
		}
	};
	projMatLoc = glGetUniformLocation(programId, "projMat");
	glUniformMatrix4fv(projMatLoc, 1, GL_TRUE, projMat.entries);
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glFlush();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
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
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("squareShaderized.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
