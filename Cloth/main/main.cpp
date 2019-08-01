// Skeleton.cpp : Defines the entry point for the console application.
//

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <algorithm>
#include <iterator>

#include <cstdio>
#include <algorithm>

#include "shader.hpp"
#include "texture.hpp"
#include "DebugOpenGL.hpp"

const unsigned int windowWidth = 1280;
const unsigned int windowHeight = 720;

// Grid Resolution (resX=resY)
const unsigned int N = 64;
// Workgroup Size
const unsigned int Nwg = 32;
// Number of vertices (resX=resY)
const unsigned int vNum = N*N;

// Vec4 like structure
struct xyzw
{
	float x, y, z, w;
};

//compute shader
Shader vertexGravityShader;
Shader vertexCollisionShader;
Shader vertexDistanceShader;
Shader vertexBendingShader;
Shader vertexFinalUpdateShader;
//standard pipeline
Shader renderShader;

// Position buffer
GLuint positionBuffer; //x
GLuint positionBufferTmp; //p
GLuint velocityBuffer; //v
GLuint normalBuffer; //v

// Vertex array object
GLuint vao;
GLuint elementbuffer;
std::vector<unsigned int> indices;

void onInitialization()
{
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		printf("Cannot initialize GLEW\n");
		exit(-1);
	}
	glGetError();

	DebugOpenGL::init();
	DebugOpenGL::enableLowSeverityMessages(false);

	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	//glEnableClientState(GL_VERTEX_ARRAY);

	vertexGravityShader.loadShader(GL_COMPUTE_SHADER, "../shaders/gravity.comp");
	vertexGravityShader.compile();

	vertexCollisionShader.loadShader(GL_COMPUTE_SHADER, "../shaders/collision.comp");
	vertexCollisionShader.compile();

	vertexDistanceShader.loadShader(GL_COMPUTE_SHADER, "../shaders/distance.comp");
	vertexDistanceShader.compile();

	vertexBendingShader.loadShader(GL_COMPUTE_SHADER, "../shaders/bending.comp");
	vertexBendingShader.compile();

	vertexFinalUpdateShader.loadShader(GL_COMPUTE_SHADER, "../shaders/finalUpdate.comp");
	vertexFinalUpdateShader.compile();

	renderShader.loadShader(GL_VERTEX_SHADER, "../shaders/render.vert");
	renderShader.loadShader(GL_FRAGMENT_SHADER, "../shaders/render.frag");
	renderShader.compile();

	// Initialize the particle position buffer
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vNum * sizeof(xyzw), NULL, GL_STATIC_DRAW);
	xyzw* pos = (xyzw*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, vNum * sizeof(xyzw), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (unsigned int i = 0; i < N; ++i)
		for (unsigned int j = 0; j < N; ++j)
	{
		pos[j*N+i].x = (double)i / ((double)N - 1.0) - 0.5;
		pos[j*N+i].y = 0;
		pos[j*N+i].z = (double)j / ((double)N - 1.0) - 0.5;
		pos[j*N+i].w = 1.0f;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glGenBuffers(1, &positionBufferTmp);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBufferTmp);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vNum * sizeof(xyzw), NULL, GL_STATIC_DRAW);
	xyzw* posTmp = (xyzw*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, vNum * sizeof(xyzw), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (unsigned int i = 0; i < N; ++i)
		for (unsigned int j = 0; j < N; ++j)
		{
			posTmp[j*N + i].x = (double)i / ((double)N - 1.0) - 0.5;
			posTmp[j*N + i].y = 0;
			posTmp[j*N + i].z = (double)j / ((double)N - 1.0) - 0.5;
			posTmp[j*N + i].w = 1.0f;
		}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glGenBuffers(1, &velocityBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vNum * sizeof(xyzw), NULL, GL_STATIC_DRAW);
	xyzw* vel = (xyzw*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, vNum * sizeof(xyzw), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (unsigned int i = 0; i < N*N; ++i)
	{
		vel[i].x = 0;
		vel[i].y = 0;
		vel[i].z = 0;
		vel[i].w = 0;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, normalBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vNum * sizeof(xyzw), NULL, GL_STATIC_DRAW);
	xyzw* nor = (xyzw*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, vNum * sizeof(xyzw), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	for (unsigned int i = 0; i < N*N; ++i)
	{
		nor[i].x = 0;
		nor[i].y = 0;
		nor[i].z = 1;
		nor[i].w = 0;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//indices
	for (int j = 0; j < N - 1; ++j)
	{
		for (int i = 0; i < N - 1; ++i)
		{
			int row1 = j * (N);
			int row2 = (j + 1) * (N);

			// triangle 1
			indices.push_back(row1 + i);
			indices.push_back(row1 + i + 1);
			indices.push_back(row2 + i + 1);
			// triangle 2
			indices.push_back(row1 + i);
			indices.push_back(row2 + i + 1);
			indices.push_back(row2 + i);
		}
	}

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Initialize the vertex array object with the position and velocity buffers
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, sizeof(xyzw), (GLvoid*)0);


	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer((GLuint)1, 4, GL_FLOAT, GL_FALSE, sizeof(xyzw), (GLvoid*)0);
	glBindVertexArray(0);


	// Set point primitive size
	//glPointSize(2.0f);
}
const float dt = 0.01f;

void onDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vertexGravityShader.enable();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, positionBufferTmp);
	vertexGravityShader.setUniform1f("dt", dt);
	glDispatchCompute(N / Nwg, N / Nwg, 1);
	vertexGravityShader.disable();


	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

	glm::mat4 view = glm::lookAt(glm::vec3(0, 0.5, 2.5), glm::vec3(0, -0.5, 0.5), glm::vec3(0, 1, 0));
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)windowWidth/ (float)windowHeight, 0.1f, 10.0f);
	
	const float d = 1.0f / (N - 1);
	
	const int NITER = 50;
	for (int i = 0; i < NITER; ++i)
	{
		vertexCollisionShader.enable();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, positionBufferTmp);
		glDispatchCompute(N / Nwg, N / Nwg, 1);
		vertexCollisionShader.disable();

		vertexDistanceShader.enable();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, positionBufferTmp);
		vertexDistanceShader.setUniform1f("d", d);
		glDispatchCompute(N / Nwg, N / Nwg, 1);
		vertexDistanceShader.disable();
	
		vertexBendingShader.enable();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, positionBufferTmp);
		glDispatchCompute(N / Nwg, N / Nwg, 1);
		vertexBendingShader.disable();
	}
	
	vertexFinalUpdateShader.enable();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, positionBufferTmp);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, normalBuffer);
	vertexFinalUpdateShader.setUniform1f("dt", dt);
	glDispatchCompute(N / Nwg, N / Nwg, 1);
	vertexFinalUpdateShader.disable();

	// Render the particles
	renderShader.enable();
	renderShader.setUniformMat4("viewproj", proj*view);

	glBindVertexArray(vao);
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDrawArrays(GL_POINTS, 0, N*N);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(
		GL_TRIANGLES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);
	glBindVertexArray(0);
	renderShader.disable();
	if (glGetError() != GL_NO_ERROR)
	{
		std::cout << "Something is not right.";
	}
	glutSwapBuffers();
}

void onKeyboard(unsigned char key, int pX, int pY) {
	switch (key)
	{
	case 27:
		glutExit();
		break;
	}
}

void onIdle()
{
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow(argv[0]);
	glewExperimental = true;
	glewInit();

	printf("GL Vendor    : %s\n", glGetString(GL_VENDOR));
	printf("GL Renderer  : %s\n", glGetString(GL_RENDERER));
	printf("GL Version (string)  : %s\n", glGetString(GL_VERSION));
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	onInitialization();
	glutDisplayFunc(onDisplay);
	glutKeyboardFunc(onKeyboard);
	glutIdleFunc(onIdle);
	glutMainLoop();

	return 0;
}

