#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

float deltaTime = 0.0f, lastTime = 0.0f;

const char* vertexSource = R"glsl(
#version 330 core
layout (location = 0) in vec2 position;

void main(){
    gl_Position = vec4(position, 0.0f, 1.0f);
}
)glsl";

const char* fragmentSource = R"glsl(
#version 330 core

uniform vec3 triangleColor;

out vec4 outColor;

void main(){
    outColor = vec4(triangleColor, 1.0f);
}
)glsl";

void error_callback(int error, const char* description){
    fprintf(stderr, "ERROR: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0, width, height);
}

int main() {
    GLFWwindow* window;

    // check for GLFW starting error
    if (!glfwInit()){
        std::cout << "Could not start GLFW" << std::endl;
    }

    // set up how to display errors
    glfwSetErrorCallback(error_callback);

    // specifies that we want glfw3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Mac m1 specific stuff
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // creating the window
    window = glfwCreateWindow(800, 600, "RayCasting", NULL, NULL);
    if(!window){
        glfwTerminate();
        std::cout << "error creating window" << std::endl;
        return -1;
    }

    // Make the glfw context current REQUIRED for openGL API stuff
    glfwMakeContextCurrent(window);
    // lowers the frame rate
    glfwSwapInterval(1);

    // Load GLEW to get OpenGL stuff
    GLenum err = glewInit();
    // check if GLEW loaded properly
    if (GLEW_OK != err){
        fprintf(stderr, "ERROR: %s\n", glewGetErrorString(err));
    }

    // setting up callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback() will set at some point soon

    // setting up the window
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0,0,width, height);

    // setting up a triangle
    float vertices[] = {
            0.0f, 0.5f, // top corner
            0.5f, -0.5f, // bottom right
            -0.5f, -0.5f // bottom left
    };

    // sending data to GPU
    // GLuint is cross-platform for unsigned int
    // create vertex buffer object and Vertex Array Object
    GLuint vbo, vao;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    // upload data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
    // bind data to buffer and therefore GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

    // Shader Shit
    // create vertex Shader object
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // load data to Vertex Shader Object
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    // compile Shader
    glCompileShader(vertexShader);
    // check for shader comp errors
    char infoLog[512];
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: VERTEX" << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }

    // create Fragment Shader object
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // load data
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    // compile
    glCompileShader(fragmentShader);
    // check for comp errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: FRAGMENT" << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }

    // combine shaders into a program;
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // link program
    glLinkProgram(shaderProgram);
    // use newly made shader program
    glUseProgram(shaderProgram);

    // get reference to the position in vShader
    GLint posAttribute = glGetAttribLocation(shaderProgram, "position");

    // tell shader that position takes two float values starting at position 0 with 0 bytes between them
    // [0, 1, 2, 3, 4, 5] => [[0, 1] [2, 3] [4, 5]] kinda
    // GL_FALSE just means that the shaders do not need to be normalized to between -1.0 and 1.0
    glVertexAttribPointer(posAttribute, 2, GL_FLOAT, GL_FALSE, 0,0);
    // enable the position attribute
    glEnableVertexAttribArray(posAttribute);

    // setting Shader Color
    //get ref to triangle color var in frag shader
    GLint outputColor = glGetUniformLocation(shaderProgram, "triangleColor");
    //set outputColor to values
    glUniform3f(outputColor, 0.3f, 0.8f, 0.9f);

    // GAMELOOP TIME!!
    while (!glfwWindowShouldClose(window)){
        // getting delta time so movement isn't dependent on frame rate
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // drawing
        // drawing triangles
        // first specifies primitive (triangles, lines) second is were to start in the vertex array and third is the number of vertices to process
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
