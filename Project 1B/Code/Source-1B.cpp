// Include standard headers
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

// Define matrices for view and projection transformations
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;


// Getter function for the view matrix
glm::mat4 getViewMatrix() {
    return ViewMatrix;
}


// Getter function for the projection matrix
glm::mat4 getProjectionMatrix() {
    return ProjectionMatrix;
}


// Boolean flags to track key press states for navigation and zoom
bool rightKeyPressed = false;     // Move camera or object to the right
bool leftKeyPressed = false;      // Move camera or object to the left
bool downKeyPressed = false;      // Move camera or object downwards
bool upKeyPressed = false;        // Move camera or object upwards

// Boolean flags to track key press states for rotation around X and Y axes
bool rightofXKeyPressed = false;  // Rotate around the X-axis to the right
bool leftofXKeyPressed = false;   // Rotate around the X-axis to the left
bool rightofYKeyPressed = false;  // Rotate around the Y-axis to the right
bool leftofYKeyPressed = false;   // Rotate around the Y-axis to the left

// Boolean flags to track zoom-in and zoom-out actions
bool zoomInKeyPressed = false;    // Zoom in the camera or object
bool zoomOutKeyPressed = false;   // Zoom out the camera or object


// Define a 10x10 maze grid, where 1 represents walls and 0 represents open paths
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
    {1,1,1,1,1,1,1,1,1,1}
};

// Camera control variables
float cameraDistance = 20.0f;     // Controls the distance of the camera from the origin
float pitch = 0.0f;               // Rotation angle around the X-axis (up and down rotation)
float yaw = 0.0f;                 // Rotation angle around the Y-axis (left and right rotation)

// Function to handle camera movements and rotations based on key inputs
void camera_function() {
    static bool initialized = false;
    	if (!initialized) {
        	cameraDistance = 20.0f;
                pitch = 0.0f;
                yaw = 0.0f;
                initialized = true;
        }
	
    // Control pitch (up and down rotation) around the X-axis
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (!leftofXKeyPressed) {
            std::cout << "Left of x key pressed\n";
            // Rotate upward
	    pitch += 0.1f;    
	    // Set flag to true to prevent repeated actions   
            leftofXKeyPressed = true;  
        }
    } else {
	// Reset flag when key is released
        leftofXKeyPressed = false;  
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        if (!rightofXKeyPressed) {
            std::cout << "Right of x key pressed\n";
            // Rotate downward
	    pitch -= 0.1f;    
	    // Set flag to true to prevent repeated actions
            rightofXKeyPressed = true;
        }
    } else {
        rightofXKeyPressed = false;
    }

    // Control yaw (left and right rotation) around the Y-axis
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (!leftofYKeyPressed) {
            std::cout << "Left of y key pressed\n";
            // Rotate to the left
	    yaw += 0.1f;
	    // Set flag to true to prevent repeated actions
            leftofYKeyPressed = true;
        }
    } else {
        leftofYKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        if (!rightofYKeyPressed) {
            std::cout << "Right key pressed\n";
            // Rotate to the right
	    yaw -= 0.1f;   
	    // Set flag to true to prevent repeated actions
            rightofYKeyPressed = true;
        }
    } else {
        rightofYKeyPressed = false;
    }

    // Zoom in
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        if (!zoomInKeyPressed) {
            std::cout << "Zoom in key pressed\n";
            // Decrease distance to zoom in
	    cameraDistance -= 0.1f;   
	    // Minimum zoom limit
            if (cameraDistance < 1.0f) cameraDistance = 1.0f; 
            // Set flag to true to prevent repeated actions
	    zoomInKeyPressed = true;
        }
    } else {
        zoomInKeyPressed = false;
    }

    // Zoom out
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        if (!zoomOutKeyPressed) {
            std::cout << "Zoom out key pressed\n";
            // Increase distance to zoom out
	    cameraDistance += 0.1f;   
	    // Maximum zoom limit
            if (cameraDistance > 20.0f) cameraDistance = 20.0f; 
            // Set flag to true to prevent repeated actions
	    zoomOutKeyPressed = true;
        }
    } else {
        zoomOutKeyPressed = false;
    }

    // Calculate the new camera position based on yaw, pitch, and distance
    float cameraX = cameraDistance * sin(yaw) * cos(pitch);
    float cameraY = cameraDistance * sin(pitch);
    float cameraZ = cameraDistance * cos(yaw) * cos(pitch);

    // Define the camera's position, target point, and up direction
    glm::vec3 cameraPos = glm::vec3(cameraX, cameraY, cameraZ);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);   // Camera looks toward the origin
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);       // Up direction of the camera

    // Create the view matrix based on the camera's position, target, and up vector
    ViewMatrix = glm::lookAt(cameraPos, target, up);
}


