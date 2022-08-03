#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <glm/glm.hpp>
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

uniform vec4 triangleColor;

out vec4 outColor;

void main(){
    outColor = triangleColor;
}
)glsl";
const float WIDTH = 1600.0f, HEIGHT = 900.0f;
const int outputs = 2880;
float magnitudes[outputs][2];
float rayInfo[outputs][3];
float mouseX = 0.0f, mouseY = 0.0f;
float prevMouseX = WIDTH, prevMouseY = HEIGHT;

glm::vec3 startPt = glm::vec3(WIDTH/2.0f, HEIGHT/2.0f, 0.0f);

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

void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos){
//    if (firstFrame){
//        prevMouseX = xpos;
//        prevMouseY = ypos;
//    }
//    float xOffset = xpos - prevMouseX;
//    float yOffset = xpos - prevMouseY;
//    prevMouseX = xpos;
//    prevMouseY = ypos;
//
//    const float sens = 1 * deltaTime;
//    xOffset *= sens;
//    yOffset *= sens;
    mouseX = xpos;
    mouseY = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
//        float xOffset = mouseX - prevMouseX;
//        float yOffset = mouseY - prevMouseY;
        prevMouseX = mouseX;
        prevMouseY = mouseY;

        startPt.x = prevMouseX;
        startPt.y = HEIGHT - prevMouseY;

        for (unsigned int i = 0;  i < outputs; i ++){
            float angle = i * 360.0f/outputs;
            float magnitude1 = 0.0f;
            float magnitude2 = 0.0f;

            if (angle >= 0 && angle < 90){
                //quadrant one
                magnitude2 = abs((WIDTH - startPt.x) / cos(glm::radians(angle)));
                magnitude1 = abs((HEIGHT - startPt.y) / sin(glm::radians(angle)));
            }else if (angle >= 90 && angle < 180){
                magnitude2 = abs((startPt.x) / cos(glm::radians(angle)));
                magnitude1 = abs((HEIGHT - startPt.y) / sin(glm::radians(angle)));
            }else if (angle >= 180 && angle < 270) {
                magnitude2 = abs((startPt.x) / cos(glm::radians(angle)));
                magnitude1 = abs((startPt.y) / sin(glm::radians(angle)));
            }else {
                magnitude2 = abs((WIDTH - startPt.x) / cos(glm::radians(angle)));
                magnitude1 = abs((startPt.y) / sin(glm::radians(angle)));
            }
            if (magnitude1 > magnitude2){
                magnitudes[i][0] = magnitude2;
            }else{
                magnitudes[i][0] = magnitude1;
            }
            // angle mirrored along y axis
            float tempAngle = 0.0f;
            rayInfo[i][0] = 180 - angle;
            // coordinates for point of the collision
            rayInfo[i][1] = startPt.x - magnitudes[i][0] * cos(glm::radians(rayInfo[i][0]));
            rayInfo[i][2] = startPt.y + magnitudes[i][0] * sin(glm::radians(rayInfo[i][0]));
            if (magnitude1 < magnitude2) {
                rayInfo[i][0] -= 180;
            }

            //bounce 1
            if ((rayInfo[i][0] >= 0 && rayInfo[i][0] < 90) || (rayInfo[i][0] < -270 && rayInfo[i][0] >= -360)){
                //quadrant one
                magnitude2 = abs((WIDTH - rayInfo[i][1]) / cos(glm::radians(rayInfo[i][0])));
                magnitude1 = abs((HEIGHT - rayInfo[i][2]) / sin(glm::radians(rayInfo[i][0])));
            }else if ((rayInfo[i][0] >= 90 && rayInfo[i][0] < 180) || (rayInfo[i][0] < -180 && rayInfo[i][0] >= -270)){
                magnitude2 = abs((rayInfo[i][1]) / cos(glm::radians(rayInfo[i][0])));
                magnitude1 = abs((HEIGHT - rayInfo[i][2]) / sin(glm::radians(rayInfo[i][0])));
            }else if ((rayInfo[i][0] >= 180 && rayInfo[i][0] < 270) || rayInfo[i][0] < -90 && rayInfo[i][0] >= -180) {
                magnitude2 = abs((rayInfo[i][1]) / cos(glm::radians(rayInfo[i][0])));
                magnitude1 = abs((rayInfo[i][2]) / sin(glm::radians(rayInfo[i][0])));
            }else if ((rayInfo[i][0] >= 270 && rayInfo[i][0] < 360) || (rayInfo[i][0] < 0 && rayInfo[i][0] >= -90)){
                magnitude2 = abs((WIDTH - rayInfo[i][1]) / cos(glm::radians(rayInfo[i][0])));
                magnitude1 = abs((rayInfo[i][2]) / sin(glm::radians(rayInfo[i][0])));
            }
            if (magnitude1 > magnitude2){
                magnitudes[i][1] = magnitude2;
            }else{
                magnitudes[i][1] = magnitude1;
            }
        }
    }
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
    window = glfwCreateWindow(WIDTH, HEIGHT, "RayCasting", nullptr, nullptr);
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
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

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

