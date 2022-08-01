#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>a
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float deltaTime = 0.0f, lastTime = 0.0f;

const char* vertexSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * model * vec4(position, 1.0f);
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

bool firstFrame = true;

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
    window = glfwCreateWindow(800, 600, "RayCasting", nullptr, nullptr);
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
    glEnable(GL_DEPTH_TEST);

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

    // setting up a triangle
    float vertices[] = {
            //front face
            0.5f, 0.5f, 0.5f,// top right
            0.5f, -0.5f, 0.5f, // bottom right
            -0.5f, -0.5f, 0.5f, // bottom left
            -0.5, 0.5, 0.5f, // top left
            //back face
            0.5f, 0.5f, -0.5f,// top right
            0.5f, -0.5f, -0.5f, // bottom right
            -0.5f, -0.5f, -0.5f, // bottom left
            -0.5, 0.5, -0.5f, // top left

    };
    GLuint indices[] = {
            // front face
            0, 1, // first line
            1, 2, // second line
            2, 3, // third line
            3, 0, //fourth line
            // back face
            4, 5,
            5, 6,
            6, 7,
            7, 4,
            // connecting faces
            0, 4,
            1, 5,
            2, 6,
            3, 7

    };

    // sending data to GPU
    // GLuint is cross-platform for unsigned int
    // create vertex buffer object and Vertex Array Object
    GLuint vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    // upload data to GPU
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // bind data to buffer and therefore GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // get reference to the position in vShader
    GLint posAttribute = glGetAttribLocation(shaderProgram, "position");

    // tell shader that position takes two float values starting at position 0 with 0 bytes between them
    // [0, 1, 2, 3, 4, 5] => [[0, 1] [2, 3] [4, 5]] kinda
    // GL_FALSE just means that the shaders do not need to be normalized to between -1.0 and 1.0
    glVertexAttribPointer(posAttribute, 3, GL_FLOAT, GL_FALSE, 0,0);
    // enable the position attribute
    glEnableVertexAttribArray(posAttribute);


    // projection matrix is the view type (perspective/orthographic) and the settings for that
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
    // sends projection data to shader
    int projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // GAME LOOP TIME!!
    while (!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // getting delta time so movement isn't dependent on frame rate
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // putting things on the screen
        // drawing triangles
        // first specifies primitive (triangles, lines) second is were to start in the vertex array and third is the number of vertices to process
        // glDrawArrays(GL_LINES, 0, 3);
        glBindVertexArray(vao);

        // view matrix is where the camera will be positioned
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        // sends view data to vertex shader
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


        for (GLuint i = 0; i < 36; i ++){
            // setting Shader Color
            // get ref to triangle color var in frag shader
            GLint outputColor = glGetUniformLocation(shaderProgram, "triangleColor");
            // set outputColor to values
            glUniform3f(outputColor, sin(i*M_PI/18.0f)/2 + 0.5, cos(i * M_PI/18.0f)/2 + 0.5, 0.9f);
            if (firstFrame){
                std::cout << sin(i/18.0f * M_PI) << std::endl;
            }
            // going 3-D
            // model matrix is the transforms applied to all objects vertices to convert from local to world space
            // initialized to the 4x4 identity matrix
            glm::mat4 model = glm::mat4(1.0f);
            // rotate models 55 degrees along the x-axis
            model = glm::rotate(model, (float)glfwGetTime() + glm::radians(i * 10.0f), glm::vec3(0.5f, 1.0f, 0.5f));
//            model = glm::translate(model, glm::vec3(0.0f, 0.0f, i * 1.0f));

            // send models transformation data to the vertex shader
            int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
        }
        firstFrame = false;
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
