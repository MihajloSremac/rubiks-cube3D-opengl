
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <map>
#include <chrono>
#include <thread>
#include <iomanip>
#include <queue>
#include <vector>
#include <array>

using namespace std;

#include <GL/glew.h>  
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "TextRender.h"
#include "Timer.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);

void initCube();
void updateCubeState(int face, bool clockwise);
void updateRotation();
void drawCube(unsigned int shaderProgram);
void handleInput(GLFWwindow* window);
void importantCallbacks(GLFWwindow* window, int key, int scancode, int action, int mods);
string printTime(double elapsedTime);
void rotateFace(int face, bool clockwise);
string generateScramble();
void processNextMove();
void resetCube();

unsigned int VAOcube, VBOcube;

const glm::vec4 RED(1.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const glm::vec4 BLUE(0.0f, 0.0f, 1.0f, 1.0f);
const glm::vec4 ORANGE(1.0f, 0.5f, 0.0f, 1.0f);
const glm::vec4 WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 YELLOW(1.0f, 1.0f, 0.0f, 1.0f);

struct Cubie {
	glm::vec3 position;
	std::array<glm::vec4, 6> colors;
};

struct CubeState {
	std::vector<Cubie> cubies;
	float rotationX = 40.0f;
	float rotationY = -40.0f;
	glm::mat4 model = glm::mat4(1.0f);

	int currentLayer = -1;
	float layerRotation = 0.0f;
	bool isRotating = false;
	float rotationCounter = 0.0f;
	bool clockwise = true;
};

CubeState cubeState;
Timer timer;
string scramble = "";
queue<std::string> scrambleQueue;

float cubieVertices[] = {
	// Front face (Z+)
	-0.1f, -0.1f,  0.1f,     1.0f, 0.0f, 0.0f,
	 0.1f, -0.1f,  0.1f,     1.0f, 0.0f, 0.0f,
	 0.1f,  0.1f,  0.1f,     1.0f, 0.0f, 0.0f,
	 0.1f,  0.1f,  0.1f,     1.0f, 0.0f, 0.0f,
	-0.1f,  0.1f,  0.1f,     1.0f, 0.0f, 0.0f, 
	-0.1f, -0.1f,  0.1f,     1.0f, 0.0f, 0.0f, 

	// Back face (Z-)
	-0.1f, -0.1f, -0.1f,     1.0f, 0.5f, 0.0f,
	 0.1f, -0.1f, -0.1f,     1.0f, 0.5f, 0.0f, 
	 0.1f,  0.1f, -0.1f,     1.0f, 0.5f, 0.0f,
	 0.1f,  0.1f, -0.1f,     1.0f, 0.5f, 0.0f,
	-0.1f,  0.1f, -0.1f,     1.0f, 0.5f, 0.0f,
	-0.1f, -0.1f, -0.1f,     1.0f, 0.5f, 0.0f,

	// Top face (Y+)
	-0.1f,  0.1f, -0.1f,     0.0f, 1.0f, 0.0f, 
	 0.1f,  0.1f, -0.1f,     0.0f, 1.0f, 0.0f, 
	 0.1f,  0.1f,  0.1f,     0.0f, 1.0f, 0.0f, 
	 0.1f,  0.1f,  0.1f,     0.0f, 1.0f, 0.0f,
	-0.1f,  0.1f,  0.1f,     0.0f, 1.0f, 0.0f, 
	-0.1f,  0.1f, -0.1f,     0.0f, 1.0f, 0.0f,

	// Bottom face (Y-)
	-0.1f, -0.1f, -0.1f,     1.0f, 1.0f, 0.0f,
	 0.1f, -0.1f, -0.1f,     1.0f, 1.0f, 0.0f,
	 0.1f, -0.1f,  0.1f,     1.0f, 1.0f, 0.0f,
	 0.1f, -0.1f,  0.1f,     1.0f, 1.0f, 0.0f, 
	-0.1f, -0.1f,  0.1f,     1.0f, 1.0f, 0.0f,
	-0.1f, -0.1f, -0.1f,     1.0f, 1.0f, 0.0f,

	// Right face (X+)
	 0.1f, -0.1f, -0.1f,     0.0f, 0.0f, 1.0f,
	 0.1f,  0.1f, -0.1f,     0.0f, 0.0f, 1.0f,  
	 0.1f,  0.1f,  0.1f,     0.0f, 0.0f, 1.0f, 
	 0.1f,  0.1f,  0.1f,     0.0f, 0.0f, 1.0f,  
	 0.1f, -0.1f,  0.1f,     0.0f, 0.0f, 1.0f,  
	 0.1f, -0.1f, -0.1f,     0.0f, 0.0f, 1.0f,  

	 // Left face (X-)
	 -0.1f, -0.1f, -0.1f,     1.0f, 1.0f, 1.0f, 
	 -0.1f,  0.1f, -0.1f,     1.0f, 1.0f, 1.0f,
	 -0.1f,  0.1f,  0.1f,     1.0f, 1.0f, 1.0f, 
	 -0.1f,  0.1f,  0.1f,     1.0f, 1.0f, 1.0f,
	 -0.1f, -0.1f,  0.1f,     1.0f, 1.0f, 1.0f, 
	 -0.1f, -0.1f, -0.1f,     1.0f, 1.0f, 1.0f  
};

int main(void)
{
	if (!glfwInit())
	{
		std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window;
	unsigned int wWidth = 900;
	unsigned int wHeight = 900;
	const char wTitle[] = "Rubik's Cube";
	window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Prozor nije napravljen! :(\n";
		glfwTerminate();
		return 2;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW nije mogao da se ucita! :'(\n";
		return 3;
	}

	glfwSetKeyCallback(window, importantCallbacks);


	TextRender textRender("font/consolab.ttf", "text.vert", "text.frag", 29);
	TextRender timeTextRender("font/digital-7.ttf", "text.vert", "text.frag", 150);

	unsigned int basicShader = createShader("basic.vert", "basic.frag"); 

	initCube();

	glm::mat4 projection = glm::perspective(glm::radians(35.0f), 1.0f, 0.1f, 100.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

	glUseProgram(basicShader);
	unsigned int viewLoc = glGetUniformLocation(basicShader, "view");
	unsigned int projLoc = glGetUniformLocation(basicShader, "projection");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glClearColor(0.0f, 0.25f, 0.25f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	scramble = generateScramble();
	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		handleInput(window);
		updateRotation();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(basicShader);
		drawCube(basicShader);

		processNextMove();
		//Text part
		textRender.RenderText("Sremac Mihajlo RA 138/2021", 30.0f, 30.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		timer.Update();
		glm::vec3 color = timer.GetTextColor();
		std::string timerText = std::to_string(timer.GetElapsedTime()).substr(0, 4);
		timeTextRender.RenderText(printTime(timer.GetElapsedTime()), 325.0f, 600.0f, 0.6f, color);
		textRender.RenderText(scramble, 100.0f, 750.0f, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(basicShader);
	glfwTerminate();
	return 0;
}

void updateRotation() {
	cubeState.model = glm::mat4(1.0f);
	cubeState.model = glm::translate(cubeState.model, glm::vec3(0.0f, -0.4f, 0.0f));
	cubeState.model = glm::rotate(cubeState.model, glm::radians(cubeState.rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	cubeState.model = glm::rotate(cubeState.model, glm::radians(cubeState.rotationY), glm::vec3(0.0f, 1.0f, 0.0f));

	if (cubeState.isRotating) {
		cubeState.rotationCounter += 5.0f;
		if (cubeState.rotationCounter >= 90.0f) {
			cubeState.isRotating = false;
			cubeState.rotationCounter = 0.0f;
			updateCubeState(cubeState.currentLayer, cubeState.clockwise);
		}
	}
}

void updateCubeState(int face, bool clockwise) {
	std::vector<Cubie> newCubies = cubeState.cubies;

	if (face == 0) {
		for (int i = 0; i < cubeState.cubies.size(); i++) {
			if (cubeState.cubies[i].position.x == 1.0f) {
				glm::vec3 oldPos = cubeState.cubies[i].position;
				glm::vec3 newPos;

				if (clockwise) {
					newPos.x = oldPos.x;
					newPos.y = oldPos.z;
					newPos.z = -oldPos.y;
				}
				else {
					newPos.x = oldPos.x;
					newPos.y = -oldPos.z;
					newPos.z = oldPos.y;
				}

				for (int j = 0; j < cubeState.cubies.size(); j++) {
					if (glm::length(cubeState.cubies[j].position - newPos) < 0.1f) {
						newCubies[j].colors = cubeState.cubies[i].colors;

						/* cubie.colors[0] = GREEN;
						 cubie.colors[1] = BLUE;
						 cubie.colors[2] = WHITE;
						 cubie.colors[3] = YELLOW;
						 cubie.colors[4] = RED;
						 cubie.colors[5] = ORANGE;*/
						std::array<glm::vec4, 6> tempColors = newCubies[j].colors;
						if (clockwise) {
							tempColors[0] = newCubies[j].colors[3];
							tempColors[1] = newCubies[j].colors[2];
							tempColors[2] = newCubies[j].colors[0];
							tempColors[3] = newCubies[j].colors[1];
							tempColors[4] = newCubies[j].colors[4];
							tempColors[5] = newCubies[j].colors[5];
						}
						else {
							tempColors[0] = newCubies[j].colors[2];
							tempColors[1] = newCubies[j].colors[3];
							tempColors[2] = newCubies[j].colors[1];
							tempColors[3] = newCubies[j].colors[0];
							tempColors[4] = newCubies[j].colors[4];
							tempColors[5] = newCubies[j].colors[5];
						}
						newCubies[j].colors = tempColors;
						break;

					}
				}
			}
		}
	}
	else if (face == 1) {
		for (int i = 0; i < cubeState.cubies.size(); i++) {
			if (cubeState.cubies[i].position.x == -1.0f) {
				glm::vec3 oldPos = cubeState.cubies[i].position;
				glm::vec3 newPos;

				if (clockwise) {
					newPos.x = oldPos.x;
					newPos.y = -oldPos.z;
					newPos.z = oldPos.y;
				}
				else {
					newPos.x = oldPos.x;
					newPos.y = oldPos.z;
					newPos.z = -oldPos.y;
				}

				for (int j = 0; j < cubeState.cubies.size(); j++) {
					if (glm::length(cubeState.cubies[j].position - newPos) < 0.1f) {
						newCubies[j].colors = cubeState.cubies[i].colors;

						std::array<glm::vec4, 6> tempColors = newCubies[j].colors;
						if (clockwise) {
							tempColors[0] = newCubies[j].colors[2];
							tempColors[1] = newCubies[j].colors[3];
							tempColors[2] = newCubies[j].colors[1]; 
							tempColors[3] = newCubies[j].colors[0];
							tempColors[4] = newCubies[j].colors[4];
							tempColors[5] = newCubies[j].colors[5];
						}
						else {
							tempColors[0] = newCubies[j].colors[3];
							tempColors[1] = newCubies[j].colors[2];
							tempColors[2] = newCubies[j].colors[0];
							tempColors[3] = newCubies[j].colors[1];
							tempColors[4] = newCubies[j].colors[4];
							tempColors[5] = newCubies[j].colors[5];
						}
						newCubies[j].colors = tempColors;
						break;
					}
				}
			}
		}
	}
	else if (face == 2) {
		for (int i = 0; i < cubeState.cubies.size(); i++) {
			if (cubeState.cubies[i].position.y == 1.0f) {
				glm::vec3 oldPos = cubeState.cubies[i].position;
				glm::vec3 newPos;

				if (clockwise) {
					newPos.x = -oldPos.z;
					newPos.y = oldPos.y;
					newPos.z = oldPos.x;
				}
				else {
					newPos.x = oldPos.z;
					newPos.y = oldPos.y;
					newPos.z = -oldPos.x;
				}

				for (int j = 0; j < cubeState.cubies.size(); j++) {
					if (glm::length(cubeState.cubies[j].position - newPos) < 0.1f) {
						newCubies[j].colors = cubeState.cubies[i].colors;

						std::array<glm::vec4, 6> tempColors = newCubies[j].colors;
						if (clockwise) {
							tempColors[0] = newCubies[j].colors[4];
							tempColors[1] = newCubies[j].colors[5];
							tempColors[2] = newCubies[j].colors[2];
							tempColors[3] = newCubies[j].colors[3];
							tempColors[4] = newCubies[j].colors[1];
							tempColors[5] = newCubies[j].colors[0];
						}
						else {
							tempColors[0] = newCubies[j].colors[5];
							tempColors[1] = newCubies[j].colors[4];
							tempColors[2] = newCubies[j].colors[2];
							tempColors[3] = newCubies[j].colors[3];
							tempColors[4] = newCubies[j].colors[0];
							tempColors[5] = newCubies[j].colors[1];
						}
						newCubies[j].colors = tempColors;
						break;
					}
				}
			}
		}
	}
	else if (face == 3) {
		for (int i = 0; i < cubeState.cubies.size(); i++) {
			if (cubeState.cubies[i].position.y == -1.0f) {
				glm::vec3 oldPos = cubeState.cubies[i].position;
				glm::vec3 newPos;

				if (clockwise) {
					newPos.x = oldPos.z;
					newPos.y = oldPos.y;
					newPos.z = -oldPos.x;
				}
				else {
					newPos.x = -oldPos.z;
					newPos.y = oldPos.y;
					newPos.z = oldPos.x;
				}

				for (int j = 0; j < cubeState.cubies.size(); j++) {
					if (glm::length(cubeState.cubies[j].position - newPos) < 0.1f) {
						newCubies[j].colors = cubeState.cubies[i].colors;

						std::array<glm::vec4, 6> tempColors = newCubies[j].colors;
						if (clockwise) {
							tempColors[0] = newCubies[j].colors[5];
							tempColors[1] = newCubies[j].colors[4];
							tempColors[2] = newCubies[j].colors[2];
							tempColors[3] = newCubies[j].colors[3];
							tempColors[4] = newCubies[j].colors[0];
							tempColors[5] = newCubies[j].colors[1];
						}
						else {
							tempColors[0] = newCubies[j].colors[4];
							tempColors[1] = newCubies[j].colors[5]; 
							tempColors[2] = newCubies[j].colors[2]; 
							tempColors[3] = newCubies[j].colors[3]; 
							tempColors[4] = newCubies[j].colors[1]; 
							tempColors[5] = newCubies[j].colors[0];
						}
						newCubies[j].colors = tempColors;
						break;
					}
				}
			}
		}
	}
	else if (face == 4) {
		for (int i = 0; i < cubeState.cubies.size(); i++) {
			if (cubeState.cubies[i].position.z == 1.0f) {
				glm::vec3 oldPos = cubeState.cubies[i].position;
				glm::vec3 newPos;

				if (clockwise) {
					
					newPos.x = oldPos.y;
					newPos.y = -oldPos.x;
					newPos.z = oldPos.z;
				}
				else {
					newPos.x = -oldPos.y;
					newPos.y = oldPos.x;
					newPos.z = oldPos.z;
				}

				for (int j = 0; j < cubeState.cubies.size(); j++) {
					if (glm::length(cubeState.cubies[j].position - newPos) < 0.1f) {
						newCubies[j].colors = cubeState.cubies[i].colors;

						std::array<glm::vec4, 6> tempColors = newCubies[j].colors;
						if (clockwise) {
							tempColors[0] = newCubies[j].colors[0]; 
							tempColors[1] = newCubies[j].colors[1];
							tempColors[2] = newCubies[j].colors[5]; 
							tempColors[3] = newCubies[j].colors[4];
							tempColors[4] = newCubies[j].colors[2];  
							tempColors[5] = newCubies[j].colors[3]; 			
						}
						else {
							tempColors[0] = newCubies[j].colors[0]; 
							tempColors[1] = newCubies[j].colors[1]; 
							tempColors[2] = newCubies[j].colors[4]; 
							tempColors[3] = newCubies[j].colors[5]; 
							tempColors[4] = newCubies[j].colors[3]; 
							tempColors[5] = newCubies[j].colors[2];  
						}
						newCubies[j].colors = tempColors;
						break;
					}
				}
			}
		}
	}
	else if (face == 5) {
		for (int i = 0; i < cubeState.cubies.size(); i++) {
			if (cubeState.cubies[i].position.z == -1.0f) {
				glm::vec3 oldPos = cubeState.cubies[i].position;
				glm::vec3 newPos;

				if (clockwise) {
					newPos.x = -oldPos.y;
					newPos.y = oldPos.x;
					newPos.z = oldPos.z;
				}
				else {
					newPos.x = oldPos.y;
					newPos.y = -oldPos.x;
					
					newPos.z = oldPos.z;
				}

				for (int j = 0; j < cubeState.cubies.size(); j++) {
					if (glm::length(cubeState.cubies[j].position - newPos) < 0.1f) {
						newCubies[j].colors = cubeState.cubies[i].colors;

						std::array<glm::vec4, 6> tempColors = newCubies[j].colors;
						if (clockwise) {	
							tempColors[0] = newCubies[j].colors[0];
							tempColors[1] = newCubies[j].colors[1];
							tempColors[2] = newCubies[j].colors[4];
							tempColors[3] = newCubies[j].colors[5];
							tempColors[4] = newCubies[j].colors[3];
							tempColors[5] = newCubies[j].colors[2];
						}
						else {
							tempColors[0] = newCubies[j].colors[0];
							tempColors[1] = newCubies[j].colors[1];
							tempColors[2] = newCubies[j].colors[5];
							tempColors[3] = newCubies[j].colors[4];
							tempColors[4] = newCubies[j].colors[2];
							tempColors[5] = newCubies[j].colors[3];
						}
						newCubies[j].colors = tempColors;
						break;
					}
				}
			}
		}
	}

	cubeState.cubies = newCubies;
}

void drawCube(unsigned int shaderProgram) {
	
	glBindVertexArray(VAOcube);

	for (const auto& cubie : cubeState.cubies) {
		glm::mat4 model = cubeState.model;

		float scaleX = 0.21f; // Scale factor for X-axis
		float scaleY = 0.21f; // Scale factor for Y-axis
		float scaleZ = 0.21f; // Scale factor for Z-axis

		model = glm::translate(model, glm::vec3(cubie.position.x * scaleX, cubie.position.y * scaleY, cubie.position.z * scaleZ));


		if (cubeState.isRotating) {
			if (cubeState.currentLayer == 0 && cubie.position.x == 1.0f) { //&& std::abs(cubie.position.x - 1.0f) < 0.1f) {
				model = glm::translate(model, -cubie.position * 0.22f);
				float angle = !cubeState.clockwise ? cubeState.rotationCounter : -cubeState.rotationCounter;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::translate(model, cubie.position * 0.22f);
			}
			if (cubeState.currentLayer == 1 && cubie.position.x == -1.0f) { //&& std::abs(cubie.position.x - 1.0f) < -1.9f) {
				model = glm::translate(model, -cubie.position * 0.22f);
				float angle = cubeState.clockwise ? cubeState.rotationCounter : -cubeState.rotationCounter;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::translate(model, cubie.position * 0.22f);
			}
			if (cubeState.currentLayer == 2 && cubie.position.y == 1.0f) {
				model = glm::translate(model, -cubie.position * 0.22f);
				float angle = !cubeState.clockwise ? cubeState.rotationCounter : -cubeState.rotationCounter;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::translate(model, cubie.position * 0.22f);
			}
			if (cubeState.currentLayer == 3 && cubie.position.y == -1.0f) {
				model = glm::translate(model, -cubie.position * 0.22f);
				float angle = cubeState.clockwise ? cubeState.rotationCounter : -cubeState.rotationCounter;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::translate(model, cubie.position * 0.22f);
			}
			if (cubeState.currentLayer == 4 && cubie.position.z == 1.0f) {
				model = glm::translate(model, -cubie.position * 0.22f);
				float angle = !cubeState.clockwise ? cubeState.rotationCounter : -cubeState.rotationCounter;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::translate(model, cubie.position * 0.22f);
			}
			if (cubeState.currentLayer == 5 && cubie.position.z == -1.0f) {
				model = glm::translate(model, -cubie.position * 0.22f);
				float angle = cubeState.clockwise ? cubeState.rotationCounter : -cubeState.rotationCounter;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
				model = glm::translate(model, cubie.position * 0.22f);
			}
		}
		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// Set color uniform
		unsigned int colorLoc = glGetUniformLocation(shaderProgram, "cubeColor");

		// Draw each face with its corresponding color
		for (int face = 0; face < 6; face++) {
			glUniform4fv(colorLoc, 1, glm::value_ptr(cubie.colors[face]));
			glDrawArrays(GL_TRIANGLES, face * 6, 6);
		}

	}
		glUseProgram(0);
}

void rotateFace(int face, bool clockwise) {
	if (cubeState.isRotating)
		return;

	cubeState.isRotating = true;
	cubeState.currentLayer = face;
	cubeState.clockwise = clockwise;
	cubeState.rotationCounter = 0.0f;
}

void handleInput(GLFWwindow* window) {
	// Handle cube rotation with arrow keys
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cubeState.rotationX -= 1.0f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cubeState.rotationX += 1.0f;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		cubeState.rotationY -= 1.0f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		cubeState.rotationY += 1.0f;

	// Handle face rotations
	//true - clockwise, false - counterclockwise
	if (!cubeState.isRotating) {
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
			rotateFace(0, true);
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
			rotateFace(0, false);
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
			rotateFace(1, true);
		if (glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_PRESS)
			rotateFace(1, false);
		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
			rotateFace(2, true);
		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
			rotateFace(2, false);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			rotateFace(3, true);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			rotateFace(3, false);
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
			rotateFace(4, true);
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
			rotateFace(4, false);
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
			rotateFace(5, true);
		if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
			rotateFace(5, false);
	}
}

void importantCallbacks(GLFWwindow* window, int key, int scancode, int action, int mods) {
	timer.KeyCallback(key, action);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		scramble = generateScramble();
	}
	if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
		resetCube();
	}
}

string printTime(double elapsedTime) {
	// Calculate minutes, seconds, and milliseconds
	int minutes = static_cast<int>(elapsedTime) / 60;
	int seconds = static_cast<int>(elapsedTime) % 60;
	int milliseconds = static_cast<int>((elapsedTime - static_cast<int>(elapsedTime)) * 100);

	// Format the output string
	std::ostringstream timeStream;
	if (minutes > 0) {
		// If minutes > 0, include minutes in the format
		timeStream << minutes << ":"
			<< std::setfill('0') << std::setw(2) << seconds << ".";
	}
	else {
		// If minutes == 0, exclude the "0:" prefix
		timeStream << seconds << ".";
	}
	timeStream << std::setfill('0') << std::setw(2) << milliseconds;

	return timeStream.str();
}

string generateScramble() {
	vector<string> faces = { "U", "D", "L", "R", "F", "B" };
	vector<string> modifiers = { "", "'", "2" };
	string scramble;
	string lastFace = "";

	resetCube();

	int moves = 25;

	srand(time(0));

	for (int i = 0; i < moves; ++i) {
		string face;

		do {
			face = faces[rand() % faces.size()];
		} while (face == lastFace);

		string modifier = modifiers[rand() % modifiers.size()];
		scramble += face + modifier + " ";
		lastFace = face;
	}

	string move;
	for (int i = 0; i < scramble.size(); ++i) {
		if (scramble[i] == ' ') {
			scrambleQueue.push(move); // Add the move to the queue
			move.clear();
		}
		else {
			move += scramble[i];
		}
	}

	return scramble;
}

void processNextMove() {
	if (!cubeState.isRotating && !scrambleQueue.empty()) {
		string move = scrambleQueue.front();
		scrambleQueue.pop();

		// Execute the move
		if (move == "R") {
			rotateFace(0, true);
		}
		else if (move == "R'") {
			rotateFace(0, false);
		}
		else if (move == "R2") {
			rotateFace(0, true);
			rotateFace(0, true);
		}
		else if (move == "L") {
			rotateFace(1, true);
		}
		else if (move == "L'") {
			rotateFace(1, false);
		}
		else if (move == "L2") {
			rotateFace(1, true);
			rotateFace(1, true);
		}
		else if (move == "U") {
			rotateFace(2, true);
		}
		else if (move == "U'") {
			rotateFace(2, false);
		}
		else if (move == "U2") {
			rotateFace(2, true);
			rotateFace(2, true);
		}
		else if (move == "D") {
			rotateFace(3, true);
		}
		else if (move == "D'") {
			rotateFace(3, false);
		}
		else if (move == "D2") {
			rotateFace(3, true);
			rotateFace(3, true);
		}
		else if (move == "F") {
			rotateFace(4, true);
		}
		else if (move == "F'") {
			rotateFace(4, false);
		}
		else if (move == "F2") {
			rotateFace(4, true);
			rotateFace(4, true);
		}
		else if (move == "B") {
			rotateFace(5, true);
		}
		else if (move == "B'") {
			rotateFace(5, false);
		}
		else if (move == "B2") {
			rotateFace(5, true);
			rotateFace(5, true);
		}
	}
}

void resetCube() {

	if (cubeState.isRotating || !scrambleQueue.empty())
		return;

	for (auto& cubie : cubeState.cubies) {
		cubie.colors[0] = GREEN;
		cubie.colors[1] = BLUE;
		cubie.colors[2] = WHITE;
		cubie.colors[3] = YELLOW;
		cubie.colors[4] = RED;
		cubie.colors[5] = ORANGE;
	}
}

void initCube() {
	cubeState.cubies.clear();
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			for (int z = -1; z <= 1; z++) {
				Cubie cubie;
				cubie.position = glm::vec3(x, y, z);

				cubie.colors[0] = GREEN;
				cubie.colors[1] = BLUE;
				cubie.colors[2] = WHITE;
				cubie.colors[3] = YELLOW;
				cubie.colors[4] = RED;
				cubie.colors[5] = ORANGE;

				cubeState.cubies.push_back(cubie);
			}
		}
	}

	glGenVertexArrays(1, &VAOcube);
	glGenBuffers(1, &VBOcube);

	glBindVertexArray(VAOcube);
	glBindBuffer(GL_ARRAY_BUFFER, VBOcube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubieVertices), cubieVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glEnable(GL_DEPTH_TEST);
}

unsigned int compileShader(GLenum type, const char* source)
{
	//Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
	//Citanje izvornog koda iz fajla
	std::string content = "";
	std::ifstream file(source);
	std::stringstream ss;
	if (file.is_open())
	{
		ss << file.rdbuf();
		file.close();
		std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
	}
	else {
		ss << "";
		std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
	}
	std::string temp = ss.str();
	const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

	int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)

	int success; //Da li je kompajliranje bilo uspjesno (1 - da)
	char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
	glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
	glCompileShader(shader); //Kompajliraj sejder

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
	if (success == GL_FALSE)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
		if (type == GL_VERTEX_SHADER)
			printf("VERTEX");
		else if (type == GL_FRAGMENT_SHADER)
			printf("FRAGMENT");
		printf(" sejder ima gresku! Greska: \n");
		printf(infoLog);
	}
	return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
	//Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

	unsigned int program; //Objedinjeni sejder
	unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
	unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

	program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

	vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
	fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

	//Zakaci verteks i fragment sejdere za objedinjeni program
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
	glValidateProgram(program); //Izvrsi provjeru novopecenog programa

	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
	if (success == GL_FALSE)
	{
		glGetShaderInfoLog(program, 512, NULL, infoLog);
		std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
		std::cout << infoLog << std::endl;
	}

	//Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
	glDetachShader(program, vertexShader);
	glDeleteShader(vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(fragmentShader);

	return program;
}
