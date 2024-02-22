#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <unistd.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "manager.hpp"

static const int window_width = 1200;
static const int window_height = 950;

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
}

static void cursor_moved(GLFWwindow *window, double xpos, double ypos)
{
        Input.last_x = xpos;
        Input.last_y = ypos;
        if (Input.first_mouse)
                Input.first_mouse = false;
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
        glEnable(GL_LINE_SMOOTH);
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
}

static void process_input(bool& paused, bool& info, int& speed)
{
        if (Input.keys['Q'] && !Input.lock['Q']) {
                Input.lock['Q'] = true;
                paused = !paused;
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

static void graphics_simulation(GLFWwindow *window, Manager& Model)
{
        bool paused = true, info = false;
        int speed = 1;
        GLfloat sample, last_frame = glfwGetTime();
        while (!Model.Finished()) {
                glfwPollEvents();
                if (glfwWindowShouldClose(window))
                        break;
                sample = glfwGetTime() - last_frame;
                last_frame = glfwGetTime();
                unsigned long t = sample * 1e6;
                process_input(paused, info, speed);
                if (t <= 40000UL)
                        usleep(40000UL - t);
                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                for (int i = 0; !paused && i < speed; i++)
                        Model.Sample();
                Model.Show(info);
                glFlush();
                assert(glGetError() == GL_NO_ERROR);
                glfwSwapBuffers(window);
        }
        glfwDestroyWindow(window);
        glfwTerminate();
}

static void main_loop(unsigned long T, unsigned int ep, bool graphics_mode)
{
        GLFWwindow *window = 0;
        if (graphics_mode) {
                window = init_gl_context();
                if (!window)
                        exit(1);
                setup_view();
                init_input();
        }

        Manager Model(window_width, window_height, ep, T, graphics_mode);
        Model.Init();
        if (graphics_mode) {
                graphics_simulation(window, Model);
        } else {
                while (!Model.Finished())
                        Model.Sample();
        }
}

static bool graphics_mode = false;
static unsigned int max_iterations = 500000;
static unsigned int episode_length = 500;
static unsigned int random_seed = 42;

int get_command_line_options(int argc, char **argv)
{
    int opt, retval = 0;
    extern char *optarg;
    extern int optopt;
    while ((opt = getopt(argc, argv, ":hgt:e:s:")) != -1) {
        switch (opt) {
        case 'h':
            retval = -1;
            break;
        case 'g':
            graphics_mode = true;
            break;
        case 't':
            max_iterations = atoi(optarg);
            break;
        case 'e':
            episode_length = atoi(optarg);
            break;
        case 's':
            random_seed = atoi(optarg);
            break;
        case ':':
            fprintf(stderr, "Opt -%c require an operand\n", optopt);
            retval = -1;
            break;
        case '?':
            fprintf(stderr, "Unrecognized option: -%c\n", optopt);
            retval = -1;
            break;
        }
    }
    return retval;
}

int main(int argc, char **argv)
{
        get_command_line_options(argc, argv);
        Vehicle::ReadConfig("scripts/config.txt");
        if (random_seed != (unsigned int)-1)
                srand(random_seed);
        else
                srand(time(0));
        main_loop(max_iterations, episode_length, graphics_mode);
        return 0;
}
