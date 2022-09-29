#include <GL/glew.h>
#include <GL/GLU.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
glm::vec3 updateVel(glm::vec3 curVel, glm::vec3 Pos, float deltaTime); // hard coding...



// initializing setup
const unsigned int WIDTH = 1200;
const unsigned int HEIGHT = 900;

// camera setup
Camera camera(glm::vec3(-4.0f, 1.0f, 8.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float totalTime = 0.0f;

// sphere setting
float radius = 0.25f;
glm::vec3 Pos(-3.0f, 4.0f, -3.0f);
glm::vec3 Vel(1.5f, 0.0f, 1.5f);
glm::vec3 gravity(0.0f, -9.8f, 0.0f);
float elasticity = 0.8f;


int main() {
	// Initializing and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "BounceBall", NULL, NULL); // window °´Ã¼ »ý¼º
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << '\n';
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);					  
	glfwSetScrollCallback(window, scroll_callback);						  
																		  
	// tell GLFW to capture our mouse									  
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);		  

	glewExperimental = GL_TRUE;

	GLenum errorCode = glewInit();
	if (GLEW_OK != errorCode) {

		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	///////// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader program
	Shader lightingShader("light.vs", "light.fs");
	Shader sphereShader("light.vs", "light.fs");
	Shader trajectory("line.vs", "line.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	float vertices[] = {
		// positions          // normals         
		-3.0f, -1.0f, -3.0f,  0.0f,  0.0f, -1.0f,
		 3.0f, -1.0f, -3.0f,  0.0f,  0.0f, -1.0f,
		 3.0f,  0.0f, -3.0f,  0.0f,  0.0f, -1.0f,
		 3.0f,  0.0f, -3.0f,  0.0f,  0.0f, -1.0f,
		-3.0f,  0.0f, -3.0f,  0.0f,  0.0f, -1.0f,
		-3.0f, -1.0f, -3.0f,  0.0f,  0.0f, -1.0f,
		  			    
		-3.0f, -1.0f,  3.0f,  0.0f,  0.0f, 1.0f, 
		 3.0f, -1.0f,  3.0f,  0.0f,  0.0f, 1.0f, 
		 3.0f,  0.0f,  3.0f,  0.0f,  0.0f, 1.0f, 
		 3.0f,  0.0f,  3.0f,  0.0f,  0.0f, 1.0f, 
		-3.0f,  0.0f,  3.0f,  0.0f,  0.0f, 1.0f, 
		-3.0f, -1.0f,  3.0f,  0.0f,  0.0f, 1.0f, 
		 			    
		-3.0f,  0.0f,  3.0f, -1.0f,  0.0f,  0.0f,
		-3.0f,  0.0f, -3.0f, -1.0f,  0.0f,  0.0f,
		-3.0f, -1.0f, -3.0f, -1.0f,  0.0f,  0.0f,
		-3.0f, -1.0f, -3.0f, -1.0f,  0.0f,  0.0f,
		-3.0f, -1.0f,  3.0f, -1.0f,  0.0f,  0.0f,
		-3.0f,  0.0f,  3.0f, -1.0f,  0.0f,  0.0f,
		  			    
		 3.0f,  0.0f,  3.0f,  1.0f,  0.0f,  0.0f,
		 3.0f,  0.0f, -3.0f,  1.0f,  0.0f,  0.0f,
		 3.0f, -1.0f, -3.0f,  1.0f,  0.0f,  0.0f,
		 3.0f, -1.0f, -3.0f,  1.0f,  0.0f,  0.0f,
		 3.0f, -1.0f,  3.0f,  1.0f,  0.0f,  0.0f,
		 3.0f,  0.0f,  3.0f,  1.0f,  0.0f,  0.0f,
		  			   
		-3.0f, -1.0f, -3.0f,  0.0f, -1.0f,  0.0f,
		 3.0f, -1.0f, -3.0f,  0.0f, -1.0f,  0.0f,
		 3.0f, -1.0f,  3.0f,  0.0f, -1.0f,  0.0f,
		 3.0f, -1.0f,  3.0f,  0.0f, -1.0f,  0.0f,
		-3.0f, -1.0f,  3.0f,  0.0f, -1.0f,  0.0f,
		-3.0f, -1.0f, -3.0f,  0.0f, -1.0f,  0.0f,
		  			  	 
		-3.0f,  0.0f, -3.0f,  0.0f,  1.0f,  0.0f,
		 3.0f,  0.0f, -3.0f,  0.0f,  1.0f,  0.0f,
		 3.0f,  0.0f,  3.0f,  0.0f,  1.0f,  0.0f,
		 3.0f,  0.0f,  3.0f,  0.0f,  1.0f,  0.0f,
		-3.0f,  0.0f,  3.0f,  0.0f,  1.0f,  0.0f,
		-3.0f,  0.0f, -3.0f,  0.0f,  1.0f,  0.0f
	};

	float sphereVerticesIdx[831];
	int count = 0;
	for (float phi = 0.0f; phi <= 180.0f; phi += 15.0f) {
		if (phi == 0.0f || phi == 180.0f) {
			sphereVerticesIdx[count++] = 0.0f;
			sphereVerticesIdx[count++] = radius * cos(glm::radians(phi));
			sphereVerticesIdx[count++] = 0.0f;
	
			continue;
		}
	
		for (float theta = 0.0f; theta <= 360.0f; theta += 15.0f) {
			sphereVerticesIdx[count++] = radius * sin(glm::radians(phi)) * cos(glm::radians(theta));
			sphereVerticesIdx[count++] = radius * cos(glm::radians(phi));
			sphereVerticesIdx[count++] = radius * sin(glm::radians(phi)) * sin(glm::radians(theta));
		}
	}				
	
	unsigned int Idx[1584];
	count = 0;
	int j = 0;
	while (count < 1584) {
		for (int i = 1; i <= 24; i++) {
			Idx[count++] = 0;
			Idx[count++] = i + 1;
			Idx[count++] = i;
		}
		for (int i = 1; i <= 249; i++) {
			Idx[count++] = i;
			Idx[count++] = i + 1;
			Idx[count++] = i + 25;

			Idx[count++] = i + 1;
			Idx[count++] = i + 26;
			Idx[count++] = i + 25;

			j++;

			if (j % 24 == 0) {
				i++;
				j = 0;
			}
		}
		for (int i = 251; i <= 274; i++) {
			Idx[count++] = 276;
			Idx[count++] = i + 1;
			Idx[count++] = i;
		}
	}
	
	float sphereVertices[9504];
	count = 0;
	for (int i = 0; i < 1584; i++) {
		// position
		sphereVertices[count++] = sphereVerticesIdx[3 * Idx[i] + 0];
		sphereVertices[count++] = sphereVerticesIdx[3 * Idx[i] + 1];
		sphereVertices[count++] = sphereVerticesIdx[3 * Idx[i] + 2];
		// normal
		for (int j = 0; j < 3; j++) sphereVertices[count++] = sphereVertices[count - 3] / radius;
	}

	float trajPoint[] = { 0.0f, 0.0f, 0.0f };

	std::vector<glm::vec3> trajPos;

	unsigned int cubeVBO, cubeVAO, sphereVBO, sphereVAO, trajVBO, trajVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);		
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);	

	glGenVertexArrays(1, &sphereVAO);
	glGenBuffers(1, &sphereVBO);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), sphereVertices, GL_STATIC_DRAW);
	glBindVertexArray(sphereVAO);
	
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &trajVAO);
	glGenBuffers(1, &trajVBO);
	glBindBuffer(GL_ARRAY_BUFFER, trajVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(trajPoint), trajPoint, GL_STATIC_DRAW);
	glBindVertexArray(trajVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = 1 / 60.0f;
		totalTime += deltaTime;

		// input
		processInput(window);

		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// activate shader
		lightingShader.use();
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("viewPos", camera.Position);

		// light properties
		lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// create transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		
		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);

		// render containers
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// render sphere
		sphereShader.use();
		sphereShader.setVec3("objectColor", 0.2f, 0.33f, 0.45f);
		sphereShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
		sphereShader.setVec3("viewPos", camera.Position);
		sphereShader.setMat4("projection", projection);
		sphereShader.setMat4("view", view);

		sphereShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		sphereShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		sphereShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		glBindVertexArray(sphereVAO);

		Vel = updateVel(Vel, Pos, deltaTime);
		Pos += deltaTime * Vel;
		model = glm::mat4(1.0f);
		model = glm::translate(model, Pos);
		sphereShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 1584);


		trajPos.push_back(Pos);

		for (int i = 0; i < trajPos.size(); i++) {
			trajectory.use();

			trajectory.setMat4("projection", projection);
			trajectory.setMat4("view", view);

			model = glm::mat4(1.0f);
			model = glm::translate(model, trajPos[i]);
			trajectory.setMat4("model", model);

			glBindVertexArray(trajVAO);
			glPointSize(3.0f);
			glDrawArrays(GL_POINTS, 0, 36);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &sphereVAO);
	glDeleteVertexArrays(1, &trajVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &sphereVBO);
	glDeleteBuffers(1, &trajVBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	const float cameraSpeed = static_cast<float>(2.5f * deltaTime);	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UPWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWNWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		Vel += glm::vec3(0.0f, 0.5f, 0.0f);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		deltaTime = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		Pos = glm::vec3(-3.0f, 4.0f, -3.0f);
		Vel = glm::vec3(1.5f, 0.0f, 1.5f);
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

glm::vec3 updateVel(glm::vec3 curVel, glm::vec3 Pos, float deltaTime) {
	if (abs(Pos.x) <= 3.0f && abs(Pos.z) <= 3.0f && Pos.y <= radius + 5e-2) {
		curVel.y *= - elasticity;
		return curVel;
	}

	return (curVel + deltaTime * gravity);
}