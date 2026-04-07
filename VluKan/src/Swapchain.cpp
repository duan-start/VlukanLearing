#include "Swapchain.h"
#include "Context.h"


namespace VulKan {
	SwapChain::SwapChain(uint32_t w, uint32_t h)
	{
		//查询信息
		QueryInfo(w, h);

		vk::SwapchainCreateInfoKHR createInfo{};
		//设置交换链创建
		//clip表示如果窗口被遮挡了，就不进行绘制
		createInfo.setClipped(true)
			//
			.setImageArrayLayers(1)
			//设置图像作为颜色附件
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
			//设置颜色混合为不透明
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			//设置surface信息
			.setSurface(Context::GetInstance().GetSurface())
			//设置颜色空间
			.setImageFormat(m_SwapChainInfo.format.format)
			.setImageColorSpace(m_SwapChainInfo.format.colorSpace)
			//设置分辨率
			.setImageExtent(m_SwapChainInfo.imageExtent)
			//设置缓冲区数量(所以在创建Images时不需要指定)
			.setMinImageCount(m_SwapChainInfo.imageCount)
			//设置垂直同步的格式（最好的就是emailBox）
			.setPresentMode(m_SwapChainInfo.present);

		//设置图像资源是否被队列族之间共享
		auto& queueIndices = Context::GetInstance().GetQueueFamilyIndices();
		if (queueIndices.GraphicIndex.value() == queueIndices.PresentIndex.value()) {
			createInfo.setQueueFamilyIndices(queueIndices.GraphicIndex.value())
				//不与其他familyIndex共享
				.setImageSharingMode(vk::SharingMode::eExclusive);
		}
		else {
			std::array indices = { queueIndices.GraphicIndex.value() , queueIndices.PresentIndex.value() };
			createInfo.setQueueFamilyIndices(indices)
				//不与其他familyIndex共享
				.setImageSharingMode(vk::SharingMode::eConcurrent);
		}
		m_SwapChain = Context::GetInstance().GetDevice().createSwapchainKHR(createInfo);

		//SetImages
		GetImages();
		//设置视图
		SetImageViews();

	}
	SwapChain::~SwapChain()
	{
		for (auto& fram : m_FrameBuffers) {
			Context::GetInstance().GetDevice().destroyFramebuffer(fram);
		}

		for (auto& view : m_ImageViews) {
			Context::GetInstance().GetDevice().destroyImageView(view);
		}
		for (auto& image : m_Images) {
			Context::GetInstance().GetDevice().destroyImage(image);
		}

		Context::GetInstance().GetDevice().destroySwapchainKHR(m_SwapChain);
		
	}

	//配置交换链信息
	void SwapChain::QueryInfo(uint32_t w, uint32_t h)
	{

	 auto&& phyDevice = Context::GetInstance().GetPhyDevice();
	 auto&& surface = Context::GetInstance().GetSurface();

		if (surface == VK_NULL_HANDLE) std::cout << "Surface Create Error";
		//支持的格式
		auto formats = phyDevice.getSurfaceFormatsKHR(surface);
		m_SwapChainInfo.format = formats[0];
		//只能选择配置，不能手动自定义（解释显存）
		for (const auto& format : formats) {
			//查找是否支持我们想要的格式
			//srgb是指非线性空间
			//不需要手动gamma校正和解压
			if (format.format == vk::Format::eR8G8B8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				m_SwapChainInfo.format = format;
				break;
			}
		}
		//表面能力查询（主要是绘制图像数量，图像的extend）
		auto capabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
		//缓冲数量
		m_SwapChainInfo.imageCount = std::clamp<uint32_t>(2, capabilities.minImageCount, capabilities.maxImageCount);

		//将w和h限制在交换链的合法区间中
		m_SwapChainInfo.imageExtent.width = std::clamp<uint32_t>(w, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		m_SwapChainInfo.imageExtent.height = std::clamp<uint32_t>(h, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		//交换链并不会对图像做而外的变换
		m_SwapChainInfo.transform = capabilities.currentTransform;

		//显示模式
		auto presents = phyDevice.getSurfacePresentModesKHR(surface);
		m_SwapChainInfo.present = presents[0];
		for (const auto& present : presents) {
			//如果可以的话，选择emailbox的显示模式
			if (present == vk::PresentModeKHR::eMailbox) {
				m_SwapChainInfo.present = present;
				break;
			}
		}
	}

	//创建帧缓冲（前提时RenderPass被构建）
	void SwapChain::CreateFrameBuffers(uint32_t w,uint32_t h)
	{
		m_FrameBuffers.resize(m_Images.size());
		for (int i = 0; i < m_FrameBuffers.size(); i++) {
			vk::FramebufferCreateInfo createInfo;
			createInfo.setAttachments(m_ImageViews[i])
				.setWidth(w)
				.setHeight(h)
				.setRenderPass(Context::GetInstance().GetRenderProc().GetRenderPass())
				.setLayers(1);
			m_FrameBuffers[i] = Context::GetInstance().GetDevice().createFramebuffer(createInfo);
		}
	}
	void SwapChain::GetImages()
	{
		//创建Images
		m_Images = Context::GetInstance().GetDevice().getSwapchainImagesKHR(m_SwapChain);
	}

	//ImageViews是针对实际Image的读写说明书（并不是只读的，准确来讲，应该是实际操作的基本单位）
	void SwapChain::SetImageViews()
	{
		//ImageView和Images
		m_ImageViews.resize(m_Images.size());
		//SetImageViews
		for (int i = 0; i < m_Images.size(); i++) {
			vk::ImageViewCreateInfo createInfo{};
			//设置映射关系

			//RGBA映射之间的关系
			vk::ComponentMapping mapping;
			//读取的资源范围
			vk::ImageSubresourceRange range;

			//设置读取资源范围
			range.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1)
				//按颜色格式读取
				.setAspectMask(vk::ImageAspectFlagBits::eColor);
			
			createInfo.setImage(m_Images[i])
				.setViewType(vk::ImageViewType::e2D)
				.setComponents(mapping)
				//Image格式的二次确认，和交换链的格式必须一致
				.setFormat(m_SwapChainInfo.format.format)
				.setSubresourceRange(range);


			m_ImageViews[i] = Context::GetInstance().GetDevice().createImageView(createInfo);
		}
	}

}