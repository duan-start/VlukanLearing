#include "Renderer.h"
#include "Context.h"
#include <memory>
namespace VulKan {
	Renderer::~Renderer()
	{
		auto&& device = Context::GetInstance().GetDevice();
		device.freeCommandBuffers(cmdPool_, cmdBuf_);
		device.destroyCommandPool(cmdPool_);
		device.destroySemaphore(imageAvaliable_);
		device.destroySemaphore(imageDrawFinish_);
		device.destroyFence(cmdAvaliableFence_);

	}

	void Renderer::Render()
	{
		//忘记这个逆天语法糖了，牛比的
		auto&& Instance = Context::GetInstance();
		auto&& device = Instance.GetDevice();
		auto result = device.acquireNextImageKHR(Instance.GetSwapChain().GetVKSwapChain(), std::numeric_limits<uint64_t>::max(),imageAvaliable_);
		if (result.result != vk::Result::eSuccess) {
			std::cout << "Acquire next image failed!" << std::endl;
		}
		//这个东西是什么
		auto imageIndex = result.value;
		cmdBuf_.reset();

		vk::CommandBufferBeginInfo begin;
		begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		cmdBuf_.begin(begin);
		{
			vk::RenderPassBeginInfo renderPassInfo;
			vk::Rect2D area;
			vk::ClearValue clearValue;
			clearValue.color = vk::ClearColorValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f});

			area.setOffset({ 0,0 })
				.setExtent(Instance.GetSwapChain().GetInfo().imageExtent);
			//set renderpassBegin
			renderPassInfo.setRenderPass(Instance.GetRenderProc().GetRenderPass())
				.setRenderArea(area)
				.setFramebuffer(Instance.GetSwapChain().GetFrameBuffer()[imageIndex])
				.setClearValues(clearValue);

			//正式设置cmdBuf
			cmdBuf_.beginRenderPass(renderPassInfo, {});
			//setCommand
			{
				cmdBuf_.bindPipeline(vk::PipelineBindPoint::eGraphics, Instance.GetRenderProc().GetRenderPipeline());
				cmdBuf_.draw(3, 1, 0, 0);
			}
			cmdBuf_.endRenderPass();
		}cmdBuf_.end();

		//命令提交
		vk::SubmitInfo submit{};
		submit.setCommandBuffers(cmdBuf_)
			.setWaitSemaphores(imageAvaliable_)
			.setSignalSemaphores(imageDrawFinish_);
		//用对应的队列去提交这个命令
		Instance.GetGraphicQueue().submit(submit,cmdAvaliableFence_);

		//显示命令
		vk::PresentInfoKHR present{};
		present.setImageIndices(imageIndex)
			.setSwapchains(Instance.GetSwapChain().GetVKSwapChain())
			.setWaitSemaphores(imageDrawFinish_);
		if (Instance.GetPresentQueue().presentKHR(present) != vk::Result::eSuccess)
			std::cout << "Image Present failed" << std::endl;

		if (Instance.GetDevice().waitForFences(cmdAvaliableFence_, true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
			std::cout << "wait for fence failed" << std::endl;

		Instance.GetDevice().resetFences(cmdAvaliableFence_);

		//等待屏障同步后再进行下一次绘制

	}
	//申请内存
	void Renderer::InitCmdPool()
	{
		vk::CommandPoolCreateInfo createInfo{};
		//每次命令ci
		createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

		cmdPool_ = Context::GetInstance().GetDevice().createCommandPool(createInfo);
	}
	void Renderer::AllocCmdBuf()
	{
		vk::CommandBufferAllocateInfo createInfo{};
		createInfo.setCommandPool(cmdPool_)
			.setCommandBufferCount(1)
			.setLevel(vk::CommandBufferLevel::ePrimary);

		cmdBuf_ = Context::GetInstance().GetDevice().allocateCommandBuffers(createInfo)[0];
	}
	void Renderer::CreateSems()
	{
		vk::SemaphoreCreateInfo createInfo{};

		imageAvaliable_ = Context::GetInstance().GetDevice().createSemaphore(createInfo);
		imageDrawFinish_ = Context::GetInstance().GetDevice().createSemaphore(createInfo);

	}
	void Renderer::CreateFence()
	{
		vk::FenceCreateInfo createInfo{};
		cmdAvaliableFence_ = Context::GetInstance().GetDevice().createFence(createInfo);
	}
}



