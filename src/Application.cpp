/*
 *  Muser 0.1.0
 *
 *  Allows the user to create 3D geometry which then is turned into sound.
 *
 *  Creator: Anthony Mesa
 *  Date: 09/25/19
 *
 */

#define GLEW_STATIC
#define STB_IMAGE_IMPLEMENTATION

#ifdef __APPLE__
	#include <OpenGL/GL.h>

#elif defined _WIN32 || defined _WIN64
	#include <windows.h>
	#include <GL/glew.h>
	#include <GL/gl.h>
	#include <GL/glu.h>

#else
#endif

#include <stb_image/stb_image.h>
#include <GLFW/glfw3.h>
#include <math_3d/math_3d.h>

#include <RenderMuser.h>
#include <Muse.h>
#include <Application.h>
#include <bi_tools.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <filesystem>

screenDimensions screen_dimensions;
GLuint VBO;

GLuint loadSplashImage()
{
	int textureWidth, textureHeight, bpp;
	GLuint id;
	
	unsigned char* image = stbi_load("media/muserbanner.jpg", &textureWidth, &textureHeight, &bpp, STBI_rgb_alpha);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	if (bpp == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	else if (bpp == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_image_free(image);

	return id;
}

GLFWwindow* showSplash()
{
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	GLFWwindow* splashWindow = glfwCreateWindow(1000, 341, "Muser loading...", NULL, NULL);
	glfwMakeContextCurrent(splashWindow);
	
	// If Windows, initialize GLEW.
	#ifdef _WIN32
		
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
		fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	#endif

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint VBO;

	GLuint texId = loadSplashImage();
	glBindTexture(GL_TEXTURE_2D, texId);

	Vector3f Vertices[4];
	Vertices[0] = Vector3f(-1.0f, 1.0f, 0.0f);
	Vertices[1] = Vector3f(1.0f, 1.0f, 0.0f);
	Vertices[2] = Vector3f(1.0f, -1.0f, 0.0f);
	Vertices[3] = Vector3f(-1.0f, -1.0f, 0.0f);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_QUADS, 0, 4);

	glfwSwapBuffers(splashWindow);

	int showSplash_window_width, showSplash_window_height;
	glfwGetFramebufferSize(splashWindow, &showSplash_window_width, &showSplash_window_height);
	glfwSetWindowPos(splashWindow, (screen_dimensions.session_screen_width / 2) - (showSplash_window_width / 2), (screen_dimensions.session_screen_height / 2) - (showSplash_window_height / 2));

	return splashWindow;
}

/*
 	Main function
 	Initializes GLFW and runs splash window, then runs main window.
 */
int main(void)
{
	if (!glfwInit())
		return -1;

	// get current glfw monitor and set screen_width and screen_height accordingly
	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	glfwGetMonitorWorkarea(primary, NULL, NULL, &screen_dimensions.session_screen_width, &screen_dimensions.session_screen_height);

	// Initialize and display a new glfw window ShowSplash
	GLFWwindow* splashWindow = showSplash();
	
	biPause();

	std::string spectrogramName = "spectrogram_array";
	Muse firstObject;
	firstObject.renderToSpectrogram(spectrogramName);

	// close and destroy the splash window
	glfwSetWindowShouldClose(splashWindow, GLFW_TRUE);
	glfwDestroyWindow(splashWindow);

	// create application main window
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
	GLFWwindow* mainWindow = glfwCreateWindow(1080, 720, "Muser", NULL, NULL);

	if (!mainWindow)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(mainWindow);

	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();

		renderMuser();

		glfwSwapBuffers(mainWindow);
	}

	glfwTerminate();
	return 0;
}
