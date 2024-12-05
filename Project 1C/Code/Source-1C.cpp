// Include standard headers
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cmath> 
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

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;


glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}


//flags
bool rightKeyPressed = false;
bool leftKeyPressed = false;
bool downKeyPressed = false;
bool upKeyPressed = false;

bool rightofXKeyPressed = false;
bool leftofXKeyPressed = false;
bool rightofYKeyPressed = false;
bool leftofYKeyPressed = false;
bool zoomInKeyPressed = false;
bool zoomOutKeyPressed = false;


bool isTreasureShrinking = false;
bool isTreasureVisible = true;   
double shrinkStartTime = glfwGetTime();


bool panXLeftKeyPressed = false;
bool panXRightKeyPressed = false;
bool panYUpKeyPressed = false;
bool panYDownKeyPressed = false;


float panOffsetX = 0.0f;
float panOffsetY = 0.0f;


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

int activeTextureIndex = 0;

float cameraX, cameraY, cameraZ;

void placeTreasure(int maze[10][10], float* treasureX, float* treasureY, float* treasureZ, GLfloat character_verticesB[], float* characterX, float* characterY, GLuint vertexbuffer3) {
	static double lastPlacementTime = 0.0;     // Last Placement time
	static bool isFirstAppearance = true;      // Flag for first appearence
	const double treasureDuration = 5.0;       // Treasure duration in seconds

	// Check if it is the first appearance or if the required time has elapsed.
	double currentTime = glfwGetTime();
	
	// Add a static flag to manage the random texture.
	static bool textureSelected = false;

	// Check if it is the first appearance or if the treasure needs to be refreshed.
	if (!isFirstAppearance && (currentTime - lastPlacementTime) < treasureDuration) {
		// Keep treasure's position
		return; 
	}

	std::cout << "Placing treasure...\n";

	int row, col;
	int attempts = 0;
	const int maxAttempts = 100;
	bool isOnCharacter;

	do {
		row = rand() % 10;
		col = rand() % 10;
		attempts++;

		if (attempts >= maxAttempts) {
			std::cout << "Could not find a valid position for treasure!\n";
			return;
		}

		// Calculate the treasure's position in OpenGL coordinates.
		*treasureX = col - 5.0f + 0.1f;
		*treasureY = 5.0f - row - 0.1f;

		// Check if the position is close to character A.
		isOnCharacter = (fabs((*treasureX) - *characterX) < 0.8f &&	fabs((*treasureY) - *characterY) < 0.8f);
	} while (maze[row][col] != 0 || isOnCharacter);

	// Select a random texture only if needed.
	if (!textureSelected) {
		// Random selection from 0 to 2
		activeTextureIndex = rand() % 3;  
		std::cout << "Active texture index set to: " << activeTextureIndex << std::endl;

		// Update to indicate that the texture has been selected
		textureSelected = true; 
	}

	// Update vertices
	GLfloat tempVertices[] = {
		*treasureX,*treasureY,*treasureZ,*treasureX,*treasureY - 0.8f,*treasureZ,*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ,*treasureX,*treasureY,*treasureZ,
		*treasureX + 0.8f,*treasureY,*treasureZ,*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ,*treasureX,*treasureY,*treasureZ - 0.8f,*treasureX,*treasureY - 0.8f,
		*treasureZ - 0.8f,*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ - 0.8f,*treasureX,*treasureY,*treasureZ - 0.8f,*treasureX + 0.8f,*treasureY,*treasureZ - 0.8f,
		*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ - 0.8f,*treasureX + 0.8f,*treasureY,*treasureZ,*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ,*treasureX + 0.8f,
		*treasureY - 0.8f,*treasureZ - 0.8f,*treasureX + 0.8f,*treasureY,*treasureZ,*treasureX + 0.8f,*treasureY,*treasureZ - 0.8f,*treasureX + 0.8f,*treasureY - 0.8f,
		*treasureZ - 0.8f,*treasureX,*treasureY,*treasureZ,*treasureX,*treasureY - 0.8f,*treasureZ,*treasureX,*treasureY - 0.8f,*treasureZ - 0.8f,*treasureX,*treasureY,
		*treasureZ,*treasureX,*treasureY,*treasureZ - 0.8f,*treasureX,*treasureY - 0.8f,*treasureZ - 0.8f,*treasureX,*treasureY,*treasureZ,*treasureX,*treasureY,
		*treasureZ - 0.8f,*treasureX + 0.8f,*treasureY,*treasureZ - 0.8f,*treasureX,*treasureY,*treasureZ,*treasureX + 0.8f,*treasureY,*treasureZ,*treasureX + 0.8f,*treasureY,
		*treasureZ - 0.8f,*treasureX,*treasureY - 0.8f,*treasureZ,*treasureX,*treasureY - 0.8f,*treasureZ - 0.8f,*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ - 0.8f,*treasureX,
		*treasureY - 0.8f,*treasureZ,*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ,*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ - 0.8f
	};
	std::copy(std::begin(tempVertices), std::end(tempVertices), character_verticesB);

	// Update buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tempVertices), tempVertices, GL_STATIC_DRAW);

	// Update last placement time
	lastPlacementTime = glfwGetTime();

	// Deactivation of first appearence flag
	isFirstAppearance = false;

	// Reset for the next placement
	textureSelected = false; 
}


float cameraDistance = 20.0f;
float pitch = 0.0f;
float yaw = 0.0f;


void camera_function() {
	// Set initial values for pitch, yaw, and cameraDistance so that the maze is immediately visible
	static bool initialized = false;
	if (!initialized) {
		cameraDistance = 20.0f;
		pitch = 0.0f;
		yaw = 0.0f;
		initialized = true;
	}

	// Check for rotation around the x-axis (pitch)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (!leftofXKeyPressed) {
			std::cout << "Left of x key pressed\n";
			pitch += 0.1f;
			leftofXKeyPressed = true;
		}
	} else {
		
		leftofXKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		if (!rightofXKeyPressed) {
			std::cout << "Right of x key pressed\n";
			pitch -= 0.1f;
			rightofXKeyPressed = true;
		}
	} else {
		// Reset the flag when the key is released
		rightofXKeyPressed = false;
	}

	// Check for rotation around the y-axis (yaw)
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		if (!leftofYKeyPressed) {
			std::cout << "Left of y key pressed\n";
			yaw += 0.1f;
			leftofYKeyPressed = true; 
		}
	} else {
		// Reset the flag when the key is released
		leftofYKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		if (!rightofYKeyPressed) {
			std::cout << "Right key pressed\n";
			yaw -= 0.1f;
			rightofYKeyPressed = true; 
		}
	} else {
		
		rightofYKeyPressed = false;
	}

	// Check for Zoom (Zoom In / Zoom Out)
	// Check for Zoom In
	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
		if (!zoomInKeyPressed) {
			std::cout << "Zoom in key pressed\n";
			cameraDistance -= 0.1f;

			// Zoom In limit
			if (cameraDistance < 1.0f) cameraDistance = 1.0f;
			zoomInKeyPressed = true; 
		}
	} else {
		// Reset the flag when the key is released
		zoomInKeyPressed = false;
	}

	// Check for Zoom Out
	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
		if (!zoomOutKeyPressed) {
			std::cout << "Zoom out key pressed\n";
			cameraDistance += 0.1f;

			// Zoom Out limit
			if (cameraDistance == 20.0f) cameraDistance = 20.0f;
			zoomOutKeyPressed = true;
		}
	} else {
		// Reset the flag when the key is released
		zoomOutKeyPressed = false;
	}

	// Check for panning along the x-axis (left-right)
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		if (!panXLeftKeyPressed) {
			std::cout << "Pan left key pressed\n";
			
			// Move left
			panOffsetX -= 0.1f; 
			panXLeftKeyPressed = true;
		}
	} else {
		panXLeftKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		if (!panXRightKeyPressed) {
			std::cout << "Pan right key pressed\n";
			
			// Move right
			panOffsetX += 0.1f; 
			panXRightKeyPressed = true;
		}
	} else {
		panXRightKeyPressed = false;
	}

	// Check for panning along the y-axis (up-down)
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		if (!panYUpKeyPressed) {
			std::cout << "Pan up key pressed\n";

			// Move up
			panOffsetY += 0.1f; 
			panYUpKeyPressed = true;
		}
	} else {
		panYUpKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		if (!panYDownKeyPressed) {
			std::cout << "Pan down key pressed\n";
			
			// Move down
			panOffsetY -= 0.1f;
			panYDownKeyPressed = true;
		}
	} else {
		panYDownKeyPressed = false;
	}

	cameraX = cameraDistance * cos(pitch) * sin(yaw);
	cameraY = cameraDistance * sin(pitch);
	cameraZ = cameraDistance * cos(pitch) * cos(yaw);

	// Creation of view matrix
	glm::vec3 cameraPos = glm::vec3(cameraX, cameraY, cameraZ);
	glm::vec3 target = glm::vec3(panOffsetX, panOffsetY, 0.25f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Set the camera's view projection
	ViewMatrix = glm::lookAt(cameraPos, target, up);
}


