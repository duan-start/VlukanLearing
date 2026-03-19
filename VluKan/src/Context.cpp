#include "Context.h"
#include <iostream>

namespace VulKan {
	std::unique_ptr<Context> Context::Instance = nullptr;

	void Context::Init() {
		if(!Instance)
		Instance.reset(new Context());
	};
	void Context::Quit() {
	};
	Context& Context::GetInstance() {
		if (!Instance) Context::Init();
		return *Instance;
	}
	Context::~Context()
	{
		m_vkIns.destroy();
	}
	Context::Context() {
		//先设置（创建）上下文实例
		CreateInstance();
		//挑选可以用作图形渲染的显卡
		PickPhyDevice();
		//查询队列族
		QueryQueueFdamilyIndex();
		//创建逻辑设别
		CreateDevice();
		GetQueue();
	}
	void Context::CreateInstance()
	{
		//参数打包，结构体
		vk::InstanceCreateInfo createInfo;

		auto layers = vk::enumerateInstanceLayerProperties();

		vk::ApplicationInfo appInfo;
		//设置vk版本，反正我的很新
		appInfo.setApiVersion(VK_API_VERSION_1_3);
		const char* extensions[] = { "VK_LAYER_KHRONOS_validation" };
		createInfo.setPApplicationInfo(&appInfo)
			.setPpEnabledLayerNames(extensions);

		m_vkIns = vk::createInstance(createInfo);
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
		vk::DeviceCreateInfo creatInfo;

		//队列创建，是从某个拥有特殊功能的队列族里面进行分配的
		//为了方便起见，我们第一次通常选用功能比较多的队列族进行队列创建（创建队列的时候，可以仅仅使用其中的一小部分）
		vk::DeviceQueueCreateInfo queueCreateInfo;
		float priority = 1.0f;
		queueCreateInfo.setPQueuePriorities(&priority)
			.setQueueCount(1)//Queue一般有上限，硬件控制
			.setQueueFamilyIndex(m_QueueFamilyInex.GraphicIndex.value());//从某一个队列族里面创建
			
		creatInfo.setQueueCreateInfos(queueCreateInfo);
		m_device = m_phyDevice.createDevice(creatInfo);

	}
	void Context::QueryQueueFdamilyIndex()
	{
		//Init graphicIndex;
		auto properties = m_phyDevice.getQueueFamilyProperties();
		for (int i = 0; i < properties.size(); i++) {
			if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
				m_QueueFamilyInex.GraphicIndex = i;
				break;//查找出第一个就退出
			}
		}
	}

	void Context::GetQueue()
	{
		//得到对应的命令队列
		m_grahicQueue = m_device.getQueue(m_QueueFamilyInex.GraphicIndex.value(), 0);
	}


	void Init()
	{
		Context::Init();
	}

	void Quit()
	{
		Context::Quit();
	}

}