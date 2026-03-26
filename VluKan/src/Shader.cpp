#include "Shader.h"  
#include <string> // Ensure std::string is defined  
#include "Context.h"

namespace VulKan {  
   void Shader::Init(const std::string& vertexSrc, const std::string& fragSrc)  
   {  
       // 对于单例模式只能用New,不能用make_unique  
       Ins.reset(new Shader(vertexSrc, fragSrc));  
   }  

   VulKan::Shader::~Shader()  
   {  
       const auto& device = Context::GetInstance().GetDevice();
       device.destroyShaderModule(m_Shader["Vertex"]);
       device.destroyShaderModule(m_Shader["Frag"]);
   }  

   // Constructor definition  
   Shader::Shader(const std::string& vertexSrc, const std::string& fragSrc) {  
       // Constructor implementation  
       vk::ShaderModuleCreateInfo createInfo;
       createInfo.setCodeSize(vertexSrc.size())
           //spirv是4 字节对齐的
           .setPCode((uint32_t*)vertexSrc.data());
       m_Shader["Vertex"] = Context::GetInstance().GetDevice().createShaderModule(createInfo);
       //Frag
       createInfo.setCodeSize(fragSrc.size())
           //spirv是4 字节对齐的
           .setPCode((uint32_t*)fragSrc.data());
       m_Shader["Frag"] = Context::GetInstance().GetDevice().createShaderModule(createInfo);


       

   }  
}
