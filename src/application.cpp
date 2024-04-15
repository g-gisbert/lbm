#include "application.h"

#include <fstream>

Application::Application(const std::string& windowName, int width, int height) :
    m_windowWidth(width), m_windowHeight(height), m_frameBuffer(new unsigned char[width*height*3]) {

    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    createWindow(windowName, width*3, height*3);

    if (glewInit() != GLEW_OK)
        throw std::runtime_error("Failed to initialize GLEW");

    std::string vertexShaderSource = readShaderFromFile("../src/shaders/twoTriangles.vs");
    std::string fragmentShaderSource = readShaderFromFile("../src/shaders/twoTriangles.fs");

    m_vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    m_fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());
    m_shaderProgram = linkShaderProgram(m_vertexShader, m_fragmentShader);

    memset(m_frameBuffer, 0, width*height*3 * sizeof(unsigned char));
    initializeTexture();

    createBuffers();

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    glUseProgram(m_shaderProgram);
    m_sim = new LBM(width, height);
}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(m_shaderProgram);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);

    delete[] m_frameBuffer;

    glDeleteTextures(1, &m_texture);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteVertexArrays(1, &m_VAO);

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Application::run() {
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(m_window)) {
        glBindTexture(GL_TEXTURE_2D, m_texture);
        /*cpt++;
        glBindTexture(GL_TEXTURE_2D, m_texture);
        for (int i = 0; i < m_windowWidth * m_windowHeight * 3; ++i) {
            m_frameBuffer[i] = (i/m_windowWidth + (cpt%256)*i + i + cpt) % 256;
        }*/
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        start = std::chrono::high_resolution_clock::now();
        m_sim->step(elapsed_seconds.count(), m_frameBuffer);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_windowWidth, m_windowHeight, GL_RGB, GL_UNSIGNED_BYTE, m_frameBuffer);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //renderImGui();
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

inline void Application::renderImGui() const {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always, ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);
    ImGui::Begin("Simulation parameters", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::Text("Hello, world!");
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::createWindow(const std::string& name, int w, int h) {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(w, h, name.c_str(), NULL, NULL);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(m_window);
}

std::string Application::readShaderFromFile(const std::string& filename) const {
    std::ifstream fileStream(filename);
    if (!fileStream.is_open())
        throw std::runtime_error("Failed to open file: " + filename);

    fileStream.seekg(0, std::ios::end);
    std::streamsize fileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);

    std::string shaderSource;
    shaderSource.resize(fileSize);
    fileStream.read(&shaderSource[0], fileSize);
    fileStream.close();

    return shaderSource;
}

GLuint Application::compileShader(GLenum shaderType, const char* shaderSource) const {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    return shader;
}

GLuint Application::linkShaderProgram(GLuint vertexShader, GLuint fragmentShader) const {
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    return shaderProgram;
}

void Application::initializeTexture() {
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_windowWidth, m_windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, m_frameBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Application::createBuffers() {
    float vertices[] = {
            1.0f,  1.0f, 0.0f,      1.0f, 1.0f,
            1.0f, -1.0f, 0.0f,      1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,      0.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,      0.0f, 1.0f
    };
    GLuint indices[] = {0, 1, 3, 1, 2, 3};

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}
