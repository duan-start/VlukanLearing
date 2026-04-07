#include "RendererProcess.h"
#include "src/Shader.h"
#include "src/Context.h"
namespace VulKan {

	//初始化具体的管线
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
			//设置图元装配的模型（而不是填充模式）
			.setTopology(vk::PrimitiveTopology::eTriangleList);
		createInfo.setPInputAssemblyState(&inputAsm);
		//3.shader(设置shader状态)
		//setStages传vector/array, setP 传data()，原始指针
		createInfo.setStages(Shader::GetIns().GetStage());
		//4.viewport（设置窗口（按这个大小进行绘制的）和裁剪（实际显示的））
		vk::PipelineViewportStateCreateInfo viewportState;
		vk::Viewport viewport(0, 0, width, height, 0, 1);
		vk::Rect2D sci({ 0,0 }, { (uint32_t)width, (uint32_t)height });
		viewportState.setViewports(viewport)
			.setPScissors(&sci)
			.setScissorCount(1)
			.setViewportCount(1);

		createInfo.setPViewportState(&viewportState);
		//5. Rasterizer（设置光栅化）
		vk::PipelineRasterizationStateCreateInfo rasInfo;

		//不丢弃光栅化内容
		rasInfo.setRasterizerDiscardEnable(false)
			//设置背面剔除
			.setCullMode(vk::CullModeFlagBits::eBack)
			//逆时针为正面
			.setFrontFace(vk::FrontFace::eClockwise)
			//设置多边形填充（填充模式）
			.setPolygonMode(vk::PolygonMode::eFill)
			//设置线框，只是对eline填充有效
			.setLineWidth(1);
		
		//
		createInfo.setPRasterizationState(&rasInfo);

		//6. sample(对光栅化结果进行采样->)
		vk::PipelineMultisampleStateCreateInfo multisample;
		//是否对每一个采样的都进行shading(如果只是普通的抗锯齿的话，并不需要SSAA)
		multisample.setSampleShadingEnable(false)
			//设置采样的个数（MsAA）
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);
		createInfo.setPMultisampleState(&multisample);

		//7.test


		//8. colorblend（设置颜色混合）
		//全局混合的规则
		vk::PipelineColorBlendStateCreateInfo blend;
		//每个附件混合的规则
		vk::PipelineColorBlendAttachmentState attachs;
		//禁用混合，直接覆盖
		attachs.setBlendEnable(false)
			//设置颜色写入权限（每次更新）
			.setColorWriteMask(vk::ColorComponentFlagBits::eR |
				vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eB |
				vk::ColorComponentFlagBits::eA);
		
		//不要用数学位运算（比如 XOR、AND、OR）来处理颜色，直接用普通的加减乘除混合。（只能二选一）
		blend.setLogicOpEnable(false)
			//把上面定义的单个附件规则绑定到全局状态里。（与Renderpass的attachment必须一一对应）
			.setAttachments(attachs);
		//设置全局状态
		createInfo.setPColorBlendState(&blend);

		//9. setRenderpass（设置renderpass和Layout）
		//表示该pipeline只能给这种对应的renderpass使用，并不是绑死具体的pipline,而是绑定对应的pipeline格式
		createInfo.setRenderPass(m_RenderPass)
			.setLayout(m_Layout);

		//管线创建
		auto result = Context::GetInstance().GetDevice().createGraphicsPipeline(nullptr,createInfo);

		if (result.result != vk::Result::eSuccess) {
			throw(std::runtime_error("PipeLine Create Error"));
		}
		m_Pipeline = result.value;
	}
	void RendererProcess::InitLayout()
	{
		//Layout(Binding + Set)[由于绘制三角形，我们并不需要传入顶点数据]
		vk::PipelineLayoutCreateInfo createInfo;
		m_Layout = Context::GetInstance().GetDevice().createPipelineLayout(createInfo);
	}

	//RenderPass
	void RendererProcess::InitRenderPass()
	{
		vk::RenderPassCreateInfo createInfo;

		//附件描述（初始状态，渲染过程，渲染结果）
		vk::AttachmentDescription attachDesc;

		//1.输出格式（不需要输入格式） 【附件的信息】
		attachDesc.setFormat(Context::GetInstance().GetSwapChain().GetInfo().format.format)
			//不关心已有数据
			.setInitialLayout(vk::ImageLayout::eUndefined)
			//输出显示颜色附件（优化格式）
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
			//每次载入的时候清屏
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			//进行保存
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			//并不关心模板的写入和保存
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			//硬件解释（多重采样）
			.setSamples(vk::SampleCountFlagBits::e1);

		//传入attachments的信息
		createInfo.setAttachments(attachDesc);
	
		vk::AttachmentReference reference;
		reference.setLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setAttachment(0);

		vk::SubpassDescription subpass;
		subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setColorAttachments(reference);
		createInfo.setSubpasses(subpass);

		//设置个subpadd的先后关系
		vk::SubpassDependency dependency;
		//1.外部的subpass
		dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
			//2.subpass数组里面的第0个
			.setDstSubpass(0)
			//
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
			.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);

		createInfo.setDependencies(dependency);

		m_RenderPass = Context::GetInstance().GetDevice().createRenderPass(createInfo);

	}
	RendererProcess::~RendererProcess()
	{
		const auto& device = Context::GetInstance().GetDevice();
		
		device.destroyPipeline(m_Pipeline);
		device.destroyRenderPass(m_RenderPass);
		device.destroyPipelineLayout(m_Layout);
	}
}