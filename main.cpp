#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

float deltaTime = 0.0f, lastTime = 0.0f;

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

    // GAMELOOP TIME!!
    while (!glfwWindowShouldClose(window)){
        // getting delta time so movement isn't dependent on frame rate
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
