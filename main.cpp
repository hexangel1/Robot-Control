#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "manager.hpp"

struct InputState {
        bool keys[1024];
        bool lock[1024];
        bool mouse_captured;
        bool captured_now;
        bool first_mouse;
        double last_x;
        double last_y;
};

static InputState Input;
static const char *const title = "Robot Group Control Simulator";
static const int window_width = 1920;
static const int window_height = 1000;

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
                if (action == GLFW_PRESS) {
                        Input.keys[key] = true;
                        Input.lock[key] = false;
                } else if (action == GLFW_RELEASE) {
                        Input.keys[key] = false;
                }
        }
        (void)code;
        (void)mode;
}

static void cursor_moved(GLFWwindow *window, double xpos, double ypos)
{
        Input.last_x = xpos;
        Input.last_y = ypos;
        if (Input.first_mouse)
                Input.first_mouse = false;
        (void)window;
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
        (void)mode;
}

static GLFWwindow *init_gl_context(void)
{
        GLFWwindow *window;
        if (!glfwInit()) {
                fputs("Failed to initialize glfw\n", stderr);
                return 0;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 8);
        window = glfwCreateWindow(window_width, window_height, title, 0, 0);
        if (!window) {
                glfwTerminate();
                fputs("Failed to create glfw window\n", stderr);
                return 0;
        }
        glfwMakeContextCurrent(window);
        glfwSetKeyCallback(window, key_pressed);
        glfwSetCursorPosCallback(window, cursor_moved);
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
        glOrtho(0.0, window_width, 0.0, window_height, 0.0, 1.0);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

static void init_input(void)
{
        for (int i = 0; i < 1024; i++) {
                Input.keys[i] = false;
                Input.lock[i] = false;
        }
        Input.mouse_captured = false;
        Input.captured_now = false;
        Input.first_mouse = true;
        Input.last_x = 0.0;
        Input.last_y = 0.0;
};

static void process_input(bool& paused, bool& info, bool& drop, int& speed)
{
        if (Input.keys['Q'] && !Input.lock['Q']) {
                Input.lock['Q'] = true;
                paused = !paused;
        }
        if (Input.keys['Y'] && !Input.lock['Y']) {
                Input.lock['Y'] = true;
                drop = true;
        }
        if (Input.keys['E'] && !Input.lock['E']) {
                Input.lock['E'] = true;
                info = !info;
        }
        if (Input.keys['F'] && !Input.lock['F']) {
                Input.lock['F'] = true;
                if (speed < 8)
                        speed++;
        }
        if (Input.keys['S'] && !Input.lock['S']) {
                Input.lock['S'] = true;
                if (speed > 0)
                        speed--;
        }
}

static void main_loop(GLFWwindow *window)
{
        Manager Model(window_width, window_height);
        bool paused = true;
        bool info = false;
        bool drop = false;
        int speed = 1;
        Model.Init();
        for (;;) {
                glfwPollEvents();
                if (glfwWindowShouldClose(window))
                        break;
                process_input(paused, info, drop, speed);
                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                for (int i = 0; !paused && i < speed; i++)
                        Model.Update();
                Model.Show(info, drop);
                drop = false;
                glFlush();
                assert(glGetError() == GL_NO_ERROR);
                glfwSwapBuffers(window);
        }
}

int main()
{
        GLFWwindow *window;
        window = init_gl_context();
        if (!window)
                return 1;
        setup_view();
        init_input();
        main_loop(window);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
}

