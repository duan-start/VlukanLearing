#pragma once
#include "src/Context.h"
#include "src/Swapchain.h"
#include "src/Shader.h"
#include  "src/Renderer.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
namespace Utils {
    std::string ReadfromFile(const std::string& path) {
        // 1. 使用二进制模式打开文件，并将文件指针定位到末尾 (ate)
        std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);

        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << path << std::endl;
            return "";
        }

        // 2. 获取文件大小并预分配字符串空间
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg); // 回到文件开头

        std::string result;
        if (size > 0) {
            result.resize(static_cast<size_t>(size));
            // 3. 直接读取到字符串的内存空间中
            if (!file.read(&result[0], size)) {
                std::cerr << "Error reading file: " << path << std::endl;
                return "";
            }
        }

        file.close();
        return result;
    }
}

namespace VulKan {

	void Init(uint32_t WIDTH,uint32_t HEIGHT) {

        //1.创建surface并进行Contxt初始化
        
        //2. 初始化交换链
        VulKan::Context::GetInstance().InitSwapChain(WIDTH, HEIGHT);

        //3.初始化shader
        VulKan::Shader::Init(Utils::ReadfromFile("assets/vert.spv"), Utils::ReadfromFile("assets/frag.spv"));

        //4. 创建pippline
        VulKan::Context::GetInstance().InitPipeline(WIDTH, HEIGHT);

        //5. 创建帧缓冲
        VulKan::Context::GetInstance().GetSwapChain().CreateFrameBuffers(WIDTH, HEIGHT);

        //6.渲染器和渲染命令的提交和配置
        VulKan::Renderer::Init();
		
	};

    //实际上都是栈上创建的的，都会进行销毁，但是通过这个可以控制销毁的顺序
	void Quit() {
        Context::Quit();
    };
}