#include "tripch.h"
#include "RHI_VulkanImpl.h"
#include <glfw/glfw3.h>

namespace Tridium::Vulkan {

    bool DynamicRHI_VulkanImpl::Init( const RHIConfig& a_Config )
    {
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = nullptr;
		appInfo.applicationVersion = TRIDIUM_VERSION;
		appInfo.pEngineName = "Tridium";
		appInfo.engineVersion = TRIDIUM_VERSION;
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		TODO( "This info should be obtainable from a platform module without glfw needed to be explicitely used" );
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

		Array<const char*> requiredExtensions{};
		for ( uint32_t i = 0; i < glfwExtensionCount; ++i )
		{
			requiredExtensions.PushBack( glfwExtensions[ i ] );
		}

		createInfo.enabledExtensionCount = Cast<uint32_t>( requiredExtensions.Size() );
		createInfo.ppEnabledExtensionNames = requiredExtensions.Data();

		VkResult result = vkCreateInstance( &createInfo, nullptr, &m_Instance );

		if ( !ASSERT( result == VK_SUCCESS
		    , "Failed to create Vulkan instance! Error: ", GetVulkanErrorString( result ) ) )
		{
			return false;
		}

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );

		Array<VkExtensionProperties> extensions( extensionCount );
		vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, extensions.Data() );
		std::cout << "Available extensions:\n";
		for ( const auto& extension : extensions )
		{
			std::cout << '\t' << extension.extensionName << '\n';
		}

        return true;
    }

	bool DynamicRHI_VulkanImpl::Shutdown()
	{
		if ( m_Instance != VK_NULL_HANDLE )
		{
			vkDestroyInstance( m_Instance, nullptr );
			m_Instance = VK_NULL_HANDLE;
		}

		return true;
	}

}
