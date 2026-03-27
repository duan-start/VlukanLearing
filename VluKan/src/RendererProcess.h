#pragma once
#include "vulkan/vulkan.hpp"
namespace VulKan {
	//renderpass是渲染流程（自定义的各种render/如shadowRender），可以对应多个renderpipline(标准的渲染通道)
	class RendererProcess
	{
	public:
		//渲染管线
		void InitRenderPipline(uint32_t width, uint32_t height);
		void InitLayout();
		//渲染pass
		void InitRenderPass();

		~RendererProcess();
		RendererProcess() = default;
	private:
		//一个渲染管线
		vk::Pipeline m_Pipline;
		//vao+uniform
		vk::PipelineLayout m_Layout;

		//一个真正的渲染流程
		vk::RenderPass m_RenderPass;
		
	};
}
