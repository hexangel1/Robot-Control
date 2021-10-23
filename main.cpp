#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include "manager.hpp"

#define UNUSED(x) (void)x

struct InputState {
        bool keys[1024];
        bool mouse_captured;
        bool captured_now;
        bool first_mouse;
        double last_x;
        double last_y;
};

static InputState Input;

static const GLuint window_width = 1920;
static const GLuint window_height = 1000;

static void gl_check_errors(void)
{
        GLenum gl_error = glGetError();
        if (gl_error != GL_NO_ERROR) {
                fprintf(stderr, "OpenGL error: %d\n", gl_error);
                glfwTerminate();
                exit(1);
        }
}

static void key_pressed(GLFWwindow *window, int key, int code,
                        int action, int mode)
{
        switch (key) {
        case GLFW_KEY_ESCAPE:
                if (action == GLFW_PRESS)
                        glfwSetWindowShouldClose(window, GL_TRUE);
                break;
        case GLFW_KEY_1:
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;
        case GLFW_KEY_2:
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                break;
        default:
                if (action == GLFW_PRESS)
                        Input.keys[key] = true;
                if (action == GLFW_RELEASE)
                        Input.keys[key] = false;
        }
        UNUSED(code);
        UNUSED(mode);
}

static void mouse_moved(GLFWwindow *window, double xpos, double ypos)
{
        Input.last_x = xpos;
        Input.last_y = ypos;
        if (Input.first_mouse)
                Input.first_mouse = false;
        UNUSED(window);
}

static void mouse_clicked(GLFWwindow *window, int button, int action, int mode)
{
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
                Input.mouse_captured = !Input.mouse_captured;
        if (Input.mouse_captured){
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                Input.captured_now = true;
        } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        UNUSED(mode);
}

static GLFWwindow *setupGL(void)
{
        const char *header = "Robot Control Simulator";
        GLFWwindow *window;
        if (!glfwInit()) {
                fputs("Failed to initialize glfw\n", stderr);
                return 0;
        }
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 8);
        window = glfwCreateWindow(window_width, window_height, header, 0, 0);
        if (!window) {
                glfwTerminate();
                fputs("Failed to create glfw window\n", stderr);
                return 0;
        }
        glfwMakeContextCurrent(window);
        glfwSetKeyCallback(window, key_pressed);
        glfwSetCursorPosCallback(window, mouse_moved);
        glfwSetMouseButtonCallback(window, mouse_clicked);
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
                glfwTerminate();
                fputs("Failed to initialize GLEW\n", stderr);
                return 0;
        }
        glEnable(GL_MULTISAMPLE);
        fprintf(stderr, "Vendor: %s\n", glGetString(GL_VENDOR));
        fprintf(stderr, "Renderer: %s\n", glGetString(GL_RENDERER));
        fprintf(stderr, "Version: %s\n", glGetString(GL_VERSION));
        fprintf(stderr, "GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        return window;
}

static void setup_view(void)
{
        glViewport(0.0, 0.0, window_width, window_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, window_width, window_height, 0.0, -1.0, 1.0);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        gl_check_errors();
}

static void init_input(void)
{
        for (int i = 0; i < 1024; i++)
                Input.keys[i] = false;
        Input.mouse_captured = true;
        Input.captured_now = false;
        Input.first_mouse = true;
        Input.last_x = 0.0;
        Input.last_y = 0.0;
};

static void handle_events(bool& paused, bool& info)
{
        static int key_not_active = 0;
        if (!key_not_active) {
                if (Input.keys['q'] || Input.keys['Q']) {
                        key_not_active = 16;
                        paused = !paused;
                }
                if (Input.keys['e'] || Input.keys['E']) {
                        key_not_active = 16;
                        info = !info;
                }
        }
        if (key_not_active)
                key_not_active--;
}

static void main_loop(GLFWwindow *window)
{
        Manager Model(window_width, window_height);
        bool paused = true;
        bool info = false;
        Model.Init();
        while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
                handle_events(paused, info);
                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                Model.Update(paused, info);
                glFlush();
                glfwSwapBuffers(window);
                gl_check_errors();
        }
}

int main(void)
{
        GLFWwindow *window;
        window = setupGL();
        if (!window)
                return 1;
        setup_view();
        init_input();
        main_loop(window);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
}

