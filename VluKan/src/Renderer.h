#pragma once
#include <memory>
#include "vulkan/vulkan.hpp"
namespace VulKan {
	class Renderer
	{
	public:

		~Renderer();

		inline static Renderer& GetInstance() { if (Instance == nullptr) Init();  return *Instance; }
		inline static void Init() { Instance.reset(new Renderer()); };

		void Render();
		

	private:
		Renderer() {
			//Ë³Ðò
			InitCmdPool();
			AllocCmdBuf();
			CreateSems();
			CreateFence();
		};
		void InitCmdPool();
		void AllocCmdBuf();

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


