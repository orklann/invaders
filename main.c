#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <GLFW/glfw3.h>

#include "types.h"
#include "cpu.h"
#include "gpu.h"

const int WIN_WIDTH = 224;
const int WIN_HEIGHT = 256;
const int FPS = 60;

static i8080 m;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode,
                         int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_C) { // coin
            m.port1 = (1 << 0) | m.port1;
        }
        else if (key == GLFW_KEY_2) { // P2 start button
            m.port1 = (1 << 1) | m.port1;
        }
        else if (key == GLFW_KEY_ENTER) { // P1 start button
            m.port1 = (1 << 2) | m.port1;
        }
        // else if (key == x) { // ?
        //     m.port1 = (1 << 3) | m.port1;
        // }
        else if (key == GLFW_KEY_SPACE) {
            m.port1 = (1 << 4) | m.port1; // P1 shoot button
            m.port2 = (1 << 4) | m.port2; // P2 shoot button
        }
        else if (key == GLFW_KEY_LEFT) {
            m.port1 = (1 << 5) | m.port1; // P1 joystick left
            m.port2 = (1 << 5) | m.port2; // P2 joystick left
        }
        else if (key == GLFW_KEY_RIGHT) {
            m.port1 = (1 << 6) | m.port1; // P1 joystick right
            m.port2 = (1 << 6) | m.port2; // P2 joystick right
        }
        // else if (key == x) { // ?
        //     m.port1 = (1 << 7) | m.port1;
        // }
    }
    else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_C) { // coin
            m.port1 = 0b11111110 & m.port1;
        }
        else if (key == GLFW_KEY_2) { // P2 start button
            m.port1 = 0b11111101 & m.port1;
        }
        else if (key == GLFW_KEY_ENTER) { // P1 start button
            m.port1 = 0b11111011 & m.port1;
        }
        // else if (key == x) { // ?
        // }
        else if (key == GLFW_KEY_SPACE) {
            m.port1 = 0b11101111 & m.port1; // P1 shoot button
            m.port2 = 0b11101111 & m.port2; // P2 shoot button
        }
        else if (key == GLFW_KEY_LEFT) {
            m.port1 = 0b11011111 & m.port1; // P1 joystick left
            m.port2 = 0b11011111 & m.port2; // P2 joystick left
        }
        else if (key == GLFW_KEY_RIGHT) {
            m.port1 = 0b10111111 & m.port1; // P1 joystick right
            m.port2 = 0b10111111 & m.port2; // P2 joystick right
        }
        // else if (key == x) { // ?
        // }
    }
}

int main(int argc, char **argv) {
    // run tests
    if (argc == 2 && !strcmp(argv[1], "--test")) {
        cpu_init(&m);
        cpu_run_tests(&m, "roms/tests/TEST.COM");
        cpu_init(&m);
        cpu_run_tests(&m, "roms/tests/8080PRE.COM");
        // cpu_init(&m);
        // cpu_run_tests(&m, "roms/tests/8080EX1.COM");
        cpu_init(&m);
        cpu_run_tests(&m, "roms/tests/CPUTEST.COM");
        return 0;
    }

    cpu_init(&m);
    if (cpu_load_file(&m, "roms/invaders.h", 0x0000) != 0) return -1;
    if (cpu_load_file(&m, "roms/invaders.g", 0x0800) != 0) return -1;
    if (cpu_load_file(&m, "roms/invaders.f", 0x1000) != 0) return -1;
    if (cpu_load_file(&m, "roms/invaders.e", 0x1800) != 0) return -1;

    // GLFW setup
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        return -1;
    }

    // create a window and its context
    window = glfwCreateWindow(WIN_WIDTH * 2, WIN_HEIGHT * 2, "Space Invaders",
                              NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwSetKeyCallback(window, key_callback);

    glfwSetWindowSizeLimits(window, WIN_WIDTH, WIN_HEIGHT,
                            GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(window);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glfwSwapInterval(1);

    gpu_init(&m);

    float step_timer = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        gpu_draw(&m);
        glfwSwapBuffers(window);

        glfwPollEvents();

        if (glfwGetTime() - step_timer > 1.f / (FPS + 10)) {
            // (+10 to boost up the game's speed a little)
            step_timer = glfwGetTime();
            cpu_update(&m);
            gpu_update(&m);
        }
    }

    glfwTerminate();
    return 0;
}
