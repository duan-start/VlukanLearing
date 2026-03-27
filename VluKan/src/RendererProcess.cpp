#include "RendererProcess.h"
#include "src/Shader.h"
#include "src/Context.h"
namespace VulKan {
	void RendererProcess::InitRenderPipline(uint32_t width,uint32_t height)
	{
		//setp表示设置指针，set表示设置普通数据成员
		
		vk::GraphicsPipelineCreateInfo createInfo;
		//1.vertex input
		//空输入，表示不从vertexbuffer里面读取数据
		vk::PipelineVertexInputStateCreateInfo inputState;
		createInfo.setPVertexInputState(&inputState);
		//2.primitives
		//图元装配的状态
		vk::PipelineInputAssemblyStateCreateInfo inputAsm;
		//表示对于strip是否某处断开，如果是list则一定是false
		inputAsm.setPrimitiveRestartEnable(false)
			//设置图元装配的模型
			.setTopology(vk::PrimitiveTopology::eTriangleList);
		createInfo.setPInputAssemblyState(&inputAsm);
		//3.shader
		//setStages传vector/array, setP 传data()，原始指针
		createInfo.setStages(Shader::GetIns().GetStage());
		//4.viewport
		vk::PipelineViewportStateCreateInfo viewportState;
		vk::Viewport viewport(0, 0, width, height, 0, 1);
		vk::Rect2D sci({ 0,0 }, { (uint32_t)width, (uint32_t)height });
		viewportState.setViewports(viewport)
			.setPScissors(&sci);
		createInfo.setPViewportState(&viewportState);
		//5. Rasterizer
		vk::PipelineRasterizationStateCreateInfo rasInfo;
		rasInfo.setRasterizerDiscardEnable(false)
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eCounterClockwise)
			.setPolygonMode(vk::PolygonMode::eFill)
			.setLineWidth(1);
		createInfo.setPRasterizationState(&rasInfo);

		//6. sample
		vk::PipelineMultisampleStateCreateInfo multisample;
		multisample.setSampleShadingEnable(false)
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);
		createInfo.setPMultisampleState(&multisample);

		//7.test


		//8. colorblend;
		vk::PipelineColorBlendStateCreateInfo blend;
		vk::PipelineColorBlendAttachmentState attachs;
		attachs.setBlendEnable(false)
			.setColorWriteMask(vk::ColorComponentFlagBits::eR |
				vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eB |
				vk::ColorComponentFlagBits::eA);
		blend.setLogicOpEnable(false)
			.setAttachments(attachs);
		createInfo.setPColorBlendState(&blend);

		auto result = Context::GetInstance().GetDevice().createGraphicsPipeline(nullptr,createInfo);

		if (result.result != vk::Result::eSuccess) {
			throw(std::runtime_error("PipeLine Create Error"));
		}
	}
}