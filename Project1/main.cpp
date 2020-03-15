#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

const int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
const int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

glm::vec3 cameraPos = glm::vec3(1.0f, 1.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

GLuint screenShader;

double timeInterval = 0.0f, currentFrame = 0.0f, lastFrame = 0.0f;

double lastX = 400, lastY = 300;
double pitch = 0.0f, yaw = -90.0f;

bool keys[1000];
bool inverted = false;

void moveCamera()
{
	GLfloat cameraSpeed = 5.0f * timeInterval;
	if (keys[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_LEFT_SHIFT])
		cameraPos -= cameraSpeed * glm::vec3(0.0f, 1.0f, 0.0f);
	if (keys[GLFW_KEY_SPACE])
		cameraPos -= cameraSpeed * glm::vec3(0.0f, -1.0f, 0.0f);
	if (cameraPos.y < 1.0f)
		cameraPos.y = 1.0f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
	if (key == GLFW_KEY_I && action == GLFW_RELEASE) {
		if (!inverted) {
			Shader *s = new Shader("shaders/screen.vert", "shaders/screenInverted.frag");
			screenShader = s->id;
			inverted = true;
		}
		else {
			Shader *s = new Shader("shaders/screen.vert", "shaders/screen.frag");
			screenShader = s->id;
			inverted = false;
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	xoffset *= 0.05f;
	yoffset *= 0.05f;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0)
		pitch = 89.0;
	if (pitch < -89.0)
		pitch = -89.0;

	glm::vec3 front;
	front.x = cos((float)glm::radians(pitch)) * cos((float)glm::radians(yaw));
	front.y = sin((float)glm::radians(pitch));
	front.z = cos((float)glm::radians(pitch)) * sin((float)glm::radians(yaw));
	cameraFront = glm::normalize(front);
}

unsigned int loadCubemap(std::vector<std::string> &sides)
{
	unsigned int skyboxTexture;
	glGenTextures(1, &skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	int width, height;
	for (unsigned int i = 0; i < sides.size(); i++)
	{
		unsigned char* image = SOIL_load_image(sides[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	return skyboxTexture;
}

unsigned int cubeInit(unsigned int &cubeVBO) {
	float cubeVertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	unsigned int cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glBindVertexArray(cubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	return cubeVAO;
}

unsigned int skyboxInit() {
	float skyboxVertices[] = {
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

	unsigned int skyboxVBO, skyboxVAO;
	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	return skyboxVAO;
}

unsigned int planeInit() {
	float planeVertices[] = {
		 7.0f, -0.5f,  7.0f,  0.0f, 1.0f, 0.0f, 4.0f, 0.0f,
		-7.0f, -0.5f,  7.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-7.0f, -0.5f, -7.0f,  0.0f, 1.0f, 0.0f, 0.0f, 4.0f,
		 7.0f, -0.5f,  7.0f,  0.0f, 1.0f, 0.0f, 4.0f, 0.0f,
		-7.0f, -0.5f, -7.0f,  0.0f, 1.0f, 0.0f, 0.0f, 4.0f,
		 7.0f, -0.5f, -7.0f,  0.0f, 1.0f, 0.0f, 4.0f, 4.0f
	};

	unsigned int planeVAO, planeVBO;
	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	return planeVAO;
}

unsigned int quadInit() {
	float quadVertices[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	unsigned int quadVAO, quadVBO;
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	return quadVAO;
}

unsigned int billboardInit() {
	float billboardVertices[] = {
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

	unsigned int billboardVBO, billboardVAO;
	glGenBuffers(1, &billboardVBO);
	glBindBuffer(GL_ARRAY_BUFFER, billboardVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(billboardVertices), billboardVertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &billboardVAO);
	glBindVertexArray(billboardVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	return billboardVAO;
}

unsigned int framebufferInit(unsigned int &textureColorBuffer) {
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	unsigned int renderbuffer;
	glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

	return framebuffer;
}

GLuint genTexture(const char *path, bool RGBA = false) {
	int width, height;
	unsigned char *image;
	if (!RGBA) image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	else image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (!RGBA) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);

	return texture;
}

unsigned int wallInit()
{float wallVertices[] = {
		-1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

		-1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
	};

	unsigned int wallVAO, wallVBO;
	glGenBuffers(1, &wallVBO);
	glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), &wallVertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &wallVAO);
	glBindVertexArray(wallVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	return wallVAO;
}

unsigned int depthMapInit(unsigned int &depthMapFBO) {
	unsigned int depthMap;
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return depthMap;
}

int compare(std::pair<glm::vec3, float> a, std::pair<glm::vec3, float> b) {
	return a.second > b.second;
}

void renderScene(GLuint shader, unsigned int planeTexture, unsigned int planeVAO,
	unsigned int cubeTexture, unsigned int cubeVAO) {

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planeTexture);
	glBindVertexArray(planeVAO);
	glm::mat4 model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	glBindVertexArray(cubeVAO);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.5f, 0.5f, 2.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-0.5f, 1.5f, 2.0f));
	model = glm::rotate(model, glm::radians(-60.0f), glm::vec3(-1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

}

void renderTwoCubes(unsigned int cubeVAO, unsigned int cubeTexture, unsigned int shader, float scale) {
	glBindVertexArray(cubeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-5.0f, 0.8f, 5.0f));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(scale));
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-5.0f, 2.0f, 6.0f));
	model = glm::rotate(model, glm::radians(60.0f), glm::vec3(1.0f));
	model = glm::scale(model, glm::vec3(scale));
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main(void) {

	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Computer Graphics", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glewInit();

	Shader *s = new Shader("shaders/cube.vert", "shaders/cube.frag");
	GLuint cubeShader = s->id;
	s = new Shader("shaders/skybox.vert", "shaders/skybox.frag");
	GLuint skyboxShader = s->id;
	s = new Shader("shaders/lamp.vert", "shaders/lamp.frag");
	GLuint lampShader = s->id;
	s = new Shader("shaders/screen.vert", "shaders/screen.frag");
	screenShader = s->id;
	s = new Shader("shaders/billboard.vert", "shaders/billboard.frag");
	GLuint billboardShader = s->id;
	s = new Shader("shaders/cubeReflect.vert", "shaders/cubeReflect.frag");
	GLuint reflectShader = s->id;
	s = new Shader("shaders/shadowMapping.vert", "shaders/shadowMapping.frag");
	GLuint shadowShader = s->id;
	s = new Shader("shaders/shadowMappingDepth.vert", "shaders/shadowMappingDepth.frag");
	GLuint simpleDepthShader = s->id;
	s = new Shader("shaders/normalMapping.vert", "shaders/normalMapping.frag");
	GLuint normalShader = s->id;
	s = new Shader("shaders/cube.vert", "shaders/singleColor.frag");
	GLuint singleColorShader = s->id;

	unsigned int cubeVBO, cubeVAO = cubeInit(cubeVBO);

	unsigned int lampVAO;
	glGenVertexArrays(1, &lampVAO);
	glBindVertexArray(lampVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

	unsigned int skyboxVAO = skyboxInit();
	std::vector<std::string> skyboxSides
	{
		"skybox/DarkStormyRight.jpg",
		"skybox/DarkStormyLeft.jpg",
		"skybox/DarkStormyUp.jpg",
		"skybox/DarkStormyDown.jpg",
		"skybox/DarkStormyBack.jpg",
		"skybox/DarkStormyFront.jpg"
	};
	unsigned int skyboxTexture = loadCubemap(skyboxSides);

	unsigned int planeVAO = planeInit();

	unsigned int quadVAO = quadInit();

	unsigned int wallVAO = wallInit();

	unsigned int billboardVAO = billboardInit();
	std::vector<glm::vec3> billboardPositions
	{
		glm::vec3(4.5f, 0.0f, -0.48f),
		glm::vec3(5.5f, 0.0f, 0.51f),
		glm::vec3(7.0f, 0.0f, 0.7f),
		glm::vec3(3.3f, 0.0f, -2.3f),
		glm::vec3(2.6f, 0.0f, 1.6f),
		glm::vec3(4.5f, 1.0f, 0.0f),
		glm::vec3(3.5f, 1.0f, 1.0f)
	};

	unsigned int textureColorBuffer;
	unsigned int framebuffer = framebufferInit(textureColorBuffer);

	unsigned int depthMapFBO;
	unsigned int depthMap = depthMapInit(depthMapFBO);

	GLuint cubeTexture = genTexture("textures/box.jpg");
	GLuint planeTexture = genTexture("textures/floor.jpg");
	GLuint billboardTexture = genTexture("textures/billboard.png", true);
	GLuint wallTexture = genTexture("textures/wall.jpg");
	GLuint wallNormalTexture = genTexture("textures/wall_norm.jpg");
	

	//---------------------------------------------------------------------
	//---------------------------------------------------------------------

	while (!glfwWindowShouldClose(window))
	{
		glfwSetKeyCallback(window, key_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwPollEvents();

		currentFrame = glfwGetTime();
		timeInterval = currentFrame - lastFrame;
		lastFrame = currentFrame;

		moveCamera();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);


		glm::mat4 view, projection, model;
		glm::vec3 lampPosition = glm::vec3(0.0f, 4.0f, 3.0f);
		lampPosition.x = 2 * (float)sin(100 * glm::radians(glfwGetTime()));

		model = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		

//----------------------------------------------


		glm::mat4 lightProjection, lightView;
		float near_plane = 1.0f, far_plane = 10.0f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(lampPosition, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		glUseProgram(simpleDepthShader);
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeTexture);
		renderScene(simpleDepthShader, planeTexture, planeVAO, cubeTexture, cubeVAO);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glUseProgram(shadowShader);
		glUniform1i(glGetUniformLocation(shadowShader, "ourTexture"), 0);
		glUniform1i(glGetUniformLocation(shadowShader, "shadowMap"), 1);
		projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(glGetUniformLocation(shadowShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shadowShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniform3fv(glGetUniformLocation(shadowShader, "lampPosition"), 1, glm::value_ptr(lampPosition));
		glUniformMatrix4fv(glGetUniformLocation(shadowShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderScene(shadowShader, planeTexture, planeVAO, cubeTexture, cubeVAO);

//--------------------------------------------------------

		glUseProgram(normalShader);
		glUniform1i(glGetUniformLocation(normalShader, "ourTexture"), 0);
		glUniform1i(glGetUniformLocation(normalShader, "normalMap"), 1);
		glUniformMatrix4fv(glGetUniformLocation(normalShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(normalShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(glGetUniformLocation(normalShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(glGetUniformLocation(normalShader, "lampPosition"), 1, glm::value_ptr(lampPosition));
		glUniform3fv(glGetUniformLocation(normalShader, "cameraPosition"), 1, glm::value_ptr(cameraPos));
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, wallTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, wallNormalTexture);
		glBindVertexArray(wallVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

//------------------------------------------------------------

		glDepthFunc(GL_LEQUAL);
		glUseProgram(skyboxShader);
		glm::mat4 view2 = glm::mat4(glm::mat3(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"), 1, GL_FALSE, glm::value_ptr(view2));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

//------------------------------------------------------------------

		glUseProgram(reflectShader);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(reflectShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(reflectShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(reflectShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(glGetUniformLocation(reflectShader, "cameraPosition"), 1, glm::value_ptr(cameraPos));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

//------------------------------------------------------------------

		std::vector<std::pair<glm::vec3, float>> billboardDistances;
		for (unsigned int i = 0; i < billboardPositions.size(); i++)
		{
			float distance = glm::length(cameraPos - billboardPositions[i]);
			billboardDistances.push_back(std::pair<glm::vec3, float>(billboardPositions[i], distance));
		}
		std::sort(billboardDistances.begin(), billboardDistances.end(), compare);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(billboardShader);
		glUniformMatrix4fv(glGetUniformLocation(billboardShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(billboardShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(billboardVAO);
		glBindTexture(GL_TEXTURE_2D, billboardTexture);
		for (int i = 0; i < billboardDistances.size(); ++i)
		{
			glm::vec3 billboardPos = billboardDistances[i].first;
			model = glm::mat4(1.0f);
			model = glm::translate(model, billboardPos);
			model = glm::lookAt(billboardPos, cameraPos, glm::vec3(0, 1, 0));
			model = glm::inverse(model);
			glUniformMatrix4fv(glGetUniformLocation(billboardShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

//------------------------------------------------------------------

		glUseProgram(lampShader);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lampPosition);
		model = glm::scale(model, glm::vec3(0.25f));
		glUniformMatrix4fv(glGetUniformLocation(lampShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lampShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lampShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

//------------------------------------------------------------------

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		glUseProgram(cubeShader);
		glUniformMatrix4fv(glGetUniformLocation(cubeShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(cubeShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		renderTwoCubes(cubeVAO, cubeTexture, cubeShader, 1.0f);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(singleColorShader);
		glUniformMatrix4fv(glGetUniformLocation(singleColorShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(singleColorShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		renderTwoCubes(cubeVAO, 0, singleColorShader, 1.1f);

		glStencilMask(0xFF);
		glDisable(GL_STENCIL_TEST);

//---------------------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT );

		glUseProgram(screenShader);
		glBindVertexArray(quadVAO);
		glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lampVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteVertexArrays(1, &wallVAO);
	glDeleteVertexArrays(1, &billboardVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteFramebuffers(1, &depthMapFBO);
	glfwTerminate();
	return 0;
}