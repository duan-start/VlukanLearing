#include "src/VluKan.h"
#include "GLFW/glfw3.h"

#define WIDTH 1200
#define HEIGHT 800

int main(int argc,char* argv) {
    glfwInit();

    // 不使用 OpenGL，并不需要显示指定glfw和vulkan之间的关系
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
   
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Window", nullptr, nullptr);
    VulKan::Init();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    VulKan::Quit;

}