bool isWall(float x, float y, float z) {
	// Convert OpenGL coordinates to maze indices (2D)
	int col = static_cast<int>(x + 5.0f);
	int row = static_cast<int>(5.0f - y);

	// Check if the row and column are within the maze boundaries
	if (row >= 0 && row < 10 && col >= 0 && col < 10) {
		// Returns true if it is a wall
		return maze[row][col] == 1;
	}
	// If the point is out of bounds, consider it a wall
	return true;
}


void moveChar(float* x, float* y, float* z, GLfloat character_vertices[], GLFWwindow* window, GLuint vertexbuffer2) {
	float new_x = *x;
	float new_y = *y;
	float new_z = *z;

	// Corresponds to maze[2][0]
	const float start_x = -4.75f; 
	const float start_y = 2.7f;

	// Corresponds to maze[7][9]
	const float end_x = 4.25f;   
	const float end_y = -2.3f;

	// Check for movement to the right
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		if (!rightKeyPressed) {
			// Move to the right
			if (new_x == end_x && new_y == end_y) {
				std::cout << "Right key pressed\n";
				new_x = start_x;
				new_y = start_y;
			} else {
				std::cout << "Right key pressed\n";
				new_x += 1.0f;
			}
			rightKeyPressed = true;
		}
	}
	else {
		// Reset the flag when the key is released
		rightKeyPressed = false;
	}

	// Check for movement to the left
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		if (!leftKeyPressed) {
			// If at the start, move to the exit
			if (new_x == start_x && new_y == start_y) {
				std::cout << "Left key pressed\n";
				new_x = end_x;
				new_y = end_y;
			} else if (*x > -4.75f) {
				std::cout << "Left key pressed\n";
				new_x -= 1.0f;
			}
			leftKeyPressed = true;
		}
	} else {
		leftKeyPressed = false;
	}

	// Check for movement downward
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		if (!downKeyPressed) {
			std::cout << "Down key pressed\n";
			new_y -= 1.0f;
			downKeyPressed = true;
		}
	} else {
		downKeyPressed = false;
	}

	// Check for movement upward
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		if (!upKeyPressed) {
			std::cout << "Up key pressed\n";
			new_y += 1.0f;
			upKeyPressed = true;
		}
	} else {
		upKeyPressed = false;
	}

	// Check if the new position is a wall (x and y only)
	if (!isWall(new_x, new_y, new_z)) {
		*x = new_x;
		*y = new_y;
		*z = new_z;
	}

	// Update character vertices for the new 3D position
	// Simple vertices for a 3D cube (36 vertices for a cube)
	character_vertices[0] = *x;
	character_vertices[1] = *y;
	character_vertices[2] = *z;
	character_vertices[3] = *x;
	character_vertices[4] = *y - 0.5f;
	character_vertices[5] = *z;
	character_vertices[6] = *x + 0.5f;
	character_vertices[7] = *y - 0.5f;
	character_vertices[8] = *z;
	character_vertices[9] = *x;
	character_vertices[10] = *y;
	character_vertices[11] = *z;
	character_vertices[12] = *x + 0.5f;
	character_vertices[13] = *y;
	character_vertices[14] = *z;
	character_vertices[15] = *x + 0.5f;
	character_vertices[16] = *y - 0.5f;
	character_vertices[17] = *z;
	character_vertices[18] = *x;
	character_vertices[19] = *y;
	character_vertices[20] = *z - 0.5f;
	character_vertices[21] = *x;
	character_vertices[22] = *y - 0.5f;
	character_vertices[23] = *z - 0.5f;
	character_vertices[24] = *x + 0.5f;
	character_vertices[25] = *y - 0.5f;
	character_vertices[26] = *z - 0.5f;
	character_vertices[27] = *x;
	character_vertices[28] = *y;
	character_vertices[29] = *z - 0.5f;
	character_vertices[30] = *x + 0.5f;
	character_vertices[31] = *y;
	character_vertices[32] = *z - 0.5f;
	character_vertices[33] = *x + 0.5f;
	character_vertices[34] = *y - 0.5f;
	character_vertices[35] = *z - 0.5f;
	character_vertices[36] = *x + 0.5f;
	character_vertices[37] = *y;
	character_vertices[38] = *z;
	character_vertices[39] = *x + 0.5f;
	character_vertices[40] = *y - 0.5f;
	character_vertices[41] = *z;
	character_vertices[42] = *x + 0.5f;
	character_vertices[43] = *y - 0.5f;
	character_vertices[44] = *z - 0.5f;
	character_vertices[45] = *x + 0.5f;
	character_vertices[46] = *y;
	character_vertices[47] = *z;
	character_vertices[48] = *x + 0.5f;
	character_vertices[49] = *y;
	character_vertices[50] = *z - 0.5f;
	character_vertices[51] = *x + 0.5f;
	character_vertices[52] = *y - 0.5f;
	character_vertices[53] = *z - 0.5f;
	character_vertices[54] = *x;
	character_vertices[55] = *y;
	character_vertices[56] = *z;
	character_vertices[57] = *x;
	character_vertices[58] = *y - 0.5f;
	character_vertices[59] = *z;
	character_vertices[60] = *x;
	character_vertices[61] = *y - 0.5f;
	character_vertices[62] = *z - 0.5f;
	character_vertices[63] = *x;
	character_vertices[64] = *y;
	character_vertices[65] = *z;
	character_vertices[66] = *x;
	character_vertices[67] = *y;
	character_vertices[68] = *z - 0.5f;
	character_vertices[69] = *x;
	character_vertices[70] = *y - 0.5f;
	character_vertices[71] = *z - 0.5f;
	character_vertices[72] = *x;
	character_vertices[73] = *y;
	character_vertices[74] = *z;
	character_vertices[75] = *x;
	character_vertices[76] = *y;
	character_vertices[77] = *z - 0.5f;
	character_vertices[78] = *x + 0.5f;
	character_vertices[79] = *y;
	character_vertices[80] = *z - 0.5f;
	character_vertices[81] = *x;
	character_vertices[82] = *y;
	character_vertices[83] = *z;
	character_vertices[84] = *x + 0.5f;
	character_vertices[85] = *y;
	character_vertices[86] = *z;
	character_vertices[87] = *x + 0.5f;
	character_vertices[88] = *y;
	character_vertices[89] = *z - 0.5f;
	character_vertices[90] = *x;
	character_vertices[91] = *y - 0.5f;
	character_vertices[92] = *z;
	character_vertices[93] = *x;
	character_vertices[94] = *y - 0.5f;
	character_vertices[95] = *z - 0.5f;
	character_vertices[96] = *x + 0.5f;
	character_vertices[97] = *y - 0.5f;
	character_vertices[98] = *z - 0.5f;
	character_vertices[99] = *x;
	character_vertices[100] = *y - 0.5f;
	character_vertices[101] = *z;
	character_vertices[102] = *x + 0.5f;
	character_vertices[103] = *y - 0.5f;
	character_vertices[104] = *z;
	character_vertices[105] = *x + 0.5f;
	character_vertices[106] = *y - 0.5f;
	character_vertices[107] = *z - 0.5f;

	// Buffer update for new coordinates
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 108, character_vertices, GL_STATIC_DRAW);
}


