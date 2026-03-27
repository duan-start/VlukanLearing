#pragma once
#include "vulkan/vulkan.hpp"
#include "src/Swapchain.h"
#include "src/RendererProcess.h"
#include <memory>
#include <optional>
#include <functional>
#include <iostream>

namespace VulKan {
	class Context final
	{
	public:

		using CreatSurfaceFunc = std::function<vk::SurfaceKHR(vk::Instance&)>;
		static void Init(CreatSurfaceFunc func);
		static void Quit();
		void InitSwapChain(uint32_t w, uint32_t h);
		void InitPipeline(uint32_t w, uint32_t h);
		static Context& GetInstance();
		~Context();

	public:
		struct QueueFmaile {
			//默认初始化是std::optional::not
			std::optional<uint32_t> GraphicIndex;
			std::optional<uint32_t> PresentIndex;

			operator bool() { return GraphicIndex.has_value() && PresentIndex.has_value(); }
		};

		 vk::PhysicalDevice GetPhyDevice() { return m_phyDevice; };
		 vk::Device GetDevice() { return m_device; }
		 vk::SurfaceKHR GetSurface() { if (m_surface == VK_NULL_HANDLE)std::cout << "Wrong"; return m_surface; }

		 QueueFmaile& GetQueueFamilyIndices() {return m_QueueFamilyInex;}
	private:
		static std::unique_ptr<Context> Instance;
		Context(CreatSurfaceFunc func);

		void CreateInstance(CreatSurfaceFunc func);
		void PickPhyDevice();
		void CreateDevice();
		void QueryQueueFdamilyIndex();
		void GetQueue();
		
	private:
		//渲染实例
		vk::Instance m_vkIns;
		//选择物理设备
		vk::PhysicalDevice m_phyDevice;
		//创建交互的逻辑设备
		vk::Device m_device;
		vk::Queue m_grahicQueue;
		vk::Queue m_PresentQueue;
		
		QueueFmaile m_QueueFamilyInex;
		//surface khr(渲染到窗口)[绑定vkimage 和窗口]
		vk::SurfaceKHR m_surface;

		//swapChain
		std::unique_ptr<SwapChain> m_swapChain;

		//Pipeline
		std::unique_ptr<RendererProcess> m_PipeProcess;
	};
}


