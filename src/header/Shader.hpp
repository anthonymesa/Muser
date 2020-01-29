/*
 *  Muser 0.1.0
 *  Creator: Anthony Mesa
 *  Date: 09/25/19
 *
 *  Shader Class
 * 
 *  Contains all shader code
 */

#ifndef SHADER_H
#define SHADER_H

#ifdef _WIN32 || _WIN64
	#include <glad/glad.h>
#endif

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

class Shader
{
public:

    GLuint ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    void Use();
    void SetBool(const std::string &name, bool value) const;  
    void SetInt(const std::string &name, int value) const;   
    void SetFloat(const std::string &name, float value) const;

private:

    void CheckCompileErrors(GLuint shader, std::string type);
};

Shader::Shader(const char* vertex_path, const char* fragment_path)
{
    std::ifstream vertex_shader_file;
    std::ifstream fragment_shader_file;
    std::stringstream vertex_buffer, fragment_buffer;
    std::string vertex_buffer_str, fragment_buffer_str;

    vertex_shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fragment_shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vertex_shader_file.open(vertex_path);
        fragment_shader_file.open(fragment_path);

        vertex_buffer << vertex_shader_file.rdbuf();
        fragment_buffer << fragment_shader_file.rdbuf();

        vertex_buffer_str = vertex_buffer.str();
        fragment_buffer_str = fragment_buffer.str();

        vertex_shader_file.close();
        fragment_shader_file.close();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::SHADER_FILES_NOT_SUCCESSFULLY_OPENED" << std::endl;
    }
    
    // Warning: safe only until vertexBufferStr is destroyed or modified
    const GLchar *vertex_source = vertex_buffer_str.c_str();
    const GLchar *fragment_source = fragment_buffer_str.c_str();

    GLuint vertex_shader, fragment_shader;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);
    glCompileShader(vertex_shader);
    CheckCompileErrors(vertex_shader, "VERTEX");
    glCompileShader(fragment_shader);
    CheckCompileErrors(fragment_shader, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertex_shader);
    glAttachShader(ID, fragment_shader);
    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::Use()
{
    glUseProgram(ID);
}

void Shader::SetBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}

void Shader::SetInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
}

void Shader::SetFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
}

void Shader::CheckCompileErrors(GLuint shader, std::string type)
{
    int  success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

#endif







