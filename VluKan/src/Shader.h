#pragma once
#include "vulkan/vulkan.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace VulKan {
	class Shader
	{
	public:
		static void Init(const std::string& vetextSrc, const std::string& fragSrc);
		static Shader& GetIns() { return *Ins; };
		static void Quit(){ Ins.reset(nullptr); }
		~Shader();
	private:
		Shader(const std::string& vertexSrc, const std::string& fragSrc);
		inline static std::unique_ptr<Shader> Ins = nullptr;

	private:
		std::unordered_map<std::string, vk::ShaderModule> m_Shader{ {"Vertex",{}},{"Frag",{}} };
	};
}


