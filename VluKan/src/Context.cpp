#include "Context.h"
#include <iostream>

namespace VulKan {
	std::unique_ptr<Context> Context::m_Instance = nullptr;

	void Context::Init() {
		if(!m_Instance)
		m_Instance.reset(new Context());
	};
	void Context::Quit() {
	};
	Context& Context::GetInstance() {
		if (!m_Instance) Context::Init();
		return *m_Instance;
	}
	Context::~Context()
	{
		vkIns.destroy();
	}
	Context::Context() {
		//先设置（创建）上下文实例
		CreateInstance();
		//挑选可以用作图形渲染的显卡
		PickPhyDevice();
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

		vkIns = vk::createInstance(createInfo);
	}
	void Context::PickPhyDevice()
	{
		auto devices = vkIns.enumeratePhysicalDevices();
		
		//看看哪个显卡可以用来绘制图像
		for (auto& device : devices) {
			auto features = device.getFeatures();
			if (features.geometryShader) phyDevice = device;	
		}
		std::cout << phyDevice.getProperties().deviceName;
	}
	;

	void Init()
	{
		Context::Init();
	}

	void Quit()
	{
		Context::Quit();
	}

}