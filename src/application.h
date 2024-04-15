#ifndef EFS_APPLICATION_H
#define EFS_APPLICATION_H

#include <iostream>
#include <string>
#include <chrono>

#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "simulation.h"
#include "LBM.h"

class Application {
public:
    Application(const std::string& windowName, int width, int height);
    ~Application();

    void run();

private:
    void renderImGui() const;

    void createWindow(const std::string& name, int w, int h);
    std::string readShaderFromFile(const std::string& filename) const;
    GLuint compileShader(GLenum shaderType, const char* shaderSource) const;
    GLuint linkShaderProgram(GLuint vertexShader, GLuint fragmentShader) const;
    void initializeTexture();
    void createBuffers();

    int m_windowWidth;
    int m_windowHeight;
    GLFWwindow* m_window;

    unsigned char* m_frameBuffer;
    GLuint m_texture;

    GLuint m_vertexShader;
    GLuint m_fragmentShader;
    GLuint m_shaderProgram;

    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;

    LBM* m_sim;
};


#endif //EFS_APPLICATION_H
