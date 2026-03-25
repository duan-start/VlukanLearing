#include "Context.h"
#include <iostream>

namespace VulKan {
	std::unique_ptr<Context> Context::Instance = nullptr;

	void Context::Init(CreatSurfaceFunc func) {
		if(!Instance)
		Instance.reset(new Context(func));
	};
	void Context::Quit() {

	}
	void Context::InitSwapChain(uint32_t w, uint32_t h)
	{
		m_swapChain.reset(new SwapChain(w,h));
	}
	;
	 Context& Context::GetInstance() {
		if (!Instance) std::cout << "No Instance to use;";
		return *Instance;
	}
	Context::~Context()
	{
		m_swapChain.reset();

		m_device.destroy();
		m_vkIns.destroySurfaceKHR(m_surface);
		m_vkIns.destroy();

	}
	Context::Context(CreatSurfaceFunc func) {
		//先设置（创建）上下文实例
		CreateInstance(func);
		//挑选可以用作图形渲染的显卡
		PickPhyDevice();
		//查询队列族
		QueryQueueFdamilyIndex();
		//创建逻辑设别
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
		auto devices = m_vkIns.enumeratePhysicalDevices();
		
		//看看哪个显卡可以用来绘制图像
		for (auto& device : devices) {
			auto features = device.getFeatures();
			if (features.geometryShader) m_phyDevice = device;	
		}
		std::cout << m_phyDevice.getProperties().deviceName;
	}

	void Context::CreateDevice()
	{
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
			queueCreateInfo.setPQueuePriorities(&priority)
				.setQueueCount(1)//Queue一般有上限，硬件控制
				.setQueueFamilyIndex(m_QueueFamilyInex.GraphicIndex.value());//某一个队列族里面创建
			queueCreateInfos.push_back(queueCreateInfo);
		}

		if (m_QueueFamilyInex.GraphicIndex.value() != m_QueueFamilyInex.PresentIndex.value()) {
			queueCreateInfo.setPQueuePriorities(&priority)
				.setQueueCount(1)//Queue一般有上限，硬件控制
				.setQueueFamilyIndex(m_QueueFamilyInex.PresentIndex.value());//某一个队列族里面创建
			queueCreateInfos.push_back(queueCreateInfo);
		}
			
		// --- 关键修正点 ---
		creatInfo.setQueueCreateInfos(queueCreateInfos)
			.setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()))
			.setPpEnabledExtensionNames(deviceExtensions.data());


		m_device = m_phyDevice.createDevice(creatInfo);
	}

	void Context::QueryQueueFdamilyIndex()
	{
		//Init graphicIndex;
		auto properties = m_phyDevice.getQueueFamilyProperties();
		//查找graphic queue和presnet Queue
		for (int i = 0; i < properties.size(); i++) {
			if (!m_QueueFamilyInex.GraphicIndex.has_value()&& properties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
				m_QueueFamilyInex.GraphicIndex = i;
			}
			if(!m_QueueFamilyInex.PresentIndex.has_value()&&m_phyDevice.getSurfaceSupportKHR(i,m_surface))
			{
				m_QueueFamilyInex.PresentIndex = i;
			}
			if (m_QueueFamilyInex) break;

		}

	}

	void Context::GetQueue()
	{
		//得到对应的命令队列
		m_grahicQueue = m_device.getQueue(m_QueueFamilyInex.GraphicIndex.value(), 0);
		m_PresentQueue=m_device.getQueue(m_QueueFamilyInex.PresentIndex.value(), 0);
	}

}