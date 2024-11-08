#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;


bool rightKeyPressed = false;
bool leftKeyPressed = false;
bool downKeyPressed = false;
bool upKeyPressed = false;

// Maze
int maze[10][10] = {
	{1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,1},
	{0,0,1,1,1,1,0,1,0,1},
	{1,0,1,0,0,0,0,1,0,1},
	{1,0,1,0,1,1,0,1,0,1},
	{1,0,0,0,0,1,0,0,0,1},
	{1,0,1,1,0,1,1,1,0,1},
	{1,0,0,0,0,0,0,1,0,0},
	{1,0,1,0,1,1,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1},
};


bool isWall(float x, float y) {
	// Convert the OpenGL coordinates to maze indices
	// Offset x by +5 to match the maze grid's center
	int col = static_cast<int>(x + 5.0f);
	// Offset y by +5 and invert to match the row indexing
	int row = static_cast<int>(5.0f - y);

	// Check that row and col are within maze boundaries
	if (row >= 0 && row < 10 && col >= 0 && col < 10) {
		// Return true if it's a wall
		return maze[row][col] == 1;  
	}
	// Treat out-of-bound positions as walls
	return true;  
}


void moveChar(float* x, float* y, GLfloat character_vertices[], GLFWwindow* window, GLuint vertexbuffer2) {
	float new_x = *x;
	float new_y = *y;

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		// Only act if the key was not pressed previously
		if (!rightKeyPressed) {
			std::cout << "Right key pressed\n";
			// Move right
			new_x += 1.0f;  
			// Set the flag to true
			rightKeyPressed = true;  
		}
	}
	else {
		// Reset flag when the key is released
		rightKeyPressed = false;
	}


	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		if (!leftKeyPressed) {
			std::cout << "Left key pressed\n";
			// Limit left movement to initial boundary
			if (*x > -4.75f) { 
				// Move left
				new_x -= 1.0f; 
			}
			// Set the flag to true
			leftKeyPressed = true;  
		}
	}

	else {
		// Reset flag when the key is released
		leftKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		if (!downKeyPressed) {
			std::cout << "Down key pressed\n";
			// Move down
			new_y -= 1.0f; 
			// Set the flag to true
			downKeyPressed = true;
		}
	}
	else {
		// Reset flag when the key is released
		downKeyPressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		if (!upKeyPressed) {
			std::cout << "Up key pressed\n";
			// Move right
			new_y += 1.0f;  
			// Set the flag to true
			upKeyPressed = true;
		}
	}
	else {
		// Reset flag when the key is released
		upKeyPressed = false;
	}
	// Check for walls
	if (!isWall(new_x, new_y)) {
		*x = new_x;
		*y = new_y;

		// Character's coordinates update
		character_vertices[0] = *x;
		character_vertices[1] = *y + 0.5f;
		character_vertices[2] = *x;
		character_vertices[3] = *y;
		character_vertices[4] = *x + 0.5f;
		character_vertices[5] = *y;
		character_vertices[6] = *x;
		character_vertices[7] = *y + 0.5f;
		character_vertices[8] = *x + 0.5f;
		character_vertices[9] = *y + 0.5f;
		character_vertices[10] = *x + 0.5f;
		character_vertices[11] = *y;

		// Buffer update for new coordinates
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, character_vertices, GL_STATIC_DRAW);
	}
}


// LoadShaders is our black box for now
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}


