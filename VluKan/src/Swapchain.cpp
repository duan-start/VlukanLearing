#include "Swapchain.h"
#include "Context.h"


namespace VulKan {
	SwapChain::SwapChain(uint32_t w, uint32_t h)
	{
		//查询信息
		QueryInfo(w, h);
		vk::SwapchainCreateInfoKHR createInfo{};
		//设置交换链创建
		createInfo.setClipped(true)
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
			//设置缓冲区数量
			.setMinImageCount(m_SwapChainInfo.imageCount)
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
		SetImageViews();

	}
	SwapChain::~SwapChain()
	{
		for (auto& view : m_ImageViews) {
			Context::GetInstance().GetDevice().destroyImageView(view);
		}
		for (auto& image : m_Images) {
			Context::GetInstance().GetDevice().destroyImage(image);
		}

		Context::GetInstance().GetDevice().destroySwapchainKHR(m_SwapChain);
		
	}
	void SwapChain::QueryInfo(uint32_t w, uint32_t h)
	{

	const auto& phyDevice = Context::GetInstance().GetPhyDevice();
	const auto& surface = Context::GetInstance().GetSurface();

		if (surface == VK_NULL_HANDLE) std::cout << "Wrong";
		//获取信息
		//支持的格式
		auto formats = phyDevice.getSurfaceFormatsKHR(surface);
		m_SwapChainInfo.format = formats[0];
		for (const auto& format : formats) {
			//查找是否支持我们想要的格式
			//srgb是指非线性空间
			//不需要手动gamma校正和解压
			if (format.format == vk::Format::eR8G8B8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				m_SwapChainInfo.format = format;
				break;
			}
		}
		//支持的容量
		auto capabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
		//缓冲数量
		m_SwapChainInfo.imageCount = std::clamp<uint32_t>(2, capabilities.minImageCount, capabilities.maxImageCount);
		m_SwapChainInfo.imageExtent.width = std::clamp<uint32_t>(w, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		m_SwapChainInfo.imageExtent.height = std::clamp<uint32_t>(h, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		m_SwapChainInfo.transform = capabilities.currentTransform;

		//显示模式
		auto presents = phyDevice.getSurfacePresentModesKHR(surface);
		m_SwapChainInfo.present = presents[0];
		for (const auto& present : presents) {
			//看看显示模式中支不支持最好的
			if (present == vk::PresentModeKHR::eMailbox) {
				m_SwapChainInfo.present = present;
				break;
			}
		}

	}
	void SwapChain::GetImages()
	{
		m_Images = Context::GetInstance().GetDevice().getSwapchainImagesKHR(m_SwapChain);
	}

	void SwapChain::SetImageViews()
	{
		m_ImageViews.resize(m_Images.size());
		//SetImageViews
		for (int i = 0; i < m_Images.size(); i++) {
			vk::ImageViewCreateInfo createInfo{};
			//设置映射关系
			vk::ComponentMapping mapping;
			vk::ImageSubresourceRange range;

			range.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1)
				.setAspectMask(vk::ImageAspectFlagBits::eColor);
			createInfo.setImage(m_Images[i])
				.setViewType(vk::ImageViewType::e2D)
				.setComponents(mapping)
				.setFormat(m_SwapChainInfo.format.format)
				.setSubresourceRange(range);
			m_ImageViews[i] = Context::GetInstance().GetDevice().createImageView(createInfo);
		}
	}

}