// Function to check if a given (x, y, z) position is within a wall in the maze
bool isWall(float x, float y, float z) {
    // Convert OpenGL coordinates to 2D maze grid indices
    int col = static_cast<int>(x + 5.0f);   // Convert x coordinate to column index
    int row = static_cast<int>(5.0f - y);   // Convert y coordinate to row index

    // Ensure indices are within maze boundaries (10x10 grid)
    if (row >= 0 && row < 10 && col >= 0 && col < 10) {
        // Return true if the maze cell is a wall (indicated by 1)
        return maze[row][col] == 1;
    }
    // If coordinates are out of bounds, treat as if in a wall
    return true;
}


// Function to move the character based on key inputs
void moveChar(float* x, float* y, float* z, GLfloat character_vertices[], GLFWwindow* window, GLuint vertexbuffer2) {
    // Create temporary variables for new positions to update after checking for walls
    float new_x = *x;
    float new_y = *y;
    float new_z = *z;

    // Starting point in maze (maze[2][0])
    const float start_x = -4.75f;
    const float start_y = 2.7f;

    // Exit point in maze (maze[7][9])
    const float end_x = 4.25f;
    const float end_y = -2.3f;

    // Move right when 'L' key is pressed
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        // Only act if key wasn’t pressed previously
	if (!rightKeyPressed) {  
	    // If at exit, move to start
            if (new_x == end_x && new_y == end_y) {  
                std::cout << "Right key pressed\n";
                new_x = start_x;
                new_y = start_y;
            } else {
                std::cout << "Right key pressed\n";
                // Move one unit right
		new_x += 1.0f;  
            }
	    // Set flag to indicate key press
            rightKeyPressed = true;  
        }
    } else {
	// Reset flag when key is released
        rightKeyPressed = false;  
    }

    // Move left when 'J' key is pressed
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        if (!leftKeyPressed) {
	    // If at start, move to exit
            if (new_x == start_x && new_y == start_y) {  
                std::cout << "Left key pressed\n";
                new_x = end_x;
                new_y = end_y;
	    // Boundary check to prevent moving out of bounds
            } else if (*x > -4.75f) {  
                std::cout << "Left key pressed\n";
                // Move one unit left
		new_x -= 1.0f;  
            }
            leftKeyPressed = true;
        }
    } else {
        leftKeyPressed = false;
    }

    // Move down when 'K' key is pressed
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        if (!downKeyPressed) {
            std::cout << "Down key pressed\n";
            // Move one unit down
	    new_y -= 1.0f;  
            downKeyPressed = true;
        }
    } else {
        downKeyPressed = false;
    }

    // Move up when 'I' key is pressed
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        if (!upKeyPressed) {
            std::cout << "Up key pressed\n";
            // Move one unit up
	    new_y += 1.0f;  
            upKeyPressed = true;
        }
    } else {
        upKeyPressed = false;
    }

    // Check if the new position is within a wall
    if (!isWall(new_x, new_y, new_z)) {
        // Update character's position if new position is not in a wall
        *x = new_x;
        *y = new_y;
        *z = new_z;
    }

		// Update character vertices for the new 3D position
		// Simple vertices for a 3D cube (8 vertices for a cube)
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


