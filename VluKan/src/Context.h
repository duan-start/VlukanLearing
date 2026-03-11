#pragma once
#include "vulkan/vulkan.hpp"
#include <memory>
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
		static std::unique_ptr<Context> m_Instance;
		Context();

		void CreateInstance();
		void PickPhyDevice();
	private:
		vk::Instance vkIns;
		vk::PhysicalDevice phyDevice;
	};
}


