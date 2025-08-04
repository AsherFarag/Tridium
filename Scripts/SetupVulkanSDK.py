import os

VulkanSDK = os.environ.get('VULKAN_SDK')
RequiredVulkanVersion = '1.3'

# Check if the Vulkan SDK is installed and 
def ValidateVulkanSDK():
    if VulkanSDK is None:
        print("Vulkan SDK is not installed")
        return False
    
    if RequiredVulkanVersion not in VulkanSDK:
        print(f"Vulkan SDK version {RequiredVulkanVersion} is required, but found: {VulkanSDK}")
        return False

    print("Vulkan SDK is valid.")
    return True

if __name__ == "__main__":
    ValidateVulkanSDK()