// Function to load and compile shaders from the given file paths
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

    // Create shader objects for vertex and fragment shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the vertex shader code from the specified file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);

    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf(); // Read the file buffer into a stringstream
        VertexShaderCode = sstr.str();      // Store the string
        VertexShaderStream.close();         // Close the file after reading
    } else {
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
        getchar();

		// Return 0 if the vertex shader file cannot be opened
        return 0; 
    }

    // Read the fragment shader code from the specified file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
		
		// Read the file buffer into a stringstream
        sstr << FragmentShaderStream.rdbuf(); 

		// Store the string   
        FragmentShaderCode = sstr.str(); 

		// Close the file after reading        
        FragmentShaderStream.close();         
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile the vertex shader
    printf("Compiling shader : %s\n", vertex_file_path);

	// Get shader source code as a C-string
    char const* VertexSourcePointer = VertexShaderCode.c_str(); 

	// Load the source code into the shader
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL); 

	// Compile the vertex shader
    glCompileShader(VertexShaderID); 

    // Check if the vertex shader compiled successfully
	// Check compile status
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result); 

	// Get error log length
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength); 
    
	// If there is an error log
	if (InfoLogLength > 0) { 
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        
		// Print error log
		printf("%s\n", &VertexShaderErrorMessage[0]); 
    }

    // Compile the fragment shader
    printf("Compiling shader : %s\n", fragment_file_path);
    
	// Get shader source code as a C-string
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();

	// Load the source code into the shader 
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL); 

	// Compile the fragment shader
    glCompileShader(FragmentShaderID); 

    // Check if the fragment shader compiled successfully
	// Check compile status
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result); 

	// Get error log length
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength); 

	// If there is an error log
    if (InfoLogLength > 0) { 
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);

		// Print error log
        printf("%s\n", &FragmentShaderErrorMessage[0]); 
    }

    // Link the shaders to create a program
    printf("Linking program\n");

	// Create a shader program
    GLuint ProgramID = glCreateProgram(); 

	// Attach the vertex shader
    glAttachShader(ProgramID, VertexShaderID); 

	// Attach the fragment shader
    glAttachShader(ProgramID, FragmentShaderID); 

	// Link the shaders into a program
    glLinkProgram(ProgramID); 

    // Check if the program linked successfully
	// Check link status
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result); 

	// Get error log length
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength); 
    
	// If there is an error log
	if (InfoLogLength > 0) { 
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        
		// Print error log
		printf("%s\n", &ProgramErrorMessage[0]); 
    }

    // Detach and delete shaders after linking
    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);
    
	// Delete vertex shader
	glDeleteShader(VertexShaderID); 

	// Delete fragment shader
    glDeleteShader(FragmentShaderID); 

	// Return the program ID
    return ProgramID; 
}


