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
    //询问 GLFW，为了在当前操作系统上显示窗口，我需要向 Vulkan 申请哪些“特权”
    //vk_surfaceKHR
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

    //Extension是用来加扩展功能的
    //Layer是用来进行debug和logging的

    //Init
    {
        VulKan::Context::Init([&](vk::Instance& vkIns)->vk::SurfaceKHR {

            vk::ApplicationInfo appInfo;
            //设置vk版本，反正我的很新
            appInfo.setApiVersion(VK_API_VERSION_1_2);
            
            //设置验证扩展层（用来debug）
            const char* layerExtensions[] = { "VK_LAYER_KHRONOS_validation" };

            vk::InstanceCreateInfo createInfo;
            //设置扩展个数和具体
            createInfo.setEnabledExtensionCount(extensionCount)
                .setPpEnabledExtensionNames(extensions)
                //设置vk版本
                .setPApplicationInfo(&appInfo)
                // 设置layer数量和验证
                .setEnabledLayerCount(1)
                .setPpEnabledLayerNames(layerExtensions);

            //确保了全局资源（如层、扩展）不会在不同的 Vulkan 应用间产生灾难性的冲突
            vkIns = vk::createInstance(createInfo);

            // --- 创建 Surface ---
            // 虽然使用的是 C++ 风格，但创建 Surface 依然要靠 GLFW 的 C 函数
            VkSurfaceKHR rawSurface; // 先用 C 句柄接收

            //Surface和窗口一一对应
            if (glfwCreateWindowSurface(vkIns, window, nullptr, &rawSurface) != VK_SUCCESS)
                throw std::runtime_error("failed to create window surface!");

            // --- 返回时自动封装为 C++ 风格对象 vk::SurfaceKHR ---
            return vk::SurfaceKHR(rawSurface);
            });

        VulKan::Init(WIDTH,HEIGHT);

    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        VulKan::Renderer::GetInstance().Render();
    }

    VulKan::Quit();

}
