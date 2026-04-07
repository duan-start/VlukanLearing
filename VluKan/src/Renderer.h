#pragma once
#include <memory>
#include "vulkan/vulkan.hpp"
namespace VulKan {

	//单例设计模式
	class Renderer
	{
	public:
		~Renderer();
		inline static Renderer& GetInstance() { if (Instance == nullptr) Init();  return *Instance; }
		inline static void Init() { Instance.reset(new Renderer()); };
		void Render();
	private:
		Renderer() {
			//顺序
			InitCmdPool();
			AllocCmdBuf();
			CreateSems();
			CreateFence();
		};

		//分配内存和分配BUf
		void InitCmdPool();
		void AllocCmdBuf();

		//创建信号量和栅栏
		void CreateSems();
		void CreateFence();
	private:
		inline static std::unique_ptr<Renderer> Instance = nullptr;
		vk::CommandPool cmdPool_;
		vk::CommandBuffer cmdBuf_;

		vk::Semaphore imageAvaliable_;
		vk::Semaphore imageDrawFinish_;
		vk::Fence cmdAvaliableFence_;
	};
}