// Main function to set up the window and OpenGL context
int main(void)
{
    // Initialize GLFW (graphics library for window and input management)
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    // Set GLFW window hints for OpenGL version and profile
	// Enable 4x antialiasing
    glfwWindowHint(GLFW_SAMPLES, 4); 

	// OpenGL version 3.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 

	// OpenGL version 3.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 

	// Make OpenGL context forward-compatible
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

	// Use core profile (no fixed-function pipeline)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(950, 950, u8"ΑΣΚΗΣΗ 1Β - 2024", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Make the OpenGL context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW (extension wrangler for OpenGL)
    // Enable experimental extensions
    glewExperimental = true; 
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure the escape key is captured
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Set background color to black
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Enable depth testing
    glEnable(GL_DEPTH_TEST); 

    // Generate and bind a vertex array object (VAO)
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Load and compile shaders, creating the shader program
    GLuint programID = LoadShaders("P1BVertexShader.vertexshader", "P1BFragmentShader.fragmentshader");

    // Get the location of the MVP (Model-View-Projection) matrix in the shader program
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    // Define projection matrix (perspective projection)
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);

    // Define camera view matrix (looking at origin from camera position)
    glm::mat4 View = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 20.0f),  // Camera position in world space
        glm::vec3(0.0f, 0.0f, 0.25f),  // Look at the origin
        glm::vec3(0.0f, 1.0f, 0.0f));  // Define 'up' direction

    // Define model matrix (identity matrix for untransformed object)
    glm::mat4 Model = glm::mat4(1.0f);

    // ModelViewProjection matrix = Projection * View * Model
    glm::mat4 MVP = Projection * View * Model;

    // Set the size for the object to be rendered
    GLfloat len = 5.0f, wid = 2.5f, heig = 2.5f;

	static const GLfloat cube[] = {
		//Front
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
		//Back
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
		//Right
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
		//Left
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
		//Up
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
		//Down
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
	float z = 0.0f;

	GLfloat charachter_vertices[] = {
		//Front
		x,y,z,		x,y - 0.5f,z,		x + 0.5f,y - 0.5f,z,	x,y,z,		x + 0.5f,y,z,		x + 0.5f,y - 0.5f,z,
		//Back
		x,y,z - 0.5f,		x,y - 0.5f,z - 0.5f,	x + 0.5f,y - 0.5f,z - 0.5f,	x,y,z - 0.5f,		x + 0.5f,y,z - 0.5f,	x + 0.5f,y - 0.5f,z - 0.5f,
		//Right
		x + 0.5f,y,z,		x + 0.5f,y - 0.5f,z,		x + 0.5f,y - 0.5f,z - 0.5f,	x + 0.5f,y,z,	x + 0.5f,y,z - 0.5f,	x + 0.5f,y - 0.5f,z - 0.5f,
		//Left
		x,y,z,		x,y - 0.5f,z,		x,y - 0.5f,z - 0.5f,	x,y,z,		x,y,z - 0.5f,		x,y - 0.5f,z - 0.5f,
		//Up
		x,y,z,		x,y,z - 0.5f,	x + 0.5f,y,z - 0.5f,	x,y,z,		x + 0.5f,y,z,	x + 0.5f,y,z - 0.5f,
		//Down
		x,y - 0.5f,z,		x,y - 0.5f,z - 0.5f,	x + 0.5f,y - 0.5f,z - 0.5f,		x,y - 0.5f,z,		x + 0.5f,y - 0.5f,z,		x + 0.5f,y - 0.5f,z - 0.5f
	};

	GLfloat r2 = 1.0f;
	GLfloat g2 = 1.0f;
	GLfloat b2 = -0.25f;

	static const GLfloat colorChar[] = {
		r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,
		r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,
		r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,
		r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,
		r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,
		r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a,	r2,  g2,  b2,a
	};

	// Create and initialize vertex buffer for the cube
	GLuint vertexbuffer;

	// Generate the vertex buffer object
	glGenBuffers(1, &vertexbuffer); 

	// Bind the buffer to the array buffer target
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer); 

	// Copy the cube vertex data to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW); 

	// Create and initialize color buffer for the cube
	GLuint colorbuffer;

	// Generate the color buffer object
	glGenBuffers(1, &colorbuffer); 

	// Bind the buffer to the array buffer target
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer); 

	// Copy the color data to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW); 

	// Create and initialize vertex buffer for the character
	GLuint vertexbuffer2;

	// Generate the vertex buffer for character
	glGenBuffers(1, &vertexbuffer2); 

	// Bind the buffer to the array buffer target
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2); 

	// Copy character vertex data to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(charachter_vertices), charachter_vertices, GL_STATIC_DRAW); 

	// Create and initialize color buffer for the character
	GLuint colorbuffer2;

	// Generate the color buffer for character
	glGenBuffers(1, &colorbuffer2); 

	// Bind the buffer to the array buffer target
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2); 

	// Copy character color data to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorChar), colorChar, GL_STATIC_DRAW); 

	do {
		// Clear the screen (color and depth buffers)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use the compiled shader program
		glUseProgram(programID);

		// Update camera position/view
		camera_function();

		// Define the projection matrix (perspective projection with 45 degree field of view)
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		
		glm::mat4 ViewMatrix = getViewMatrix();
		
		// Identity matrix for model
		glm::mat4 ModelMatrix = glm::mat4(1.0f);
		
		// Multiply MVP matrices
		glm::mat4 MVP = Projection * ViewMatrix * ModelMatrix;
		
		// Update the shader with the new MVP matrix
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// Enable and bind the vertex buffer for the cube
		// Enable the first vertex attribute (position)
		glEnableVertexAttribArray(0); 
		
		// Bind the cube vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer); 
		glVertexAttribPointer(
			0,          // Attribute index
			3,          // Number of components (x, y, z)
			GL_FLOAT,   // Data type
			GL_FALSE,   // Normalize flag
			0,          // Stride
			(void*)0    // Offset to the start of the buffer
		);

		// Enable and bind the color buffer for the cube
		// Enable the second vertex attribute (color)
		glEnableVertexAttribArray(1); 

		// Bind the cube color buffer
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer); 
		glVertexAttribPointer(
			1,          // Attribute index
			4,          // Number of components (r, g, b, a)
			GL_FLOAT,   // Data type
			GL_FALSE,   // Normalize flag
			0,          // Stride
			(void*)0    // Offset to the start of the buffer
		);

		// Draw the cube (576 vertices)
		glDrawArrays(GL_TRIANGLES, 0, 576); 

		// Enable and bind the vertex buffer for the character
		// Enable the first vertex attribute (position)
		glEnableVertexAttribArray(0); 

		// Bind the character vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2); 
		glVertexAttribPointer(
			0,          // Attribute index
			3,          // Number of components (x, y, z)
			GL_FLOAT,   // Data type
			GL_FALSE,   // Normalize flag
			0,          // Stride
			(void*)0    // Offset to the start of the buffer
		);

		// Enable and bind the color buffer for the character
		// Enable the second vertex attribute (color)
		glEnableVertexAttribArray(1); 

		// Bind the character color buffer
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2); 
		glVertexAttribPointer(
			1,          // Attribute index
			4,          // Number of components (r, g, b, a)
			GL_FLOAT,   // Data type
			GL_FALSE,   // Normalize flag
			0,          // Stride
			(void*)0    // Offset to the start of the buffer
		);

		// Draw the character (36 vertices)
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Disable the vertex attribute array for position after drawing
		glDisableVertexAttribArray(0);

		// Move character based on input (x, y, z) and update vertex buffer
		moveChar(&x, &y, &z, charachter_vertices, window, vertexbuffer2);

		// Draw the updated character
		// Enable the position attribute
		glEnableVertexAttribArray(0); 

		// Bind the vertex buffer for the character
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2); 

		// Bind the color buffer for the character
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2); 

		// Specify the vertex format
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); 

		// Draw the character
		glDrawArrays(GL_TRIANGLES, 0, 36); 

		// Disable the vertex attribute array for position
		glDisableVertexAttribArray(0); 

		// Swap buffers to update the window content
		glfwSwapBuffers(window);

		// Poll for window events (keyboard, mouse, etc.)
		glfwPollEvents(); 

	} while (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup the buffers and program after the loop ends
	// Delete the cube vertex buffer
	glDeleteBuffers(1, &vertexbuffer); 

	// Delete the vertex array object
	glDeleteVertexArrays(1, &VertexArrayID); 

	// Delete the shader program
	glDeleteProgram(programID); 

	// Close the OpenGL window and terminate GLFW
	glfwTerminate();

    // Return from main function
	return 0; 
}
