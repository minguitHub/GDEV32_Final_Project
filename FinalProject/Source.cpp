#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

#include <iostream>
#include <cmath>
#include <math.h>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION // turns .h file to .cpp file
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// structs
/// <summary>
/// Struct containing data about a vertex
/// </summary>
struct Vertex
{
	GLfloat x, y, z;	// Position
	GLubyte r, g, b;	// Color
	GLfloat nx, ny, nz;	// normal vectors
};

// window size variables
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera movement variables
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f; // for creating uniform speed across different machines
float lastFrame = 0.0f;

float pitch = 0.0f;
float yaw = -90.0f;
float mouseLastXPos = 400.0f;
float mouseLastYPos = 300.0f;
bool firstMouseEnter = true;

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Programming Exercise 1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback); // set cursor to mouse calculatios

	// configuring mouse input for camera 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hides bouse and keeps at center

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// enabling depth test to avoid drawing overlaps
	glEnable(GL_DEPTH_TEST);

	// creating shader program
	Shader ourShader("source.vsh", "source.fsh");
	Shader shadowShader("shadowMapper.vsh", "shadowMapper.fsh");
	Shader skyboxShader("skybox.vsh", "skybox.fsh");

	Vertex cubeVertices[36];
	// data points
	{
							// positions			// colors		// normals
		//back
		cubeVertices[0] = { -0.5f, -0.5f, -0.5f,	255, 0, 0,		0.0f, 0.0f, -1.0f };
		cubeVertices[1] = { 0.5f, -0.5f, -0.5f,		255, 0, 0,		0.0f, 0.0f, -1.0f };
		cubeVertices[2] = { 0.5f,  0.5f, -0.5f,		255, 0, 0,		0.0f, 0.0f, -1.0f };
		cubeVertices[3] = { 0.5f,  0.5f, -0.5f,		255, 0, 0,		0.0f, 0.0f, -1.0f };
		cubeVertices[4] = { -0.5f,  0.5f, -0.5f,	255, 0, 0,		0.0f, 0.0f, -1.0f };
		cubeVertices[5] = { -0.5f, -0.5f, -0.5f,	255, 0, 0,		0.0f, 0.0f, -1.0f };

		//front
		cubeVertices[6] = { -0.5f, -0.5f,  0.5f,	0, 255, 0,		0.0f, 0.0f, 1.0f };
		cubeVertices[7] = { 0.5f, -0.5f,  0.5f,		0, 255, 0,		0.0f, 0.0f, 1.0f };
		cubeVertices[8] = { 0.5f,  0.5f,  0.5f,		0, 255, 0,		0.0f, 0.0f, 1.0f };
		cubeVertices[9] = { 0.5f,  0.5f,  0.5f,		0, 255, 0,		0.0f, 0.0f, 1.0f };
		cubeVertices[10] = { -0.5f,  0.5f,  0.5f,	0, 255, 0,		0.0f, 0.0f, 1.0f };
		cubeVertices[11] = { -0.5f, -0.5f,  0.5f,	0, 255, 0,		0.0f, 0.0f, 1.0f };

		//left
		cubeVertices[12] = { -0.5f,  0.5f,  0.5f,	0, 0, 255,		-1.0f, 0.0f, 0.0f };
		cubeVertices[13] = { -0.5f,  0.5f, -0.5f,	0, 0, 255,		-1.0f, 0.0f, 0.0f };
		cubeVertices[14] = { -0.5f, -0.5f, -0.5f,	0, 0, 255,		-0.5f, 0.0f, 0.0f };
		cubeVertices[15] = { -0.5f, -0.5f, -0.5f,	0, 0, 255,		-1.0f, 0.0f, 0.0f };
		cubeVertices[16] = { -0.5f, -0.5f,  0.5f,	0, 0, 255,		-1.0f, 0.0f, 0.0f };
		cubeVertices[17] = { -0.5f,  0.5f,  0.5f,	0, 0, 255,		-1.0f, 0.0f, 0.0f };

		//right
		cubeVertices[18] = { 0.5f,  0.5f,  0.5f,	255, 255, 0,	1.0f, 0.0f, 0.0f };
		cubeVertices[19] = { 0.5f,  0.5f, -0.5f,	255, 255, 0,	1.0f, 0.0f, 0.0f };
		cubeVertices[20] = { 0.5f, -0.5f, -0.5f,	255, 255, 0,	1.0f, 0.0f, 0.0f };
		cubeVertices[21] = { 0.5f, -0.5f, -0.5f,	255, 255, 0,	1.0f, 0.0f, 0.0f };
		cubeVertices[22] = { 0.5f, -0.5f,  0.5f,	255, 255, 0,	1.0f, 0.0f, 0.0f };
		cubeVertices[23] = { 0.5f,  0.5f,  0.5f,	255, 255, 0,	1.0f, 0.0f, 0.0f };

		//bottom
		cubeVertices[24] = { -0.5f, -0.5f, -0.5f,	0, 255, 255,	0.0f, -1.0f, 0.0f };
		cubeVertices[25] = { 0.5f, -0.5f, -0.5f,	0, 255, 255,	0.0f, -1.0f, 0.0f };
		cubeVertices[26] = { 0.5f, -0.5f,  0.5f,	0, 255, 255,	0.0f, -1.0f, 0.0f };
		cubeVertices[27] = { 0.5f, -0.5f,  0.5f,	0, 255, 255,	0.0f, -1.0f, 0.0f };
		cubeVertices[28] = { -0.5f, -0.5f,  0.5f,	0, 255, 255,	0.0f, -1.0f, 0.0f };
		cubeVertices[29] = { -0.5f, -0.5f, -0.5f,	0, 255, 255,	0.0f, -1.0f, 0.0f };

		//top
		cubeVertices[30] = { -0.5f,  0.5f, -0.5f,	255, 0, 255,	0.0f, 1.0f, 0.0f };
		cubeVertices[31] = { 0.5f,  0.5f, -0.5f,	255, 0, 255,	0.0f, 1.0f, 0.0f };
		cubeVertices[32] = { 0.5f,  0.5f,  0.5f,	255, 0, 255,	0.0f, 1.0f, 0.0f };
		cubeVertices[33] = { 0.5f,  0.5f,  0.5f,	255, 0, 255,	0.0f, 1.0f, 0.0f };
		cubeVertices[34] = { -0.5f,  0.5f,  0.5f,	255, 0, 255,	0.0f, 1.0f, 0.0f };
		cubeVertices[35] = { -0.5f,  0.5f, -0.5f,	255, 0, 255,	0.0f, 1.0f, 0.0f };
	}

	Vertex planeVertices[6];
	// data points
	{
							// positions			// colors		// normals
		//bottom
		planeVertices[0] = { -0.5f, 0.0f, -0.5f,	128, 128, 128,	0.0f, 1.0f, 0.0f };
		planeVertices[1] = { 0.5f, 0.0f, -0.5f,		128, 128, 128,	0.0f, 1.0f, 0.0f };
		planeVertices[2] = { 0.5f, 0.0f,  0.5f,		128, 128, 128,	0.0f, 1.0f, 0.0f };
		planeVertices[3] = { 0.5f, 0.0f,  0.5f,		128, 128, 128,	0.0f, 1.0f, 0.0f };
		planeVertices[4] = { -0.5f, 0.0f,  0.5f,	128, 128, 128,	0.0f, 1.0f, 0.0f };
		planeVertices[5] = { -0.5f, 0.0f, -0.5f,	128, 128, 128,	0.0f, 1.0f, 0.0f };
	}

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// creating cubeVAO, cubeVBO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO); // creating vertex array object
	glGenBuffers(1, &cubeVBO); // creating vertex buffer object
	glBindVertexArray(cubeVAO); // VAO must be binded before VBO
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	// defining how OpenGL should interpret the data
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // position
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, r))); // color
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, nx))); // normal
	// unbind cubeVBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// creating planeVAO, planeVBO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO); // creating vertex array object
	glGenBuffers(1, &planeVBO); // creating vertex buffer object
	glBindVertexArray(planeVAO); // VAO must be binded before VBO
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	// defining how OpenGL should interpret the data
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // position
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, r))); // color
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, nx))); // normal
	// unbind planeVBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// creating skyboxVAO, skyboxVBO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO); // creating vertex array object
	glGenBuffers(1, &skyboxVBO); // creating vertex buffer object
	glBindVertexArray(skyboxVAO); // VAO must be binded here before VBO
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	// defining how OpenGL should interpret the data
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0); // position
	// unbind planeVBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// SKYBOX
	unsigned int skyboxTexture;
	int skyboxImageWidth, skyboxImageHeight, nrChannels;
	
	//std::vector<std::string> skyboxFaces
	std::string skyboxFaces[6]
	{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"
	};

	glGenTextures(1, &skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	for (unsigned int i = 0; i < (sizeof(skyboxFaces)/sizeof(*skyboxFaces)); i++)
	{
		unsigned char* data = stbi_load(skyboxFaces[i].c_str(), &skyboxImageWidth, &skyboxImageHeight, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, //cubemap has 6 faces and can be incremented like enum 
				0, GL_RGB, skyboxImageWidth, skyboxImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
		}
		else
		{
			std::cout << "Cubemap failed to load." << std::endl;
		}
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	// SHADOWS
	// generating framebuffer for depth map
	unsigned int shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);

	// creating 2D texture for the fbo shadow/depth buffer
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	unsigned int shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// attach shadow texture to fbo depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// check if fbo is okay
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Error! Framebuffer not complete!" << std::endl;
	}

	// activate skybox shader
	skyboxShader.use();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skyboxTex"), 0);

	// activate main shader
	ourShader.use();

	//===================
	// LIGHTING UNIFORMS
	//===================
	// light color
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		int lightColorLoc = glGetUniformLocation(ourShader.ID, "lightColor");
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	
	// directional light uniforms
		// directional light position
		glm::vec3 directionalLightPos(5.0f, 8.0f, 2.0f);
		int directionalLightPosLoc = glGetUniformLocation(ourShader.ID, "directionalLightPos");
		glUniform3fv(directionalLightPosLoc, 1, glm::value_ptr(directionalLightPos));

		// directional light ambient intensity
		glm::vec3 directionalLightAmbientIntensity(0.2f, 0.2f, 0.2f);
		int directionalLightAmbientIntensityLoc = glGetUniformLocation(ourShader.ID, "directionalLightAmbientIntensity");
		glUniform3fv(directionalLightAmbientIntensityLoc, 1, glm::value_ptr(directionalLightAmbientIntensity));

		// directional light diffuse intensity
		glm::vec3 directionalLightDiffuseIntensity(1.0f, 1.0f, 1.0f);
		int directionalLightDiffuseIntensityLoc = glGetUniformLocation(ourShader.ID, "directionalLightDiffuseIntensity");
		glUniform3fv(directionalLightDiffuseIntensityLoc, 1, glm::value_ptr(directionalLightDiffuseIntensity));

		// directional light specular intensity
		glm::vec3 directionalLightSpecularIntensity(1.0f, 1.0f, 1.0f);
		int directionalLightSpecularIntensityLoc = glGetUniformLocation(ourShader.ID, "directionalLightSpecularIntensity");
		glUniform3fv(directionalLightSpecularIntensityLoc, 1, glm::value_ptr(directionalLightSpecularIntensity));

	// The Rendering Loop
	while (!glfwWindowShouldClose(window))
	{
		// camera calculations
		float currentFrame = glfwGetTime(); // movement speed calculations
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		int eyePosLoc = glGetUniformLocation(ourShader.ID, "eyePos");
		glUniformMatrix3fv(eyePosLoc, 1, GL_FALSE, glm::value_ptr(cameraPos));

		// input
		processInput(window);

		// rendering
		glClear(GL_DEPTH_BUFFER_BIT);
		
		// activate skybox shader
		glDepthMask(GL_FALSE);
		skyboxShader.use();

		// skybox uniform locations and drawing
		int skyboxProjectionLoc = glGetUniformLocation(skyboxShader.ID, "skyboxProjection");
		glm::mat4 skyboxProjection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
		glUniformMatrix4fv(skyboxProjectionLoc, 1, GL_FALSE, glm::value_ptr(skyboxProjection));

		int skyboxViewLoc = glGetUniformLocation(skyboxShader.ID, "skyboxView");
		glm::mat4 skyboxView = glm::mat4(glm::mat3(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)));
		glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(skyboxView));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glBindVertexArray(skyboxVAO);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glDepthMask(GL_TRUE);
		
		//// activate shadow map shader
		//shadowShader.use();

		//// uniform locations
		//// projection from light pov for shadow map
		//int lightProjectionLoc = glGetUniformLocation(shadowShader.ID, "lightProjection");
		//int lightModelLoc = glGetUniformLocation(shadowShader.ID, "lightModel");
		//int lightViewLoc = glGetUniformLocation(shadowShader.ID, "lightView");

		//// lightmvp transformation for the shadow map
		//glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 30.f); // left, right, up, down, near, far
		//glUniformMatrix4fv(lightProjectionLoc, 1, GL_FALSE, glm::value_ptr(lightProjection));

		//glm::mat4 lightView = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		//lightView = glm::lookAt(directionalLightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.1f, 0.0f));
		//glUniformMatrix4fv(lightViewLoc, 1, GL_FALSE, glm::value_ptr(lightView));

		//glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		//glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		//glClear(GL_DEPTH_BUFFER_BIT);
		//				// -------------------
		//				//		  PLANE
		//				// -------------------

		//				// creating transformations (MVP) for plane
		//				// general pattern: intialize -> MVP -> glUniformMatrix -> draw

		//				// plane
		//				glBindVertexArray(planeVAO); // start drawing planes
		//				glm::mat4 model = glm::mat4(1.0f); // initialize identity matrix

		//				model = glm::scale(model, glm::vec3(20.0f, 0.0f, 20.0f));

		//				glUniformMatrix4fv(lightModelLoc, 1, GL_FALSE, glm::value_ptr(model));

		//				glDrawArrays(GL_TRIANGLES, 0, 6);

		//				// -------------------
		//				//		  CUBE
		//				// -------------------

		//				// creating transformations (MVP) for cube
		//				// general pattern: intialize -> MVP -> glUniformMatrix -> draw

		//				// cube 1
		//				glBindVertexArray(cubeVAO); // start drawing cubes
		//				model = glm::mat4(1.0f); // initialize identity matrix

		//				model = glm::translate(model, glm::vec3(0.0f, 2.0f, -2.0f));
		//				model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));

		//				glUniformMatrix4fv(lightModelLoc, 1, GL_FALSE, glm::value_ptr(model));

		//				glDrawArrays(GL_TRIANGLES, 0, 36);

		//				// cube 2
		//				model = glm::mat4(1.0f); // initialize identity matrix

		//				model = glm::translate(model, glm::vec3(-0.25f, 0.25f, -0.5f));
		//				model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//				model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//				model = glm::scale(model, glm::vec3(0.50f, 0.50f, 0.50f));

		//				glUniformMatrix4fv(lightModelLoc, 1, GL_FALSE, glm::value_ptr(model));

		//				glDrawArrays(GL_TRIANGLES, 0, 36);

		//				// cube 3
		//				model = glm::mat4(1.0f); // initialize identity matrix

		//				model = glm::translate(model, glm::vec3(-4.5f, 0.5f, -4.5f));
		//				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		//				glUniformMatrix4fv(lightModelLoc, 1, GL_FALSE, glm::value_ptr(model));

		//				glDrawArrays(GL_TRIANGLES, 0, 36);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//// main render
		//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		//glClearColor(0.098f, 0.098f, 0.4392f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the screen using the color that was set in previous line 

		//// activate main shader
		//ourShader.use();

		//// main transformation uniform location
		//int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
		//int modelLoc = glGetUniformLocation(ourShader.ID, "model");
		//int viewLoc = glGetUniformLocation(ourShader.ID, "view");
		//int lightProjectionLocInMain = glGetUniformLocation(ourShader.ID, "lightProjection"); // for updating the lightProj in main shader
		//int lightViewLocInMain = glGetUniformLocation(ourShader.ID, "lightView"); // for updating the lightView in main shader

		//// main transformations and uniforming
		//glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
		//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//glUniformMatrix4fv(lightProjectionLocInMain, 1, GL_FALSE, glm::value_ptr(lightProjection));

		//glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		//view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		//glUniformMatrix4fv(lightViewLocInMain, 1, GL_FALSE, glm::value_ptr(lightView));

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, shadowMap);
		//GLint shadowMapTextureLoc = glGetUniformLocation(ourShader.ID, "shadowMapTexture");
		//glUniform1i(shadowMapTextureLoc, 0);

		//// -------------------
		////		  PLANE
		//// -------------------

		//// creating transformations (MVP) for plane
		//// general pattern: intialize -> MVP -> glUniformMatrix -> draw

		//// plane
		//glBindVertexArray(planeVAO); // start drawing planes
		//model = glm::mat4(1.0f); // initialize identity matrix; definition moved up to shadow portion

		//model = glm::scale(model, glm::vec3(20.0f, 0.0f, 20.0f));

		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		//glDrawArrays(GL_TRIANGLES, 0, 6);

		//// -------------------
		////		  CUBE
		//// -------------------

		//// creating transformations (MVP) for cube
		//// general pattern: intialize -> MVP -> glUniformMatrix -> draw

		//// cube 1
		//glBindVertexArray(cubeVAO); // start drawing cubes
		//model = glm::mat4(1.0f); // initialize identity matrix

		//model = glm::translate(model, glm::vec3(0.0f, 2.0f, -2.0f));
		//model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));

		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		//glDrawArrays(GL_TRIANGLES, 0, 36);

		//// cube 2
		//model = glm::mat4(1.0f); // initialize identity matrix

		//model = glm::translate(model, glm::vec3(-0.25f, 0.25f, -0.5f));
		//model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.50f, 0.50f, 0.50f));

		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		//glDrawArrays(GL_TRIANGLES, 0, 36);

		//// cube 3
		//model = glm::mat4(1.0f); // initialize identity matrix

		//model = glm::translate(model, glm::vec3(-4.5f, 0.5f, -4.5f));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		//glDrawArrays(GL_TRIANGLES, 0, 36);

		// check and call events and swap the buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// de-allocating resources
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);

	glfwTerminate();
	return 0;
}

// for resizing windows
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// specifies the space where all the elements are rendered
	// not necessary for it to be the same size as the window
	// first two arguments are lower left and last two arguments are upper right
	glViewport(0, 0, width, height);
}

// for input control in glfw
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

	const float cameraSpeed = 3 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// for mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	xpos = static_cast<float>(xpos);
	ypos = static_cast<float>(ypos);

	if (firstMouseEnter)
	{
		mouseLastXPos = xpos;
		mouseLastYPos = ypos;
		firstMouseEnter = false;
	}

	float xOffset = xpos - mouseLastXPos;
	float yOffset = mouseLastYPos - ypos;
	mouseLastXPos = xpos;
	mouseLastYPos = ypos;

	float sensitivity = 0.1f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}