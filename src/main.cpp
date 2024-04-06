#include <iostream>
#include "GL/glew.h"
#include <GLFW/glfw3.h>


#define CHECK_GL_ERROR() \
do { \
    GLenum error = glGetError(); \
    if (error != GL_NO_ERROR) { \
        std::cerr << "OpenGL error: " << gluErrorString(error) << std::endl; \
    } \
} while (false)


#define CHECK_PROGRAM_ERROR(program) \
do { \
    GLint success; \
    glGetProgramiv(program, GL_LINK_STATUS, &success); \
    if (!success) { \
        char infoLog[512]; \
        glGetProgramInfoLog(program, 512, NULL, infoLog); \
        std::cerr << "Program linking failed:\n" << infoLog << std::endl; \
    } \
} while (false)

#define CHECK_SHADER_ERROR(shader) \
do { \
    GLint success; \
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); \
    if (!success) { \
        char infoLog[512]; \
        glGetShaderInfoLog(shader, 512, NULL, infoLog); \
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl; \
    } \
} while (false)


// Shader sources
const char* vertexShaderSource = R"(
    #version 300 es
    precision mediump float;
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    layout (location = 2) in vec2 aTexCoord;

    out vec3 ourColor;
    out vec2 TexCoord;

    void main()
    {
        gl_Position = vec4(aPos, 1.0);
        ourColor = aColor;
        TexCoord = aTexCoord;
    }
)";

const char* fragmentShaderSource = R"(
    #version 300 es
    precision mediump float;
    out vec4 FragColor;

    in vec3 ourColor;
    in vec2 TexCoord;

    uniform sampler2D ourTexture;

    void main()
    {
        FragColor = texture(ourTexture, TexCoord);
    }
)";

// Image dimensions
const int width = 640;
const int height = 480;

// Image data (example buffer)
unsigned char imageData[width * height * 3]; // RGB image


int main() {

    for (int i = 0; i < width * height * 3; ++i) {
        imageData[i] = (3*i + i) % 256;
    }


    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(width, height, "Image Rendering", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    CHECK_SHADER_ERROR(vertexShader);

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    CHECK_SHADER_ERROR(fragmentShader);

    // Shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    CHECK_PROGRAM_ERROR(shaderProgram);

    // Delete shaders as they're linked into our program now and no longer necessary
    //glDeleteShader(vertexShader); CHECK_GL_ERROR();
    //glDeleteShader(fragmentShader); CHECK_GL_ERROR();

    // Define the texture
    GLuint texture;
    glGenTextures(1, &texture); CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, texture); CHECK_GL_ERROR();
// set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); CHECK_GL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData); CHECK_GL_ERROR();
    glGenerateMipmap(GL_TEXTURE_2D); CHECK_GL_ERROR();


    // Specify the vertex attributes
    float vertices[] = {
            // positions          // colors           // texture coords
            1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
            1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
            -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    GLuint indices[] = {
            0, 1, 3,   // First triangle
            1, 2, 3    // Second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO); CHECK_GL_ERROR();
    glGenBuffers(1, &VBO); CHECK_GL_ERROR();
    glGenBuffers(1, &EBO); CHECK_GL_ERROR();

    glBindVertexArray(VAO); CHECK_GL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, VBO); CHECK_GL_ERROR();
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); CHECK_GL_ERROR();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); CHECK_GL_ERROR();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); CHECK_GL_ERROR();

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); CHECK_GL_ERROR();
    glEnableVertexAttribArray(0); CHECK_GL_ERROR();
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); CHECK_GL_ERROR();
    glEnableVertexAttribArray(1); CHECK_GL_ERROR();
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); CHECK_GL_ERROR();
    glEnableVertexAttribArray(2); CHECK_GL_ERROR();

    // Unbind VAO
    glBindVertexArray(0); CHECK_GL_ERROR();

    glUseProgram(shaderProgram);
    CHECK_PROGRAM_ERROR(shaderProgram);

    int cpt = 0;

    // Rendering loop
    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind Texture
        cpt++;
        glBindTexture(GL_TEXTURE_2D, texture);
        for (int i = 0; i < width * height * 3; ++i) {
            imageData[i] = (i/width + (cpt%256)*i + i + cpt) % 256;
        }
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D); CHECK_GL_ERROR();

        // render container
        // Use shader program
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // Terminate GLFW
    glfwTerminate();

    return 0;
}