void checkTreasureCollision(float* characterX, float* characterY, float* characterZ, float* treasureX, float* treasureY, float* treasureZ, GLfloat character_verticesB[], GLuint vertexbuffer3, int maze[10][10],  GLuint glowIntensityLoc) {
	// Shrinking start time
	static double shrinkStartTime = 0.0; 

	// Check if the treasure is visible
	if (!isTreasureVisible) return;

	// Check for collision
	if (fabs(*characterX - *treasureX) < 0.5f && fabs(*characterY - *treasureY) < 0.5f) {
		if (!isTreasureShrinking) {
			// Start shrinking
			isTreasureShrinking = true; 

			// Record start time
			shrinkStartTime = glfwGetTime(); 

			// Enable glow
			glUniform1f(glowIntensityLoc, 1.0f);
		}
	}

	// Handle shrinking and disappearance
	if (isTreasureShrinking) {
		// Calculate the time elapsed since the start of shrinking
		double currentTime = glfwGetTime();

		// Time difference in seconds
		float elapsedTime = static_cast<float>(currentTime - shrinkStartTime); 

		// Shrink for 1 second
		if (elapsedTime < 1.0f) { 
			// Reduce to half
			float scaleFactor = 0.5f; 

			GLfloat tempVertices[] = {
				*treasureX,*treasureY,*treasureZ,*treasureX,*treasureY - (0.8f * scaleFactor),*treasureZ,*treasureX + (0.8f * scaleFactor),*treasureY - (0.8f * scaleFactor),*treasureZ,*treasureX,*treasureY,*treasureZ,
				*treasureX + (0.8f * scaleFactor),*treasureY,*treasureZ,*treasureX + (0.8f * scaleFactor),*treasureY - (0.8f * scaleFactor),*treasureZ,*treasureX,*treasureY,*treasureZ - (0.8f * scaleFactor),*treasureX,*treasureY - (0.8f * scaleFactor),
				*treasureZ - (0.8f * scaleFactor),*treasureX + (0.8f * scaleFactor),*treasureY - (0.8f * scaleFactor),*treasureZ - (0.8f * scaleFactor),*treasureX,*treasureY,*treasureZ - (0.8f * scaleFactor),*treasureX + (0.8f * scaleFactor),*treasureY,*treasureZ - (0.8f * scaleFactor),
				*treasureX + (0.8f * scaleFactor),*treasureY - (0.8f * scaleFactor),*treasureZ - (0.8f * scaleFactor),*treasureX + (0.8f * scaleFactor),*treasureY,*treasureZ,*treasureX + (0.8f * scaleFactor),*treasureY - (0.8f * scaleFactor),*treasureZ,*treasureX + (0.8f * scaleFactor),
				*treasureY - (0.8f * scaleFactor),*treasureZ - (0.8f * scaleFactor),*treasureX + (0.8f * scaleFactor),*treasureY,*treasureZ,*treasureX + (0.8f * scaleFactor),*treasureY,*treasureZ - (0.8f * scaleFactor),*treasureX + (0.8f * scaleFactor),*treasureY - (0.8f * scaleFactor),
				*treasureZ - (0.8f * scaleFactor),*treasureX,*treasureY,*treasureZ,*treasureX,*treasureY - (0.8f * scaleFactor),*treasureZ,*treasureX,*treasureY - (0.8f * scaleFactor),*treasureZ - (0.8f * scaleFactor),*treasureX,*treasureY,
				*treasureZ,*treasureX,*treasureY,*treasureZ - (0.8f * scaleFactor),*treasureX,*treasureY - (0.8f * scaleFactor),*treasureZ - (0.8f * scaleFactor),*treasureX,*treasureY,*treasureZ,*treasureX,*treasureY,
				*treasureZ - (0.8f * scaleFactor),*treasureX + (0.8f * scaleFactor),*treasureY,*treasureZ - (0.8f * scaleFactor),*treasureX,*treasureY,*treasureZ,*treasureX + (0.8f * scaleFactor),*treasureY,*treasureZ,*treasureX + (0.8f * scaleFactor),*treasureY,
				*treasureZ - (0.8f * scaleFactor),*treasureX,*treasureY - (0.8f * scaleFactor),*treasureZ,*treasureX,*treasureY - (0.8f * scaleFactor),*treasureZ - (0.8f * scaleFactor),*treasureX + (0.8f * scaleFactor),*treasureY - (0.8f * scaleFactor),*treasureZ - (0.8f * scaleFactor),*treasureX,
				*treasureY - (0.8f * scaleFactor),*treasureZ,*treasureX + (0.8f * scaleFactor),*treasureY - (0.8f * scaleFactor),*treasureZ,*treasureX + (0.8f * scaleFactor),*treasureY - (0.8f * scaleFactor),*treasureZ - (0.8f * scaleFactor)

			};

			std::copy(std::begin(tempVertices), std::end(tempVertices), character_verticesB);

			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
			glBufferData(GL_ARRAY_BUFFER, sizeof(tempVertices), tempVertices, GL_STATIC_DRAW);
		} else {
			// Disappear treasure
			isTreasureShrinking = false;
			isTreasureVisible = false;

			// Place a new treasure with its initial size
			placeTreasure(maze, treasureX, treasureY, treasureZ, character_verticesB, characterX, characterY, vertexbuffer3);

			// Restore to the original size
			GLfloat tempVertices[] = {
				*treasureX,*treasureY,*treasureZ,*treasureX,*treasureY - 0.8f,*treasureZ,*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ,*treasureX,*treasureY,*treasureZ,
				*treasureX + 0.8f,*treasureY,*treasureZ,*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ,*treasureX,*treasureY,*treasureZ - 0.8f,*treasureX,*treasureY - 0.8f,
				*treasureZ - 0.8f,*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ - 0.8f,*treasureX,*treasureY,*treasureZ - 0.8f,*treasureX + 0.8f,*treasureY,*treasureZ - 0.8f,
				*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ - 0.8f,*treasureX + 0.8f,*treasureY,*treasureZ,*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ,*treasureX + 0.8f,
				*treasureY - 0.8f,*treasureZ - 0.8f,*treasureX + 0.8f,*treasureY,*treasureZ,*treasureX + 0.8f,*treasureY,*treasureZ - 0.8f,*treasureX + 0.8f,*treasureY - 0.8f,
				*treasureZ - 0.8f,*treasureX,*treasureY,*treasureZ,*treasureX,*treasureY - 0.8f,*treasureZ,*treasureX,*treasureY - 0.8f,*treasureZ - 0.8f,*treasureX,*treasureY,
				*treasureZ,*treasureX,*treasureY,*treasureZ - 0.8f,*treasureX,*treasureY - 0.8f,*treasureZ - 0.8f,*treasureX,*treasureY,*treasureZ,*treasureX,*treasureY,
				*treasureZ - 0.8f,*treasureX + 0.8f,*treasureY,*treasureZ - 0.8f,*treasureX,*treasureY,*treasureZ,*treasureX + 0.8f,*treasureY,*treasureZ,*treasureX + 0.8f,*treasureY,
				*treasureZ - 0.8f,*treasureX,*treasureY - 0.8f,*treasureZ,*treasureX,*treasureY - 0.8f,*treasureZ - 0.8f,*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ - 0.8f,*treasureX,
				*treasureY - 0.8f,*treasureZ,*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ,*treasureX + 0.8f,*treasureY - 0.8f,*treasureZ - 0.8f
			};

			std::copy(std::begin(tempVertices), std::end(tempVertices), character_verticesB);

			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
			glBufferData(GL_ARRAY_BUFFER, sizeof(tempVertices), tempVertices, GL_STATIC_DRAW);

			// Enable visibility
			isTreasureVisible = true; 
		}
	}
}


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
	} else {
		printf("Αδύνατο να ανοίξει το %s. Είσαι στον σωστό φάκελο; Μην ξεχάσεις να διαβάσεις το FAQ!\n", vertex_file_path);
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

	// Compile the Vertex Shader
	printf("Συμπιέζω το shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check the Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile the Fragment Shader
	printf("Συμπιέζω το shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check the Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	printf("Συνδέω το πρόγραμμα\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

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


int main(void) {
	srand(static_cast<unsigned int>(time(0)));

	// Check if GLFW has loaded correctly
	if (!glfwInit()) {
		fprintf(stderr, "Αποτυχία φόρτωσης του GLFW\n");
		getchar();
		return -1;
	}

	// Window settings
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



	// Window creation
	window = glfwCreateWindow(950, 950, "Εργασία 1Γ - 2024 - Κύνήγι Θυσαυρού", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Αποτυχία ανοίγματος παραθύρου. Αν έχεις κάρτα γραφικών Intel, δεν υποστηρίζουν 3.3. Δοκίμασε την 2.1 έκδοση των tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Αποτυχία αρχικοποίησης του GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID = LoadShaders("P1BVertexShader.vertexshader", "P1BFragmentShader.fragmentshader");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);

	glm::mat4 View = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 20.0f),   
		glm::vec3(0.0f, 0.0f, 0.25f),   
		glm::vec3(0.0f, 1.0f, 0.0f));  

	glm::mat4 Model = glm::mat4(1.0f);

	glm::mat4 MVP = Projection * View * Model;

	GLfloat len = 5.0f, wid = 2.5f, heig = 2.5f;

	static const GLfloat cube[] = {
		// Front
		-5.0f,5.0f,0.0f,	-5.0f,4.0f,0.0f,	5.0f,4.0f,0.0f,		-5.0f,5.0f,0.0f,	5.0f,5.0f,0.0f,		5.0f,4.0f,0.0f,
		-5.0f,4.0f,0.0f,	-5.0f,3.0f,0.0f,	-4.0f,3.0f,0.0f,	-5.0f,4.0f,0.0f,	-4.0f,4.0f,0.0f,	-4.0f,3.0f,0.0f,
		4.0f,4.0f,0.0f,		4.0f,-2.0f,0.0f,	5.0f,-2.0f,0.0f,	4.0f,4.0f,0.0f,		5.0f,4.0f,0.0f,		5.0f,-2.0f,0.0f,
		4.0f,-3.0f,0.0f,	4.0f,-5.0f,0.0f,	5.0f,-5.0f,0.0f,	4.0f,-3.0f,0.0f,	5.0f,-3.0f,0.0f,	5.0f,-5.0f,0.0f,
		-5.0f,-4.0f,0.0f,	-5.0f,-5.0f,0.0f,	4.0f,-5.0f,0.0f,	-5.0f,-4.0f,0.0f,	4.0f,-4.0f,0.0f,	4.0f,-5.0f,0.0f,
		-5.0f,2.0f,0.0f,	-5.0f,-4.0f,0.0f,	-4.0f,-4.0f,0.0f,	-5.0f,2.0f,0.0f,	-4.0f,2.0f,0.0f,	-4.0f,-4.0f,0.0f,
		-3.0f,-3.0f,0.0f,	-3.0f,-4.0f,0.0f,	-2.0f,-4.0f,0.0f,	-3.0f,-3.0f,0.0f,	-2.0f,-3.0f,0.0f,	-2.0f,-4.0f,0.0f,
		-1.0f,-3.0f,0.0f,	-1.0f,-4.0f,0.0f,	1.0f,-4.0f,0.0f,	-1.0f,-3.0f,0.0f,	1.0f,-3.0f,0.0f,	1.0f,-4.0f,0.0f,
		0.0f,-1.0f,0.0f,	0.0f,-2.0f,0.0f,	3.0f,-2.0f,0.0f,	0.0f,-1.0f,0.0f,	3.0f,-1.0f,0.0f,	3.0f,-2.0f,0.0f,
		0.0f,0.0f,0.0f,		0.0f,-1.0f,0.0f,	1.0f,-1.0f,0.0f,	0.0f,0.0f,0.0f,		1.0f,0.0f,0.0f,		1.0f,-1.0f,0.0f,
		-1.0f,1.0f,0.0f,	-1.0f,0.0f,0.0f,	1.0f,0.0f,0.0f,		-1.0f,1.0f,0.0f,	1.0f,1.0f,0.0f,		1.0f,0.0f,0.0f,
		-3.0f,3.0f,0.0f,	-3.0f,0.0f,0.0f,	-2.0f,0.0f,0.0f,	-3.0f,3.0f,0.0f,	-2.0f,3.0f,0.0f,	-2.0f,0.0,0.0f,
		-2.0f,3.0f,0.0f,	-2.0f,2.0f,0.0f,	1.0f,2.0f,0.0f,		-2.0f,3.0f,0.0f,	1.0f,3.0f,0.0f,		1.0f,2.0f,0.0f,
		2.0f,3.0f,0.0f,		2.0f,0.0f,0.0f,		3.0f,0.0f,0.0f,		2.0f,3.0f,0.0f,		3.0f,3.0f,0.0f,		3.0f,0.0f,0.0f,
		2.0f,-2.0f,0.0f,	2.0f,-3.0f,0.0f,	3.0f,-3.0f,0.0f,	2.0f,-2.0f,0.0f,	3.0f,-3.0f,0.0f,	3.0f,-2.0f,0.0f,
		-3.0f,-1.0f,0.0f,	-3.0f,-2.0f,0.0f,	-1.0f,-2.0f,0.0f,	-3.0f,-1.0f,0.0f,	-1.0f,-1.0f,0.0f,	-1.0f,-2.0f,0.0f,

		// Back
		-5.0f,4.0f,-1.0f,	-5.0f,5.0f,-1.0f,	5.0f,5.0f,-1.0f,	-5.0f,4.0f,-1.0f,	5.0f,4.0f,-1.0f,	5.0f,5.0f,-1.0f,
		-5.0f,3.0f,-1.0f,	-5.0f,4.0f,-1.0f,	-4.0f,4.0f,-1.0f,	-5.0f,3.0f,-1.0f,	-4.0f,3.0f,-1.0f,	-4.0f,4.0f,-1.0f,
		4.0f,4.0f,-1.0f,	4.0f,-2.0f,-1.0f,	5.0f,4.0f,-1.0f,	5.0f,-2.0f,-1.0f,	5.0f,4.0f,-1.0f,	4.0f,-2.0f,-1.0f,
		4.0f,-3.0f,-1.0f,	4.0f,-5.0f,-1.0f,	5.0f,-3.0f,-1.0f,	4.0f,-5.0f,-1.0f,	5.0f,-5.0f,-1.0f,	5.0f,-3.0f,-1.0f,
		-5.0f,-5.0f,-1.0f,	-5.0f,-4.0f,-1.0f,	4.0f,-4.0f,-1.0f,	-5.0f,-5.0f,-1.0f,	4.0f,-5.0f,-1.0f,	4.0f,-4.0f,-1.0f,
		-5.0f,-4.0f,-1.0f,	-5.0f,2.0f,-1.0f,	-4.0f,2.0f,-1.0f,	-5.0f,-4.0f,-1.0f,	-4.0f,-4.0f,-1.0f,	-4.0f,2.0f,-1.0f,
		-3.0f,-4.0f,-1.0f,	-3.0f,-3.0f,-1.0f,	-2.0f,-3.0f,-1.0f,	-3.0f,-4.0f,-1.0f,	-2.0f,-4.0f,-1.0f,	-2.0f,-3.0f,-1.0f,
		-1.0f,-4.0f,-1.0f,	-1.0f,-3.0f,-1.0f,	1.0f,-3.0f,-1.0f,	-1.0f,-4.0f,-1.0f,	1.0f,-4.0f,-1.0f,	1.0f,-3.0f,-1.0f,
		0.0f,-2.0f,-1.0f,	0.0f,-1.0f,-1.0f,	3.0f,-1.0f,-1.0f,	0.0f,-2.0f,-1.0f,	3.0f,-2.0f,-1.0f,	3.0f,-1.0f,-1.0f,
		0.0f,-1.0f,-1.0f,	0.0f,0.0f,-1.0f,	1.0f,0.0f,-1.0f,	0.0f,-1.0f,-1.0f,	1.0f,-1.0f,-1.0f,	1.0f,0.0f,-1.0f,
		-1.0f,0.0f,-1.0f,	-1.0f,1.0f,-1.0f,	1.0f,1.0f,-1.0f,	-1.0f,0.0f,-1.0f,	1.0f,0.0f,-1.0f,	1.0f,1.0f,-1.0f,
		-3.0f,0.0f,-1.0f,	-3.0f,3.0f,-1.0f,	-2.0f,3.0f,0.0f,	-3.0f,0.0f,-1.0f,	-2.0f,0.0f,-1.0f,	-2.0f,3.0,-1.0f,
		-2.0f,2.0f,-1.0f,	-2.0f,3.0f,-1.0f,	1.0f,3.0f,-1.0f,	-2.0f,2.0f,-1.0f,	1.0f,2.0f,-1.0f,	1.0f,3.0f,-1.0f,
		2.0f,0.0f,-1.0f,	2.0f,3.0f,-1.0f,	3.0f,3.0f,-1.0f,	2.0f,0.0f,-1.0f,	3.0f,0.0f,-1.0f,	3.0f,3.0f,-1.0f,
		2.0f,-3.0f,-1.0f,	2.0f,-2.0f,-1.0f,	3.0f,-2.0f,-1.0f,	2.0f,-3.0f,-1.0f,	3.0f,-3.0f,-1.0f,	3.0f,-2.0f,-1.0f,
		-3.0f,-2.0f,-1.0f,	-3.0f,-1.0f,-1.0f,	-1.0f,-1.0f,-1.0f,	-3.0f,-2.0f,-1.0f,	-1.0f,-2.0f,-1.0f,	-1.0f,-1.0f,-1.0f,

		// Right
		5.0f,5.0f,0.0f,		5.0f,4.0f,0.0f,		5.0f,4.0f,-1.0f,	5.0f,5.0f,0.0f,		5.0f,5.0f,-1.0f,	5.0f,4.0f,-1.0f,
		-4.0f,4.0f,0.0f,	-4.0f,3.0f,0.0f,	-4.0f,3.0f,-1.0f,	-4.0f,4.0f,0.0f,	-4.0f,4.0f,-1.0f,	-4.0f,3.0f,-1.0f,
		5.0f,4.0f,0.0f,		5.0f,-2.0f,0.0f,	5.0f,-2.0f,-1.0f,	5.0f,4.0f,0.0f,		5.0f,4.0f,-1.0f,	5.0f,-2.0f,-1.0f,
		5.0f,-3.0f,0.0f,	5.0f,-5.0f,0.0f,	5.0f,-5.0f,-1.0f,	5.0f,-3.0f,0.0f,	5.0f,-3.0f,-1.0f,	5.0f,-5.0f,-1.0f,
		4.0f,-4.0f,0.0f,	4.0f,-5.0f,0.0f,	4.0f,-5.0f,-1.0f,	4.0f,-4.0f,0.0f,	4.0f,-4.0f,-1.0f,	4.0f,-5.0f,-1.0f,
		-4.0f,2.0f,0.0f,	-4.0f,-4.0f,0.0f,	-4.0f,-4.0f,-1.0f,	-4.0f,2.0f,0.0f,	-4.0f,2.0f,-1.0f,	-4.0f,-4.0f,-1.0f,
		-2.0f,-3.0f,0.0f,	-2.0f,-4.0f,0.0f,	-2.0f,-4.0f,-1.0f,	-2.0f,-3.0f,0.0f,	-2.0f,-3.0f,-1.0f,	-2.0f,-4.0f,-1.0f,
		1.0f,-3.0f,0.0f,	1.0f,-4.0f,0.0f,	1.0f,-4.0f,-1.0f,	1.0f,-3.0f,0.0f,	1.0f,-3.0f,-1.0f,	1.0f,-4.0f,-1.0f,
		3.0f,-1.0f,0.0f,	3.0f,-2.0f,0.0f,	3.0f,-2.0f,-1.0f,	3.0f,-1.0f,0.0f,	3.0f,-1.0f,-1.0f,	3.0f,-2.0f,-1.0f,
		1.0f,0.0f,0.0f,		1.0f,-1.0f,0.0f,	1.0f,-1.0f,-1.0f,	1.0f,0.0f,0.0f,		1.0f,0.0f,-1.0f,	1.0f,-1.0f,-1.0f,
		1.0f,1.0f,0.0f,		1.0f,0.0f,0.0f,		1.0f,0.0f,-1.0f,	1.0f,1.0f,0.0f,		1.0f,1.0f,-1.0f,	1.0f,0.0,-1.0f,
		-2.0f,3.0f,0.0f,	-2.0f,0.0f,0.0f,	-2.0f,0.0f,-1.0f,	-2.0f,3.0f,0.0f,	-2.0f,3.0f,-1.0f,	-2.0f,0.0f,-1.0f,
		1.0f,3.0f,0.0f,		1.0f,2.0f,0.0f,		1.0f,2.0f,-1.0f,	1.0f,3.0f,0.0f,		1.0f,3.0f,-1.0f,	1.0f,2.0f,-1.0f,
		3.0f,3.0f,0.0f,		3.0f,0.0f,0.0f,		3.0f,0.0f,-1.0f,	3.0f,3.0f,0.0f,		3.0f,3.0f,-1.0f,	3.0f,0.0f,-1.0f,
		3.0f,-2.0f,0.0f,	3.0f,-3.0f,0.0f,	3.0f,-3.0f,-1.0f,	3.0f,-2.0f,0.0f,	3.0f,-2.0f,-1.0f,	3.0f,-3.0f,-1.0f,
		-1.0f,-1.0f,0.0f,	-1.0f,-2.0f,0.0f,	-1.0f,-2.0f,-1.0f,	-1.0f,-1.0f,0.0f,	-1.0f,-1.0f,-1.0f,	-1.0f,-2.0f,-1.0f,

		// Left
		-5.0f,5.0f,-1.0f,	-5.0f,4.0f,-1.0f,	-5.0f,4.0f,0.0f,	-5.0f,5.0f,-1.0f,	-5.0f,5.0f,0.0f,	-5.0f,4.0f,0.0f,
		-5.0f,4.0f,-1.0f,	-5.0f,3.0f,-1.0f,	-5.0f,3.0f,0.0f,	-5.0f,4.0f,-1.0f,	-5.0f,4.0f,0.0f,	-5.0f,3.0f,0.0f,
		4.0f,4.0f,-1.0f,	4.0f,-2.0f,-1.0f,	4.0f,-2.0f,0.0f,	4.0f,4.0f,-1.0f,	4.0f,4.0f,0.0f,		4.0f,-2.0f,0.0f,
		4.0f,-3.0f,-1.0f,	4.0f,-5.0f,-1.0f,	4.0f,-5.0f,0.0f,	4.0f,-3.0f,-1.0f,	4.0f,-3.0f,0.0f,	4.0f,-5.0f,0.0f,
		-5.0f,-4.0f,-1.0f,	-5.0f,-5.0f,-1.0f,	-5.0f,-5.0f,0.0f,	-5.0f,-4.0f,-1.0f,	-5.0f,-4.0f,0.0f,	-5.0f,-5.0f,0.0f,
		-5.0f,2.0f,-1.0f,	-5.0f,-4.0f,-1.0f,	-5.0f,-4.0f,0.0f,	-5.0f,2.0f,-1.0f,	-5.0f,2.0f,0.0f,	-5.0f,-4.0f,0.0f,
		-3.0f,-3.0f,-1.0f,	-3.0f,-4.0f,-1.0f,	-3.0f,-4.0f,0.0f,	-3.0f,-3.0f,-1.0f,	-3.0f,-3.0f,0.0f,	-3.0f,-4.0f,0.0f,
		-1.0f,-3.0f,-1.0f,	-1.0f,-4.0f,-1.0f,	-1.0f,-4.0f,0.0f,	-1.0f,-3.0f,-1.0f,	-1.0f,-3.0f,0.0f,	-1.0f,-4.0f,0.0f,
		0.0f,-1.0f,-1.0f,	0.0f,-2.0f,-1.0f,	0.0f,-2.0f,0.0f,	0.0f,-1.0f,-1.0f,	0.0f,-1.0f,0.0f,	0.0f,-2.0f,0.0f,
		0.0f,0.0f,-1.0f,	0.0f,-1.0f,-1.0f,	0.0f,-1.0f,0.0f,	0.0f,0.0f,-1.0f,	0.0f,0.0f,0.0f,		0.0f,-1.0f,0.0f,
		-1.0f,1.0f,-1.0f,	-1.0f,0.0f,-1.0f,	-1.0f,0.0f,0.0f,	-1.0f,1.0f,-1.0f,	-1.0f,1.0f,0.0f,	-1.0f,0.0f,0.0f,
		-3.0f,3.0f,-1.0f,	-3.0f,0.0f,-1.0f,	-3.0f,0.0f,0.0f,	-3.0f,3.0f,-1.0f,	-3.0f,3.0f,0.0f,	-3.0f,0.0,0.0f,
		-2.0f,3.0f,-1.0f,	-2.0f,2.0f,-1.0f,	-1.0f,2.0f,0.0f,	-2.0f,3.0f,-1.0f,	-2.0f,3.0f,0.0f,	-2.0f,2.0f,0.0f,
		2.0f,3.0f,-1.0f,	2.0f,0.0f,-1.0f,	2.0f,0.0f,0.0f,		2.0f,3.0f,-1.0f,	2.0f,3.0f,0.0f,		2.0f,0.0f,0.0f,
		2.0f,-2.0f,-1.0f,	2.0f,-3.0f,-1.0f,	2.0f,-3.0f,0.0f,	2.0f,-2.0f,-1.0f,	2.0f,-2.0f,0.0f,	2.0f,-3.0f,0.0f,
		-3.0f,-1.0f,-1.0f,	-3.0f,-2.0f,-1.0f,	-3.0f,-2.0f,0.0f,	-3.0f,-1.0f,-1.0f,	-3.0f,-1.0f,0.0f,	-3.0f,-2.0f,0.0f,

		// Up
		-5.0f,5.0f,-1.0f,   -5.0f,5.0f,0.0f,	5.0f,5.0f,0.0f,    -5.0f,5.0f,-1.0f,	5.0f,5.0f,-1.0f,	5.0f,5.0f,0.0f,
		-5.0f,4.0f,-1.0f,	-5.0f,4.0f,0.0f,	-4.0f,4.0f,0.0f,	-5.0f,4.0f,-1.0f,	-4.0f,4.0f,-1.0f,	-4.0f,4.0f,0.0f,
		4.0f,4.0f,-1.0f,	4.0f,4.0f,0.0f,		5.0f,4.0f,0.0f,		4.0f,4.0f,-1.0f,	5.0f,4.0f,-1.0f,	5.0f,4.0f,0.0f,
		4.0f,-3.0f,-1.0f,	4.0f,-3.0f,0.0f,	5.0f,-3.0f,0.0f,	4.0f,-3.0f,-1.0f,	5.0f,-3.0f,-1.0f,	5.0f,-3.0f,0.0f,
		-5.0f,-4.0f,-1.0f,	-5.0f,-4.0f,0.0f,	4.0f,-4.0f,0.0f,	-5.0f,-4.0f,-1.0f,	4.0f,-4.0f,-1.0f,	4.0f,-4.0f,0.0f,
		-5.0f,2.0f,-1.0f,	-5.0f,2.0f,0.0f,	-4.0f,2.0f,0.0f,	-5.0f,2.0f,-1.0f,	-4.0f,2.0f,-1.0f,	-4.0f,2.0f,0.0f,
		-3.0f,-3.0f,-1.0f,	-3.0f,-3.0f,0.0f,	-2.0f,-3.0f,0.0f,	-3.0f,-3.0f,-1.0f,	-2.0f,-3.0f,-1.0f,	-2.0f,-3.0f,0.0f,
		-1.0f,-3.0f,-1.0f,	-1.0f,-3.0f,0.0f,	1.0f,-3.0f,0.0f,	-1.0f,-3.0f,-1.0f,	1.0f,-3.0f,-1.0f,	1.0f,-3.0f,0.0f,
		0.0f,-1.0f,-1.0f,	0.0f,-1.0f,0.0f,	3.0f,-1.0f,0.0f,	0.0f,-1.0f,-1.0f,	3.0f,-1.0f,0.0f,	3.0f,-1.0f,-1.0f,
		0.0f,0.0f,-1.0f,	0.0f,0.0f,0.0f,		1.0f,0.0f,0.0f,		0.0f,0.0f,-1.0f,	1.0f,0.0f,-1.0f,	1.0f,0.0f,0.0f,
		-1.0f,1.0f,-1.0f,	-1.0f,1.0f,0.0f,	1.0f,1.0f,0.0f,		-1.0f,1.0f,-1.0f,	1.0f,1.0f,-1.0f,	1.0f,1.0f,0.0f,
		-3.0f,3.0f,-1.0f,	-3.0f,3.0f,0.0f,	-2.0f,3.0f,0.0f,	-3.0f,3.0f,-1.0f,	-2.0f,3.0f,-1.0f,	-2.0f,3.0,0.0f,
		-2.0f,3.0f,-1.0f,	-2.0f,3.0f,0.0f,	1.0f,3.0f,0.0f,		-2.0f,3.0f,-1.0f,	1.0f,3.0f,-1.0f,	1.0f,3.0f,0.0f,
		2.0f,3.0f,-1.0f,	2.0f,3.0f,0.0f,		3.0f,3.0f,0.0f,		2.0f,3.0f,-1.0f,	3.0f,3.0f,-1.0f,	3.0f,3.0f,0.0f,
		2.0f,-2.0f,-1.0f,	2.0f,-2.0f,0.0f,	3.0f,-2.0f,0.0f,	2.0f,-2.0f,-1.0f,	3.0f,-2.0f,-1.0f,	3.0f,-2.0f,0.0f,
		-3.0f,-1.0f,-1.0f,	-3.0f,-1.0f,0.0f,	-1.0f,-1.0f,0.0f,	-3.0f,-1.0f,-1.0f,	-1.0f,-1.0f,-1.0f,	-1.0f,-1.0f,0.0f,

		// Down
		-5.0f,4.0f,0.0f,	-5.0f,4.0f,-1.0f,	5.0f,4.0f,-1.0f,	-5.0f,4.0f,0.0f,	5.0f,4.0f,0.0f,		5.0f,4.0f,-1.0f,
		-5.0f,3.0f,0.0f,	-5.0f,3.0f,-1.0f,	-4.0f,3.0f,-1.0f,	-5.0f,3.0f,0.0f,	-4.0f,3.0f,0.0f,	-4.0f,3.0f,-1.0f,
		4.0f,-2.0f,0.0f,	4.0f,-2.0f,-1.0f,	5.0f,-2.0f,-1.0f,	4.0f,-2.0f,0.0f,	5.0f,-2.0f,0.0f,	5.0f,-2.0f,-1.0f,
		4.0f,-5.0f,0.0f,	4.0f,-5.0f,-1.0f,	5.0f,-5.0f,-1.0f,	4.0f,-5.0f,0.0f,	5.0f,-5.0f,0.0f,	5.0f,-5.0f,-1.0f,
		-5.0f,-5.0f,0.0f,	-5.0f,-5.0f,-1.0f,	4.0f,-5.0f,-1.0f,	-5.0f,-5.0f,0.0f,	4.0f,-5.0f,0.0f,	4.0f,-5.0f,-1.0f,
		-5.0f,-4.0f,0.0f,	-5.0f,-4.0f,-1.0f,	-4.0f,-4.0f,-1.0f,	-5.0f,-4.0f,0.0f,	-4.0f,-4.0f,0.0f,	-4.0f,-4.0f,-1.0f,
		-3.0f,-4.0f,0.0f,	-3.0f,-4.0f,-1.0f,	-2.0f,-4.0f,-1.0f,	-3.0f,-4.0f,0.0f,	-2.0f,-4.0f,0.0f,	-2.0f,-4.0f,-1.0f,
		-1.0f,-4.0f,0.0f,	-1.0f,-4.0f,-1.0f,	1.0f,-4.0f,-1.0f,	-1.0f,-4.0f,0.0f,	1.0f,-4.0f,0.0f,	1.0f,-4.0f,-1.0f,
		0.0f,-2.0f,0.0f,	0.0f,-2.0f,-1.0f,	3.0f,-2.0f,-1.0f,	0.0f,-2.0f,0.0f,	3.0f,-2.0f,0.0f,	3.0f,-2.0f,-1.0f,
		0.0f,-1.0f,0.0f,	0.0f,-1.0f,-1.0f,	1.0f,-1.0f,-1.0f,	0.0f,-1.0f,0.0f,	1.0f,-1.0f,0.0f,	1.0f,-1.0f,-1.0f,
		-1.0f,0.0f,0.0f,	-1.0f,0.0f,-1.0f,	1.0f,0.0f,-1.0f,	-1.0f,0.0f,0.0f,	1.0f,0.0f,0.0f,		1.0f,0.0f,-1.0f,
		-3.0f,0.0f,0.0f,	-3.0f,0.0f,-1.0f,	-2.0f,0.0f,-1.0f,	-3.0f,0.0f,0.0f,	-2.0f,0.0f,0.0f,	-2.0f,0.0,-1.0f,
		-2.0f,2.0f,0.0f,	-2.0f,2.0f,-1.0f,	1.0f,2.0f,-1.0f,	-2.0f,2.0f,0.0f,	1.0f,2.0f,0.0f,		1.0f,2.0f,-1.0f,
		2.0f,0.0f,0.0f,		2.0f,0.0f,-1.0f,	3.0f,0.0f,-1.0f,	2.0f,0.0f,0.0f,		3.0f,0.0f,0.0f,		3.0f,0.0f,-1.0f,
		2.0f,-3.0f,0.0f,	2.0f,-3.0f,-1.0f,	3.0f,-3.0f,-1.0f,	2.0f,-3.0f,0.0f,	3.0f,-3.0f,0.0f,	3.0f,-3.0f,-1.0f,
		-3.0f,-2.0f,0.0f,	-3.0f,-2.0f,-1.0f,	-1.0f,-2.0f,-1.0f,	-3.0f,-2.0f,0.0f,	-1.0f,-2.0f,0.0f,	-1.0f,-2.0f,-1.0f
	};

	GLfloat a = 1.0f;
	GLfloat r = 0.0f;
	GLfloat g = 0.0f;
	GLfloat b = 1.0f;

	static const GLfloat color[] = {
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,
		r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a,	r,  g,  b,a
	};

	float x = -4.75f;
	float y = 2.7f;
	float z = -0.25f;

	GLfloat charachter_vertices[] = {
		// Front
		x,y,z,		x,y - 0.5f,z,		x + 0.5f,y - 0.5f,z,	x,y,z,		x + 0.5f,y,z,		x + 0.5f,y - 0.5f,z,

		// Back
		x,y,z - 0.5f,		x,y - 0.5f,z - 0.5f,	x + 0.5f,y - 0.5f,z - 0.5f,	x,y,z - 0.5f,		x + 0.5f,y,z - 0.5f,	x + 0.5f,y - 0.5f,z - 0.5f,

		// Right
		x + 0.5f,y,z,		x + 0.5f,y - 0.5f,z,		x + 0.5f,y - 0.5f,z - 0.5f,	x + 0.5f,y,z,	x + 0.5f,y,z - 0.5f,	x + 0.5f,y - 0.5f,z - 0.5f,

		// Left
		x,y,z,		x,y - 0.5f,z,		x,y - 0.5f,z - 0.5f,	x,y,z,		x,y,z - 0.5f,		x,y - 0.5f,z - 0.5f,

		// Up
		x,y,z,		x,y,z - 0.5f,	x + 0.5f,y,z - 0.5f,	x,y,z,		x + 0.5f,y,z,	x + 0.5f,y,z - 0.5f,

		// Down
		x,y - 0.5f,z,		x,y - 0.5f,z - 0.5f,	x + 0.5f,y - 0.5f,z - 0.5f,		x,y - 0.5f,z,		x + 0.5f,y - 0.5f,z,		x + 0.5f,y - 0.5f,z - 0.5f
	};

	float treasureX = -1.0f;
	float treasureY = 1.0f;
	float treasureZ = -0.1f;

	GLfloat charachter_verticesB[] = {
		// Front
		treasureX,treasureY,treasureZ,		treasureX,treasureY - 0.8f,treasureZ,		treasureX + 0.8f,treasureY - 0.8f,treasureZ,	treasureX,treasureY,treasureZ,		treasureX + 0.8f,treasureY,treasureZ,		treasureX + 0.8f,treasureY - 0.8f,treasureZ,
		
		// Back
		treasureX,treasureY,treasureZ - 0.8f,		treasureX,treasureY - 0.8f,treasureZ - 0.8f,	treasureX + 0.8f,treasureY - 0.8f,treasureZ - 0.8f,	treasureX,treasureY,treasureZ - 0.8f,		treasureX + 0.8f,treasureY,treasureZ - 0.8f,	treasureX + 0.8f,treasureY - 0.8f,treasureZ - 0.8f,
		
		// Right
		treasureX + 0.8f,treasureY,treasureZ,		treasureX + 0.8f,treasureY - 0.8f,treasureZ,		treasureX + 0.8f,treasureY - 0.8f,treasureZ - 0.8f,	treasureX + 0.8f,treasureY,treasureZ,	treasureX + 0.8f,treasureY,treasureZ - 0.8f,	treasureX + 0.8f,treasureY - 0.8f,treasureZ - 0.8f,
		
		// Left
		treasureX,treasureY,treasureZ,		treasureX,treasureY - 0.8f,treasureZ,		treasureX,treasureY - 0.8f,treasureZ - 0.8f,	treasureX,treasureY,treasureZ,		treasureX,treasureY,treasureZ - 0.8f,		treasureX,treasureY - 0.8f,treasureZ - 0.8f,
		
		// Up
		treasureX,treasureY,treasureZ,		treasureX,treasureY,treasureZ - 0.8f,	treasureX + 0.8f,treasureY,treasureZ - 0.8f,	treasureX,treasureY,treasureZ,		treasureX + 0.8f,treasureY,treasureZ,	treasureX + 0.8f,treasureY,treasureZ - 0.8f,
		
		// Down
		treasureX,treasureY - 0.8f,treasureZ,		treasureX,treasureY - 0.8f,treasureZ - 0.8f,	treasureX + 0.8f,treasureY - 0.8f,treasureZ - 0.8f,		treasureX,treasureY - 0.8f,treasureZ,		treasureX + 0.8f,treasureY - 0.8f,treasureZ,		treasureX + 0.8f,treasureY - 0.8f,treasureZ - 0.8f
	};

	GLfloat r2 = 1.0f;
	GLfloat g2 = 1.0f;
	GLfloat b2 = 0.0f;

	static const GLfloat colorChar[] = {
		r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,
		r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,
		r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,
		r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,
		r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,
		r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a
	};

	GLfloat g_uv_buffer_data[] = {
		// Front
		0.000000, 0.333333,		0.333333, 0.000000,		0.333333, 0.333333,		0.000000, 0.666667,		0.333333, 0.333333,		0.333333, 0.666667,
		
		// Back
		0.333333, 1.000000,		0.000000, 0.666667,		0.333333, 0.666667,		0.666667, 1.000000,		0.333333, 0.666667,		0.666667, 0.666667,
		
		// Right
		0.333333, 0.333333,		0.666667, 0.000000,		0.666667, 0.333333,		0.333333, 0.666667,		0.666667, 0.333333,		0.666667, 0.666667,
		
		// Left
		0.000000, 0.000000,		0.000000, 0.333333,		0.333333, 0.000000,		0.333333, 0.333333,		0.000000, 1.000000,		0.333333, 1.000000,
		
		// Top
		0.666667, 0.666667,		1.000000, 0.666667,		1.000000, 1.000000,		0.666667, 1.000000,		0.666667, 0.666667,		1.000000, 0.666667,
		
		// Bottom
		0.666667, 0.000000,		1.000000, 0.000000,		1.000000, 0.333333,		0.666667, 0.333333,		0.666667, 0.000000,		1.000000, 0.333333,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(charachter_vertices), charachter_vertices, GL_STATIC_DRAW);

	GLuint colorbuffer2;
	glGenBuffers(1, &colorbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorChar), colorChar, GL_STATIC_DRAW);

	GLuint vertexbuffer3;
	glGenBuffers(1, &vertexbuffer3);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(charachter_verticesB), charachter_verticesB, GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

	GLuint textureIDs[3];
	std::string textureFiles[3] = { "coins.jpg", "silver.jpg", "gold.jpg" };
	int width, height, nrChannels;

	for (int i = 0; i < 3; ++i) {


		unsigned char* data = stbi_load(textureFiles[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {

			std::cout << "Image loaded successfully!" << std::endl;
			std::cout << "Width: " << width << ", Height: " << height << ", Channels: " << nrChannels << std::endl;

		}
		else {
			std::cerr << "Failed to load texture: " << textureFiles[i] << std::endl;
		}
		glGenTextures(1, &textureIDs[i]);
		glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
		if (textureIDs[i] == 0) {
			std::cerr << "Texture creation failed!" << std::endl;
		}
		else {
			std::cout << "Texture ID: " << textureIDs[i] << " loaded successfully!" << std::endl;
		}

	}

	/*glUseProgram(programID);
	///////////////////////////////////////////////////////////////////////////////////////////////////
	// definition of uniform locations
	GLuint lightPosLoc = glGetUniformLocation(programID, "lightPosition");
	GLuint lightColorLoc = glGetUniformLocation(programID, "lightColor");
	GLuint viewPosLoc = glGetUniformLocation(programID, "viewPosition");
	GLuint objectColorLoc = glGetUniformLocation(programID, "objectColor");
	//////////////////////////////////////////////////////////////////////////////////////////////////*/

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);
		
		//GLuint modelLoc = glGetUniformLocation(programID, "model");
		glUniform1i(glGetUniformLocation(programID, "texture1"), 0);  // GL_TEXTURE0
		glUniform1i(glGetUniformLocation(programID, "texture2"), 1);  // GL_TEXTURE1
		glUniform1i(glGetUniformLocation(programID, "texture3"), 2);  // GL_TEXTURE2
		GLuint glowIntensityLoc = glGetUniformLocation(programID, "glowIntensity");
		
		GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

		glm::vec3 lightPos = glm::vec3(4.0f, 4.0f, 4.0f);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		glUniform1f(glowIntensityLoc, 0.0f);

		camera_function();

		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * ViewMatrix * ModelMatrix;
		
		/*glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);     // White Light
		glUniform3f(lightPosLoc, 10.0f, 8.0f, 4.0f);        // Light Position
		glUniform3f(viewPosLoc, cameraX, cameraY, cameraZ); // Camera Position
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);      // Object Color*/

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,
			4,              // size
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		GLuint useTextureID = glGetUniformLocation(programID, "useTexture");
		GLuint activeTextureUniform = glGetUniformLocation(programID, "activeTexture");
		
		glUniform1i(useTextureID, 0); 
		glDrawArrays(GL_TRIANGLES, 0, 576);
		
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2);
		glVertexAttribPointer(
			1,
			4,              // size
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		// Draw triangles 
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		// 3rd attribute buffer : uv
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			2,              // Attribute location (UVs)
			2,              // size
			GL_FLOAT,       // type
			GL_FALSE,
			0,
			(void*)0
		);

		// Texture usage
		glUniform1i(useTextureID, 1); 
		glUniform1i(activeTextureUniform, activeTextureIndex + 1);
		glActiveTexture(GL_TEXTURE0 + activeTextureIndex);
		glBindTexture(GL_TEXTURE_2D, textureIDs[activeTextureIndex]);
		
		if (isTreasureShrinking) {
			double currentTime = glfwGetTime();
			float elapsedTime = static_cast<float>(currentTime - shrinkStartTime);

			float glowValue = glm::clamp(2.0f - elapsedTime, 0.0f, 2.0f);
			glUniform1f(glowIntensityLoc, glowValue);
		} else {
			// Glow Effect deactivation
			glUniform1f(glowIntensityLoc, 0.0f);
		}

		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		// deactivation of attributes
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(2);

		moveChar(&x, &y, &z, charachter_vertices, window, vertexbuffer2);
		placeTreasure(maze, &treasureX, &treasureY, &treasureZ, charachter_verticesB, &x, &y, vertexbuffer3);
		checkTreasureCollision(&x, &y, &z, &treasureX, &treasureY, &treasureZ, charachter_verticesB, vertexbuffer3, maze,  glowIntensityLoc);

		// Draw character
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0); // Check if the ESC key was pressed or the window was closed

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}