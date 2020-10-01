/*
 *  Muser 0.1.0
 *
 *  Allows the user to create 3D geometry which then is turned into sound.
 *
 *  Creator: Anthony Mesa
 *  Date: 09/25/19
 *
 */

#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
#elif defined _WIN32 || defined _WIN64
	#define NOMINMAX
	#include <windows.h>
	#include <glad/glad.h>
#else
#endif

#include <stb_image/stb_image.h>
#include <GLFW/glfw3.h>
#include <math_3d/math_3d.h>

#include <Muse.hpp>
#include <Main.hpp>
#include <bi_tools.hpp>
#include <Shader.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <cmath>
#include <vector>

screenDimensions screen_dimensions;

int main(void)
{
    //====================================================
    if (!glfwInit()){ return -1; }

    //Set screen dimensions
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
	glfwGetMonitorWorkarea(primary, NULL, NULL, &screen_dimensions.session_screen_width, &screen_dimensions.session_screen_height);
    
    //Set up GLFW window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	GLFWwindow* splashWindow = glfwCreateWindow(screen_dimensions.session_screen_width, screen_dimensions.session_screen_height, "Muser loading...", NULL, NULL);
    if (splashWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    //Center GLFW Window
    int showSplash_window_width, showSplash_window_height;
	glfwGetFramebufferSize(splashWindow, &showSplash_window_width, &showSplash_window_height);
	glfwSetWindowPos(splashWindow, (screen_dimensions.session_screen_width / 2) - (showSplash_window_width / 2), (screen_dimensions.session_screen_height / 2) - (showSplash_window_height / 2));
    
    glfwMakeContextCurrent(splashWindow);

    //Load GLAD
    #ifdef _WIN32 || _WIN64
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }
    #endif

    //Set OpengGl Viewport to GLFW window
    glViewport( 0, 0, screen_dimensions.session_screen_width, screen_dimensions.session_screen_height);
    
    //====================================================

    Muse new_object;

    //new_object.RenderToSpectrogram("fuck");
    //new_object.RenderToAudio("bitch");

    //Create Vertex Attribute Array

    GLuint VAO;
    glGenVertexArrays(1, &VAO);  
    glBindVertexArray(VAO);

    //Create buffers and write vertexes to buffers
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 294918*2*sizeof(float), new_object.GetVertexAttributes(), GL_STATIC_DRAW);

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 589824*sizeof(unsigned int), new_object.GetElements(), GL_STATIC_DRAW);

    //Create vertex attributes for vertex data at location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    Shader* default_shader = new Shader("data/shaders/vertex.shader", "data/shaders/fragment.shader");

    //OpenGl render loop
    while(!glfwWindowShouldClose(splashWindow))
    {
        ProcessInput(splashWindow);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS); 
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);


        default_shader->SetFloat("yPos", 1.5*glfwGetTime());
        default_shader->SetFloat("width", screen_dimensions.session_screen_width);
        default_shader->SetFloat("height", screen_dimensions.session_screen_height);


        default_shader->Use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 589824, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));

        glfwSwapBuffers(splashWindow);
        glfwPollEvents();
    }

    delete default_shader;
    glfwTerminate();
    return 0;
}

//====================================================
void ProcessInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

//====================================================
GLuint LoadSplashImage()
{
	// int textureWidth, textureHeight, bpp;
	GLuint id;
	
	// unsigned char* image = stbi_load("media/muserbanner.jpg", &textureWidth, &textureHeight, &bpp, 0);

	// glEnable(GL_TEXTURE_2D);
	// glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glGenTextures(1, &id);
	// glBindTexture(GL_TEXTURE_2D, id);

	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// if (bpp == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// else if (bpp == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	// glGenerateMipmap(GL_TEXTURE_2D);
	// stbi_image_free(image);
	return id;
}
