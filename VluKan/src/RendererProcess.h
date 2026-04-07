#pragma once
#include "vulkan/vulkan.hpp"
namespace VulKan {
	//renderpass是渲染流程（自定义的各种render/如shadowRender），可以对应多个renderpipline(标准的渲染通道)
	//将散乱的配置（Shader、混合模式、深度测试、附件描述）封装成了一个静态的执行方案
	class RendererProcess
	{
	public:
		//shader接受数据
		void InitLayout();
		//具体管线
		void InitRenderPipline(uint32_t width, uint32_t height);
		//渲染流程
		void InitRenderPass();

	public:
		const vk::Pipeline& GetRenderPipeline() const { return m_Pipeline; }
		const vk::RenderPass& GetRenderPass()const { return m_RenderPass; }
		~RendererProcess();
		RendererProcess() = default;
	private:
		//一个渲染管线
		vk::Pipeline m_Pipeline;
		//vao+uniform
		vk::PipelineLayout m_Layout;

		//一个真正的渲染流程
		vk::RenderPass m_RenderPass;
		
	};
}