//    float vertices[] = {
//            //front face
//            50.0f, 50.0f, 10.0f,// top right
//            50.0f, -50.0f, 10.0f, // bottom right
//            -50.0f, -50.0f, 10.0f, // bottom left
//            -50.0f, 50.0f, 1.0f, // top left
//            //back face
//            50.0f, 50.0f, -1.0f,// top right
//            50.0f, -50.0f, -1.0f, // bottom right
//            -50.0f, -50.0f, -1.0f, // bottom left
//            -50.0f, 50.0, -1.0f, // top left
//    };
//    GLuint indices[] = {
//            // front face
//            0, 1,
//            1, 2,
//            2, 3,
//            3, 0,
//            // back face
//            4, 5,
//            5, 6,
//            6, 7,
//            7, 4,
//            // connecting faces
//            0, 4,
//            1, 5,
//            2, 6,
//            3, 7
//    };

    float vertices[] = {
            1.0, 0.0f, 1.0f, // start point
            0.0f, 0.0f, 1.0f //endpoint
    };
    float indices[] = {
            0,1
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
    projection = glm::ortho(0.0f, WIDTH, 0.0f, HEIGHT, 0.1f, 100.00f);
    // sends projection data to shader
    int projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLint outputColor = glGetUniformLocation(shaderProgram, "triangleColor");
//    glUniform3f(outputColor, sin(i*M_PI/18.0f)/2 + 0.5, cos(i * M_PI/18.0f)/2 + 0.5, 0.9f);

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
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
        // sends view data to vertex shader
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glUniform4f(outputColor, 1.0f, 1.0f, 1.0f, 1.0f);
        for (GLuint i = 0; i < outputs; i ++){
            // setting Shader Color
            // get ref to triangle color var in frag shader
            float angle = i * 360.0f/outputs;
            // set outputColor to values
//            glUniform3f(outputColor, sin(i*M_PI/18.0f)/2 + 0.5, cos(i * M_PI/18.0f)/2 + 0.5, 0.9f);

            // going 3-D
            // model matrix is the transforms applied to all objects vertices to convert from local to world space
            // initialized to the 4x4 identity matrix
            glm::mat4 model = glm::mat4(1.0f);
            // rotate models 55 degrees along the x-axis
            model = glm::translate(model, startPt);
            model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(magnitudes[i][0], 0.0f, 0.0f));

            // send models transformation data to the vertex shader
            int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

//            glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
        }
        glUniform4f(outputColor, 0.0f, 1.0f, 0.0f, 1.0f);
        for (GLuint i = 0; i < outputs; i ++){
            // setting Shader Color
            // get ref to triangle color var in frag shader
            float angle = 36.0f - i * 360.0f/outputs;
            // set outputColor to values
//            glUniform3f(outputColor, sin(i*M_PI/18.0f)/2 + 0.5, cos(i * M_PI/18.0f)/2 + 0.5, 0.9f);

            // going 3-D
            // model matrix is the transforms applied to all objects vertices to convert from local to world space
            // initialized to the 4x4 identity matrix
            glm::mat4 model = glm::mat4(1.0f);
            // rotate models 55 degrees along the x-axis
//            model = glm::translate(model, startPt);
            model = glm::translate(model, glm::vec3(rayInfo[i][1], rayInfo[i][2], 0.0f));
            model = glm::rotate(model, glm::radians(rayInfo[i][0]), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(magnitudes[i][1], 0.0f, 0.0f));

//            std::cout << rayInfo[i][0] << std::endl;

            // send models transformation data to the vertex shader
            int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
