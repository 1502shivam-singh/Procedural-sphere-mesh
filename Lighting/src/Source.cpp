#include "Shader.h"
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include <vector>

#include<iostream>

void gladInitializing() {		//initializes glad and checks whether glad is initialized or not
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		std::cin.get();
	}
}

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
double lastX = 320.0;
double lastY = 240.0;
Camera camera(glm::vec3(0.0f, 0.0f, 1.0f));
int SCR_WIDTH = 640, SCR_HEIGHT = 480;

void frame_buffer_resizing(GLFWwindow* window, int width, int height) {		//acts as callback in function responsible for size change
	glViewport(0, 0, width, height);
}

// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow *window)	//checks for input every frame
{
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

int main() {
	if (glfwInit()) {	//Initialising glfw
		std::cout << "initialised";
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //providing a window hint (meta data stored with window to tell glfw(in this case) how window wants to be treated)

	auto* window = glfwCreateWindow(640, 480, "Hello OpenGL", nullptr, nullptr);	//window creation and checking code
	if (window) {
		std::cout << "window created successfully";
	}
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, frame_buffer_resizing);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	gladInitializing();


	Shader* shader = new Shader("shaders/VertexShader.txt", "shaders/FragmentShader.txt");	//Shader (Vertex, Fragment) creation, compilation, error checking and linking shaders into shaderprogram.
	
	//MY CODE - Render time - 19.70s Stacks and Sectors = 1000
	int Stacks = 1000, Sectors = 1000;
	float Radius = 1.0f;
	std::vector<float> vertices;

	for (int i = 0; i <= Stacks; ++i) {
		float stackStep = i / (float)Stacks;
		float phi = (glm::pi <float>()/2) - stackStep * glm::pi <float>();	// phi is angle made by ray originating from origin, with xz - plane.
		float y = Radius*sinf(phi);
		// Loop Through Slices
		for (int j = 0; j < Sectors; ++j) {
			float sectorStep = j / (float)Sectors;
			float theta = sectorStep * (glm::pi <float>() * 2);	// theta is angle made by ray originating from origin, with xy - plane.
			// Calc The Vertex Positions
			float x = Radius*sinf(theta) * cosf(phi);
			float z = Radius*cosf(theta) * cosf(phi);
			// Push Back Vertex Data
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}

	/*
	diagram for indices
	v1  v1+1
	|   /|
	|  / |
	| /  |
	|/   |
	v2  v2+1
	*/

	std::vector<int> indices;
	int v1, v2;
	for (int i = 0; i < Stacks; ++i) {
		v1 = i*(Stacks);
		v2 = v1 + Sectors;
		for (int j = 0; j < Sectors; j++) {
			//for bottom stack only single triangle required
			if (j != 0) {
				indices.push_back(v1);
				j != Sectors - 1 ? indices.push_back(v1 + 1) : indices.push_back(v1 - Sectors + 1);
				indices.push_back(v2);
			}
			//for top stack also only single triangle required
			if (j != Sectors - 1) {
				j != Sectors - 1 ? indices.push_back(v1 + 1) : indices.push_back(v1 - Sectors + 1);
				indices.push_back(v2);
				j != Sectors - 1 ? indices.push_back(v2 + 1) : indices.push_back(v2 - Sectors + 1);
			}
			v1++, v2++;
		}
	}

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);



	//---------------------------------------------------Uncomment for using textures-------------------------------------------------------
	/*unsigned int tex1;
	glGenTextures(1, &tex1);

	glBindTexture(GL_TEXTURE_2D, tex1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	*/
	//int width, height, nrChannels;
	//unsigned char* imageData = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	//glGenerateMipmap(GL_TEXTURE_2D);

	//stbi_image_free(imageData);
	//---------------------------------------------------Uncomment for using textures-------------------------------------------------------

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glViewport(0, 0, 640, 480);

	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//input area
		processInput(window);

		//rendering area
		glEnable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 0.3f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//Flushes the color buffer with yellowish color 
		shader->use();
		glm::mat4 transform = glm::mat4(0.0f);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);
		shader->setMat4("transform", view);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size() * sizeof(float), GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	delete shader;
	std::cin.get();
}
