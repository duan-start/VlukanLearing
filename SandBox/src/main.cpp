#include "src/VluKan.h"
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.hpp"
#define WIDTH 1200
#define HEIGHT 800

int main(int argc,char* argv) {
    glfwInit();

    // 不使用 OpenGL，并不需要显示指定glfw和vulkan之间的关系
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
   
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Window", nullptr, nullptr);

    // 获取当前窗口的扩展信息
    uint32_t extensionCount = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

    //Init
    {
        VulKan::Context::Init([&](vk::Instance& vkIns)->vk::SurfaceKHR {

            vk::ApplicationInfo appInfo;
            //设置vk版本，反正我的很新
            appInfo.setApiVersion(VK_API_VERSION_1_2);
            const char* layerExtensions[] = { "VK_LAYER_KHRONOS_validation" };

            vk::InstanceCreateInfo createInfo;
            createInfo.setEnabledExtensionCount(extensionCount);
            createInfo.setPpEnabledExtensionNames(extensions)
                .setPApplicationInfo(&appInfo)
                // 关键：必须显式设置 Layer 数量！
                .setEnabledLayerCount(1)
                .setPpEnabledLayerNames(layerExtensions);
            vkIns = vk::createInstance(createInfo);
            // --- 创建 Surface ---
            // 虽然使用的是 C++ 风格，但创建 Surface 依然要靠 GLFW 的 C 函数
            VkSurfaceKHR rawSurface; // 先用 C 句柄接收

            //surface虽然是靠window创建的，但是里面并没有winodw的handle
            if (glfwCreateWindowSurface(vkIns, window, nullptr, &rawSurface) != VK_SUCCESS)
                throw std::runtime_error("failed to create window surface!");

            // --- 返回时自动封装为 C++ 风格对象 vk::SurfaceKHR ---
            return vk::SurfaceKHR(rawSurface);
            });
        VulKan::Context::GetInstance().InitSwapChain(WIDTH, HEIGHT);

        VulKan::Shader::Init(Utils::ReadfromFile("assets/vert.spv"), Utils::ReadfromFile("assets/frag.spv"));
        VulKan::Context::GetInstance().InitPipeline(WIDTH, HEIGHT);
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    VulKan::Context::Quit();

}