int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(750, 750, u8"Άσκηση 1Α - 2024", NULL, NULL); // ορίζω το μέγεθοσ αλλά και τον τίτλο


	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	// Needed for core profile
	glewExperimental = true; 
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID = LoadShaders("ProjectVertexShader.vertexshader", "ProjectFragmentShader.fragmentshader");

	// camera
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	glm::mat4 Projection = glm::perspective(glm::radians(30.0f), 4.0f / 4.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 30), // Camera  in World Space
		glm::vec3(0, 0, 0),  // and looks at the origin
		glm::vec3(0, 1, 0) 
	);

	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	static const GLfloat shape_1_buffer[] = {
		// triangle 1
		-5.0f , 5.0f,	-5.0f , 4.0f,	-4.0f , 4.0f,
		// triangle 2
		-4.0f , 4.0f,	-4.0f , 5.0f,	-5.0f , 5.0f,
		// triangle 3
		-4.0f , 5.0f,	-4.0f , 4.0f,	-3.0f , 4.0f,
		// triangle 4
		-3.0f , 4.0f,	-4.0f , 5.0f,	-3.0f , 5.0f,
		// triangle 5
		-3.0f , 5.0f,	-3.0f , 4.0f,	-2.0f , 4.0f,
		// triangle 6
		-3.0f , 5.0f,	-2.0f , 5.0f,	-2.0f , 4.0f,
		// triangle 7
		-2.0f , 5.0f,	-2.0f , 4.0f,	-1.0f , 4.0f,
		// triangle 8
		-2.0f , 5.0f,	-1.0f , 4.0f,	-1.0f , 5.0f,
		// triangle 9
		-1.0f , 5.0f,	-1.0f , 4.0f,	0.0f , 4.0f,
		// triangle 10
		-1.0f , 5.0f,	0.0f , 4.0f,	0.0f , 5.0f,
		// triangle 11
		0.0f , 4.0f,	0.0f , 5.0f,	1.0f , 4.0f,
		// triangle 12
		1.0f , 5.0f,	1.0f , 4.0f,	0.0f , 5.0f,
		// triangle 13
		1.0f , 5.0f,	1.0f , 4.0f,	2.0f , 4.0f,
		// triangle 14
		1.0f , 5.0f,	2.0f , 4.0f,	2.0f , 5.0f,
		// triangle 15
		2.0f , 5.0f,	2.0f , 4.0f,	3.0f , 4.0f,
		// triangle 16
		2.0f , 5.0f,	3.0f , 5.0f,	3.0f , 4.0f,
		// triangle 17
		3.0f , 5.0f,	3.0f , 4.0f,	4.0f , 4.0f,
		// triangle 18
		3.0f , 5.0f,	4.0f , 4.0f,	4.0f , 5.0f,
		// triangle 19
		4.0f , 5.0f,	4.0f , 4.0f,	5.0f , 4.0f,
		// triangle 20
		4.0f , 5.0f,	5.0f , 4.0f,	5.0f , 5.0f,
		// triangle 21
		-5.0f , 4.0f,	-5.0f , 3.0f,  	-4.0f , 3.0f,
		// triangle 22
		-5.0f , 4.0f,	-4.0f , 4.0f,	-4.0f , 3.0f,
		// triangle 23
		4.0f , 4.0f,	4.0f , 3.0f,	5.0f , 3.0f,
		// triangle 24
		4.0f , 4.0f,	5.0f , 4.0f,	5.0f , 3.0f,
		// triangle 25
		-3.0f , 3.0f,	-3.0f , 2.0f,	-2.0f , 2.0f,
		// triangle 26
		-3.0f , 3.0f,	-2.0f , 3.0f,	-2.0f , 2.0f,
		// triangle 27
		-2.0f , 3.0f,	-2.0f , 2.0f,	-1.0f , 2.0f,
		// triangle 28
		-2.0f , 3.0f,	-1.0f , 3.0f,	-1.0f , 2.0f,
		// triangle 29
		0.0f , 2.0f,	-1.0f , 3.0f,	-1.0f , 2.0f,
		// triangle 30
		0.0f , 2.0f,	-1.0f , 3.0f,	0.0f , 3.0f,
		// triangle 31
		0.0f , 2.0f,	1.0f , 2.0f,	0.0f , 3.0f,
		// triangle 32
		1.0f , 3.0f,	1.0f , 2.0f,	0.0f , 3.0f,
		// triangle 33
		2.0f , 3.0f,	2.0f , 2.0f,	3.0f , 2.0f,
		// triangle 34
		2.0f , 3.0f,	3.0f , 3.0f,	3.0f , 2.0f,
		// triangle 35
		4.0f , 3.0f,	4.0f , 2.0f,	5.0f , 2.0f,
		// triangle 36
		4.0f , 3.0f,	5.0f , 3.0f,	5.0f , 2.0f,
		// triangle 37
		-5.0f , 2.0f,	-5.0f , 1.0f,	-4.0f , 1.0f,
		// triangle 38
		-5.0f , 2.0f,	-4.0f , 2.0f,	-4.0f , 1.0f,
		// triangle 39
		-3.0f , 2.0f,	-3.0f , 1.0f,	-2.0f , 1.0f,
		// triangle 40
		-3.0f , 2.0f,	-2.0f , 2.0f,	-2.0f , 1.0f,
		// triangle 41
		2.0f , 2.0f,	2.0f , 1.0f,	3.0f , 1.0f,
		// triangle 42
		2.0f , 2.0f,	3.0f , 2.0f,	3.0f , 1.0f,
		// triangle 43
		4.0f , 2.0f,	4.0f , 1.0f,	5.0f , 1.0f,
		// triangle 44
		4.0f , 2.0f,	5.0f , 2.0f,	5.0f , 1.0f,
		// triangle 45
		-5.0f , 1.0f,	-5.0f , 0.0f,	-4.0f , 0.0f,
		// triangle 46
		-3.0f , 1.0f,	-3.0f , 0.0f,	-2.0f , 0.0f,
		// triangle 47
		-3.0f , 1.0f,	-2.0f , 1.0f,	-2.0f , 0.0f,
		// triangle 48
		-1.0f , 1.0f,	-1.0f , 0.0f,	0.0f , 0.0f,
		// triangle 49
		-1.0f , 1.0f,	0.0f , 1.0f,	0.0f , 0.0f,
		// triangle 50
		0.0f , 1.0f,	0.0f , 0.0f,	1.0f , 0.0f,
		// triangle 51
		0.0f , 1.0f,	1.0f , 1.0f,	1.0f , 0.0f,
		// triangle 52
		2.0f , 1.0f,	2.0f , 0.0f,	3.0f , 0.0f,
		// triangle 53
		2.0f , 1.0f,	3.0f , 1.0f,	3.0f , 0.0f,
		// triangle 54
		4.0f , 1.0f,	4.0f , 0.0f,	5.0f , 0.0f,
		// triangle 55
		4.0f , 1.0f,	5.0f , 1.0f,	5.0f , 0.0f,
		// triangle 56
		-5.0f , 0.0f,	-5.0f , -1.0f,	-4.0f , -1.0f,
		// triangle 57
		-5.0f , 0.0f,	-4.0f , 0.0f,	-4.0f , -1.0f,
		// triangle 58
		0.0f , 0.0f,	0.0f , -1.0f,	1.0f , -1.0f,
		// triangle 59
		0.0f , 0.0f,	1.0f , 0.0f,	1.0f , -1.0f,
		// triangle 60
		4.0f , 0.0f,	4.0f , -1.0f,	5.0f , -1.0f,
		// triangle 61
		4.0f , 0.0f,	5.0f , 0.0f,	5.0f , -1.0f,
		// triangle 62
		-5.0f , 1.0f,	-4.0f , 1.0f,	-4.0f , 0.0f,
		// triangle 63
		-5.0f , -1.0f,	-5.0f , -2.0f,	-4.0f , -2.0f,
		// triangle 64
		-5.0f , -1.0f,	-4.0f , -1.0f,	-4.0f , -2.0f,
		// triangle 65
		-3.0f , -1.0f,	-3.0f , -2.0f,	-2.0f , -2.0f,
		// triangle 66
		-3.0f , -1.0f,	-2.0f , -1.0f,	-2.0f , -2.0f,
		// triangle 67
		-2.0f , -1.0f,	-2.0f , -2.0f,	-1.0f , -2.0f,
		// triangle 68
		-2.0f , -1.0f,	-1.0f , -1.0f,	-1.0f , -2.0f,
		// triangle 69
		0.0f , -1.0f,	0.0f , -2.0f,	1.0f , -2.0f,
		// triangle 70
		0.0f , -1.0f,	1.0f , -1.0f,	1.0f , -2.0f,
		// triangle 71
		1.0f , -1.0f,	1.0f , -2.0f,	2.0f , -2.0f,
		// triangle 72
		1.0f , -1.0f,	2.0f , -1.0f,	2.0f , -2.0f,
		// triangle 73
		2.0f , -1.0f,	2.0f , -2.0f,	3.0f , -2.0f,
		// triangle 74
		2.0f , -1.0f,	3.0f , -2.0f,	3.0f , -2.0f,
		// triangle 75
		4.0f , -1.0f,	4.0f , -2.0f,	5.0f , -2.0f,
		// triangle 76
		-5.0f , -2.0f,	-5.0f , -3.0f,	-4.0f , -3.0f,
		// triangle 77
		2.0f , -2.0f,	2.0f , -3.0f,	3.0f , -3.0f,
		// triangle 78
		2.0f , -2.0f,	3.0f , -2.0f,	3.0f , -3.0f,
		// triangle 79
		-5.0f , -3.0f,	-5.0f , -4.0f,	-4.0f , -4.0f,
		// triangle 80
		-5.0f , -3.0f,	-4.0f , -3.0f,	-4.0f , -4.0f,
		// triangle 81
		-3.0f , -3.0f,	-3.0f , -4.0f,	-2.0f , -4.0f,
		// triangle 82
		-3.0f , -3.0f,	-2.0f , -3.0f,	-2.0f , -4.0f,
		// triangle 83
		-1.0f , -3.0f,	-1.0f , -4.0f,	0.0f , -4.0f,
		// triangle 84
		-1.0f , -3.0f,	0.0f , -3.0f,	0.0f , -4.0f,
		// triangle 85
		0.0f , -3.0f,	0.0f , -4.0f,	1.0f , -4.0f,
		// triangle 86
		0.0f , -3.0f,	1.0f , -3.0f,	1.0f , -4.0f,
		// triangle 87
		4.0f , -3.0f,	4.0f , -4.0f,	5.0f , -4.0f,
		// triangle 88
		4.0f , -3.0f,	5.0f , -3.0f,	5.0f , -4.0f,
		// triangle 89
		2.0f , -1.0f,	3.0f , -1.0f,	3.0f , -2.0f,
		// triangle 90
		4.0f , -1.0f,	5.0f , -1.0f,	5.0f , -2.0f,
		// triangle 91
		-5.0f , -2.0f,	-4.0f , -2.0f,	-4.0f , -3.0f,
		// triangle 92
		-5.0f , -4.0f,	-5.0f , -5.0f,	-4.0f , -5.0f,
		// triangle 93
		-5.0f , -4.0f,	-4.0f , -4.0f,	-4.0f , -5.0f,
		// triangle 94
		-4.0f , -4.0f,	-4.0f , -5.0f,	-3.0f , -5.0f,
		// triangle 95
		-4.0f , -4.0f,	-3.0f , -4.0f,	-3.0f , -5.0f,
		// triangle 96
		-3.0f , -4.0f,	-3.0f , -5.0f,	-2.0f , -5.0f,
		// triangle 97
		-3.0f , -4.0f,	-2.0f , -4.0f,	-2.0f , -5.0f,
		// triangle 98
		-2.0f , -4.0f,	-2.0f , -5.0f,	-1.0f , -5.0f,
		// triangle 99
		-2.0f , -4.0f,	-1.0f , -4.0f,	-1.0f , -5.0f,
		// triangle 100
		-1.0f , -4.0f,	-1.0f , -5.0f,	0.0f , -5.0f,
		// triangle 101
		-1.0f , -4.0f,	0.0f , -4.0f,	0.0f , -5.0f,
		// triangle 102
		0.0f , -4.0f,	1.0f , -4.0f,	1.0f , -5.0f,
		// triangle 103
		1.0f , -4.0f,	1.0f , -5.0f,	2.0f , -5.0f,
		// triangle 104
		1.0f , -4.0f,	2.0f , -4.0f,	2.0f , -5.0f,
		// triangle 105
		2.0f , -4.0f,	2.0f , -5.0f,	3.0f , -5.0f,
		// triangle 106
		2.0f , -4.0f,	3.0f , -4.0f,	3.0f , -5.0f,
		// triangle 107
		3.0f , -4.0f,	3.0f , -5.0f,	4.0f , -5.0f,
		// triangle 108
		3.0f , -4.0f,	4.0f , -4.0f,	4.0f , -5.0f,
		// triangle 109
		4.0f , -4.0f,	4.0f , -5.0f,	5.0f , -5.0f,
		// triangle 110
		4.0f , -4.0f,	5.0f , -4.0f,	5.0f , -5.0f,
		// extra triangle
		0.0f , -4.0f,	0.0f , -5.0f,	1.0f , -5.0f
	};

	float x = -4.75;
	float y = 2.25;

	// Define the vertices for the smaller blue square (character A)
	// Assuming the character A is initially at the center of the maze (0, 0)
	GLfloat character_vertices[] = {
		// triangle 1
		x , (y + 0.5f),		x , y,		(x + 0.5f) , y,
		// triangle 2
		x , (y + 0.5f),  (x + 0.5f) , (y + 0.5f),		(x + 0.5f) , y
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shape_1_buffer), shape_1_buffer, GL_STATIC_DRAW);

	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(character_vertices), character_vertices, GL_STATIC_DRAW);

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		/// camera
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);  


		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0, must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle
		// 333 indices starting at 0 -> 1 triangle
		glDrawArrays(GL_TRIANGLES, 0, 333); 

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
		glVertexAttribPointer(
			0,                  // attribute 0, must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle
		// 6 indices starting at 0 -> 1 triangle
		glDrawArrays(GL_TRIANGLES, 0, 6); 

		glDisableVertexAttribArray(0);

		moveChar(&x, &y, character_vertices, window, vertexbuffer2);

		// Draw character
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
		
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_Q) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
