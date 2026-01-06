#include "GLShader.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLFWwindow* window = nullptr;
GLShader* shader = nullptr;
glm::vec3 cameraPos(0.0f, 0.0f, 2.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
float yaw = -90.0f, pitch = 0.0f;
bool firstMouse = true;
float lastX = 640, lastY = 360;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Buffers de OpenGL
GLuint vao, vbo, ebo;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 10.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
}

bool initGL() {
	// Inicializar GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return false;
	}

	// Configurar GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Crear ventana
	window = glfwCreateWindow(1280, 720, "Voxel Engine - Test", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Inicializar GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// Configurar OpenGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;

	return true;
}

int main() {
	if (!initGL()) {
		return -1;
	}

	// Datos del cubo - SOLO CARA FRONTAL (4 vértices)
	const GLfloat voxel[] = {
		// Front face
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f, 1.0f,

		// Back face
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f, 0.0f,

		// Right face
		 0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,     0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,     1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,     1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,     0.0f, 1.0f,

		// Left face
		-0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,     1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,     1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,     0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,     0.0f, 0.0f,

		// Top face
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f, 0.0f,

		// Bottom face
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f, 1.0f
	};

	// ÍNDICES CORRECTOS para formar 2 triángulos (quad)
	const GLuint voxelIndices[] = {
		0, 1, 2, 2, 3, 0,       // Front
		4, 5, 6, 6, 7, 4,       // Back
		8, 9, 10, 10, 11, 8,    // Right
		12, 13, 14, 14, 15, 12, // Left
		16, 17, 18, 18, 19, 16, // Top
		20, 21, 22, 22, 23, 20  // Bottom
	};

	// Crear y configurar VAO, VBO, EBO
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	// 1. Vértices
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(voxel), voxel, GL_STATIC_DRAW);

	// 2. Índices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(voxelIndices), voxelIndices, GL_STATIC_DRAW);

	// 3. Atributos de vértice
	// Posición (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// Normal (location = 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// UV (location = 2)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Desvincular buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); // IMPORTANTE: desvincular VAO al final

	// Cargar shaders
	shader = new GLShader();
	if (!shader->load("assets/shaders/basicLight.vert", "assets/shaders/basicLight.frag")) {
		std::cerr << "Failed to load shaders" << std::endl;
		return -1;
	}

	// Configurar matriz de proyección
	glm::mat4 projection = glm::perspective(
		glm::radians(60.0f),
		1280.0f / 720.0f,
		0.1f,
		100.0f
	);

	// Variables para FPS
	int frameCount = 0;
	float lastTime = glfwGetTime();
	float fpsUpdateInterval = 1.0f;

	// Bucle principal
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Input
		processInput(window);

		// Limpiar buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Matriz de vista
		glm::mat4 view = glm::lookAt(
			cameraPos,
			cameraPos + cameraFront,
			cameraUp
		);

		// Matriz de modelo (identidad - en origen)
		glm::mat4 model = glm::mat4(1.0f);

		// Usar shader
		shader->use();

		// Pasar matrices al shader
		shader->setMat4("proj", projection);
		shader->setMat4("view", view);
		shader->setVec3("camPos", cameraPos);

		// Dibujar geometría
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, sizeof(voxelIndices) / sizeof(GLuint), GL_UNSIGNED_INT, 0); // 6 índices = 2 triángulos
		glBindVertexArray(0);

		// Actualizar FPS en el título
		frameCount++;
		if (currentFrame - lastTime >= fpsUpdateInterval) {
			float fps = frameCount / (currentFrame - lastTime);
			std::string title = "Voxel Test - FPS: " + std::to_string((int)fps) +
				" - Camera: (" +
				std::to_string((int)cameraPos.x) + ", " +
				std::to_string((int)cameraPos.y) + ", " +
				std::to_string((int)cameraPos.z) + ")";
			glfwSetWindowTitle(window, title.c_str());
			frameCount = 0;
			lastTime = currentFrame;
		}

		// Intercambiar buffers y procesar eventos
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Limpiar
	delete shader;

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	glfwTerminate();
	return 0;
}