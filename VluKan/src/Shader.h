#pragma once
#include "vulkan/vulkan.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace VulKan {
	//vulkan的shader只能支持spir-v的格式
	class Shader
	{
	public:
		static void Init(const std::string& vetextSrc, const std::string& fragSrc);
		static Shader& GetIns() { return *Ins; };
		static void Quit(){ Ins.reset(nullptr); }
		void initSatge();
		const std::vector<vk::PipelineShaderStageCreateInfo>& GetStage()const {return m_stage_;}
		~Shader();
	private:
		//通过路径进行初始化
		Shader(const std::string& vertexSrc, const std::string& fragSrc);
	private:
		inline static std::unique_ptr<Shader> Ins = nullptr;

		//提供给pipeline的stage说明
		//Module 是“磁盘上的文件内容”（静态数据），而 Stage 是“流水线上的工位”（动态任务）[类似ImageView和Image之间的关系]
		std::vector<vk::PipelineShaderStageCreateInfo> m_stage_;//无需手动销毁
		std::unordered_map<std::string, vk::ShaderModule> m_Shader{ {"Vertex",{}},{"Frag",{}} };
	};
}


