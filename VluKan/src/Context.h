#pragma once
#include "vulkan/vulkan.hpp"
#include <memory>
#include <optional>
namespace VulKan {
	void Init();
	void Quit();

	class Context final
	{
	public:
		static void Init();
		static void Quit();
		static Context& GetInstance();
		~Context();
	private:
		static std::unique_ptr<Context> Instance;
		Context();

		void CreateInstance();
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
		struct QueueFmaile {
			//默认初始化是std::optional::not
			std::optional<uint32_t> GraphicIndex;
		};
		QueueFmaile m_QueueFamilyInex;
	};
}


