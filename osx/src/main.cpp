#include "tangram.h"
#include "platform.h"
#include "gl.h"

// Input handling
// ==============

const double double_tap_time = 0.5; // seconds
const double scroll_multiplier = 0.05; // scaling for zoom

bool was_panning = false;
bool rotating = false;
double last_mouse_up = -double_tap_time; // First click should never trigger a double tap
double last_x_down = 0.0;
double last_y_down = 0.0;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    
    if (button != GLFW_MOUSE_BUTTON_1) {
        return; // This event is for a mouse button that we don't care about
    }
    
    if (was_panning) {
        was_panning = false;
        return; // Clicks with movement don't count as taps
    }
    
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    double time = glfwGetTime();
    
    if (action == GLFW_PRESS) {
        last_x_down = x;
        last_y_down = y;
        return;
    }
    
    if (time - last_mouse_up < double_tap_time) {
        Tangram::handleDoubleTapGesture(x, y);
    } else {
        Tangram::handleTapGesture(x, y);
    }
    
    last_mouse_up = time;
    
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
    
    int action = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
    
    if (action == GLFW_PRESS) {
        
        if (was_panning) {
            Tangram::handlePanGesture(x - last_x_down, y - last_y_down);
        }
        
        was_panning = true;
        last_x_down = x;
        last_y_down = y;
    }
    
}

void scroll_callback(GLFWwindow* window, double scrollx, double scrolly) {
    
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    if (rotating) {
        Tangram::handleRotateGesture(scroll_multiplier * scrolly);
    } else {
        Tangram::handlePinchGesture(x, y, 1.0 + scroll_multiplier * scrolly);
    }
    
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    rotating = (mods & GLFW_MOD_SHIFT) != 0; // Whether one or more shift keys is down
}


// Window handling
// ===============

void window_size_callback(GLFWwindow* window, int width, int height) {
    
    Tangram::resize(width, height);
    
}

// Main program
// ============

int main(void) {

    GLFWwindow* window;
    int width = 800;
    int height = 600;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_SAMPLES, 2);
    window = glfwCreateWindow(width, height, "GLFW Window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    Tangram::initialize();
    Tangram::resize(width, height);

    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    
    glfwSwapInterval(1);
    
    double lastTime = glfwGetTime();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {

        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        lastTime = currentTime;
        
        /* Render here */
        Tangram::update(delta);
        Tangram::render();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    
    Tangram::teardown();
    glfwTerminate();
    return 0;
}
