#pragma once
#include "vulkan/vulkan.hpp"


namespace VulKan {



	class SwapChain final
	{
	public:
		SwapChain(uint32_t w,uint32_t h);
		~SwapChain();

		//向显卡询问支持该surface的功能信息
		struct SwapchainInfo {
			//分辨率
			vk::Extent2D imageExtent;
			//图像数量
			uint32_t imageCount;
			//存储的物理格式和布局,以及显示空间的格式hdr..
			vk::SurfaceFormatKHR format;
			

			//支持的变换方式
			vk::SurfaceTransformFlagsKHR transform;
			//选择绘制的方式，一般是mailbox
			vk::PresentModeKHR present;
		};
		//获取显卡支持的信息
		void QueryInfo(uint32_t w, uint32_t h);

		void CreateFrameBuffers(uint32_t w, uint32_t h);
		void GetImages();
		void SetImageViews();

		const std::vector<vk::Framebuffer>& GetFrameBuffer() const { return m_FrameBuffers; }
		const SwapchainInfo& GetInfo()const { return m_SwapChainInfo; }
		 vk::SwapchainKHR& GetVKSwapChain() { return m_SwapChain; }
	private:
		SwapchainInfo m_SwapChainInfo;

		vk::SwapchainKHR m_SwapChain;
		std::vector<vk::Image>  m_Images;
		std::vector<vk::ImageView> m_ImageViews;

		//和Image数量一致
		std::vector<vk::Framebuffer> m_FrameBuffers;
	};


}
