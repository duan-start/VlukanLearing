#include "Context.h"
#include <iostream>

namespace VulKan {
	std::unique_ptr<Context> Context::Instance = nullptr;

	void Context::Init(CreatSurfaceFunc func) {
		if(!Instance)
		Instance.reset(new Context(func));
	};
	void Context::Quit() {
		Context::GetInstance().GetDevice().waitIdle();
	}
	void Context::InitSwapChain(uint32_t w, uint32_t h)
	{
		m_swapChain.reset(new SwapChain(w,h));
	}

	//初始化渲染管线
	void Context::InitPipeline(uint32_t w, uint32_t h)
	{
	
		m_PipeProcess.reset(new RendererProcess());
		m_PipeProcess->InitLayout();
		m_PipeProcess->InitRenderPass();
		m_PipeProcess->InitRenderPipline(w, h);
	}
	;
	 Context& Context::GetInstance() {
		if (!Instance) std::cout << "No Instance to use;";
		return *Instance;
	}
	Context::~Context()
	{
		m_PipeProcess.reset();
		m_swapChain.reset();
		m_device.destroy();
		m_vkIns.destroySurfaceKHR(m_surface);
		m_vkIns.destroy();

	}
	Context::Context(CreatSurfaceFunc func) {
		//创建Ins
		CreateInstance(func);
		//挑选可以用作图形渲染的显卡
		PickPhyDevice();
		//查询队列族
		QueryQueueFdamilyIndex();
		//创建逻辑设备
		CreateDevice();

		GetQueue();
	}
	void Context::CreateInstance(CreatSurfaceFunc func)
	{
		m_surface=func(m_vkIns);
	}
	//选择物理设备
	void Context::PickPhyDevice()
	{
		//查看所有的物理显卡
		auto devices = m_vkIns.enumeratePhysicalDevices();
		
		//看看哪个显卡可以用来绘制图像
		for (auto& device : devices) {
			//获取显卡的功能
			auto features = device.getFeatures();
			if (features.geometryShader) m_phyDevice = device;	
		}
		//查看显卡的名字（我的就是Intel）
		std::cout << m_phyDevice.getProperties().deviceName;
	}

	//创建逻辑设备
	void Context::CreateDevice()
	{	/**
	* 【逻辑设备创建核心配置清单】
	* * 1. 队列请求 (Queue Create Infos):
	* - 作用: 基于“队列族索引”申请具体执行通道。
	* - 关系: 物理设备提供“族(Family)”，逻辑设备开启“队列(Queue)”。
	* - 核心: 必须指定优先级(0.0~1.0)，决定任务调度的硬件权重。
	* * 2. 物理特性开关 (PhysicalDevice Features):
	* - 作用: 显式开启硬件支持的特定功能（如各向异性过滤、几何着色器等）。
	* - 注意: 默认全关；若代码使用了未开启的特性，驱动将抛出错误。
	* * 3. 设备扩展 (Device Extensions):
	* - 作用: 注入平台无关的渲染能力。
	* - 必填项: VK_KHR_SWAPCHAIN_EXTENSION_NAME。
	* - 逻辑: Instance 扩展负责“看懂窗口”，Device 扩展负责“往窗口画图”。
	* * 4. 校验层 (Enabled Layers):
	* - 现状: 现代 Vulkan 已将 Instance 层级自动同步至 Device。
	* - 惯用法: 为保持向下兼容，通常传入与 Instance 相同的 Layer 列表。
	* * [结论]: vk::Device 是操作硬件的“唯一合法句柄”。
	* 创建后，需通过 device.getQueue() 获取实际提交指令的物理通道(vk::Queue)。
	*/

		const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME // 核心：开启交换链功能
		};

		vk::DeviceCreateInfo creatInfo;

		//队列创建，是从某个拥有特殊功能的队列族里面进行分配的
		//为了方便起见，我们第一次通常选用功能比较多的队列族进行队列创建（创建队列的时候，可以仅仅使用其中的一小部分）

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

		vk::DeviceQueueCreateInfo queueCreateInfo;
		float priority = 1.0f;

		{

			// 作用：0.0(低) ~ 1.0(高)，影响多个 Queue 同时竞争 GPU 时间片时的权重。
			queueCreateInfo.setPQueuePriorities(&priority)
				.setQueueCount(1)//Queue一般有上限；申请执行通道数量:
				.setQueueFamilyIndex(m_QueueFamilyInex.GraphicIndex.value());//某一个队列族里面创建
			queueCreateInfos.push_back(queueCreateInfo);
		}

		if (m_QueueFamilyInex.GraphicIndex.value() != m_QueueFamilyInex.PresentIndex.value()) {
			queueCreateInfo.setPQueuePriorities(&priority)
				.setQueueCount(1)//Queue一般有上限，硬件控制
				.setQueueFamilyIndex(m_QueueFamilyInex.PresentIndex.value());//某一个队列族里面创建
			queueCreateInfos.push_back(queueCreateInfo);
		}
			
		// 1，队列 2， 扩展
		creatInfo.setQueueCreateInfos(queueCreateInfos)
			.setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()))
			.setPpEnabledExtensionNames(deviceExtensions.data());


		m_device = m_phyDevice.createDevice(creatInfo);
	}

	//查询队列族，族是抽象集合，队列是具体实例
	void Context::QueryQueueFdamilyIndex()
	{
		//获取该物理设备的所有族的信息
		auto properties = m_phyDevice.getQueueFamilyProperties();
		//查找graphic queue和presnet Queue
		for (int i = 0; i < properties.size(); i++) {

			//查找绘图的队列族Handle
			if (!m_QueueFamilyInex.GraphicIndex.has_value()&& properties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
				m_QueueFamilyInex.GraphicIndex = i;
			}
			//查找显示的队列族Handle
			if(!m_QueueFamilyInex.PresentIndex.has_value()&&m_phyDevice.getSurfaceSupportKHR(i,m_surface))
			{
				m_QueueFamilyInex.PresentIndex = i;
			}

			//全部查找到后就退出
			if (m_QueueFamilyInex) break;

		}

	}

	void Context::GetQueue()
	{
		//得到对应的命令类型的队列
		m_grahicQueue = m_device.getQueue(m_QueueFamilyInex.GraphicIndex.value(), 0);
		m_PresentQueue=m_device.getQueue(m_QueueFamilyInex.PresentIndex.value(), 0);
	}

}