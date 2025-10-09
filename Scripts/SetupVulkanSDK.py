import os
import re

VulkanSDK = os.environ.get('VULKAN_SDK')
RequiredVulkanVersion = '1.2'

def get_vulkan_sdk_version(file_name):
    # Regular expression to match version numbers (e.g., 1.3.250.0)
    version_pattern = r"\d+\.\d+\.\d+\.\d+"
    match = re.search(version_pattern, file_name)
    if match:
        return match.group(0)
    return "Version not found"

# Check if the Vulkan SDK is installed and 
def ValidateVulkanSDK():
    if VulkanSDK is None:
        print("Vulkan SDK is not installed")
        return False

    if VulkanSDK and not os.path.exists(VulkanSDK):
        print(f"Path {VulkanSDK} does not exist, environment variable is stale.")
        return False

    # Check if the current version is >= RequiredVulkanVersion
    current_version = get_vulkan_sdk_version(VulkanSDK)
    if current_version == "Version not found":
        print("Failed to retrieve Vulkan SDK version")
        return False

    if current_version < RequiredVulkanVersion:
        print(f"Vulkan SDK version {current_version} is less than required {RequiredVulkanVersion}")
        return False

    print(f"Vulkan SDK version {current_version} is installed and valid.")
    return True

if __name__ == "__main__":
    ValidateVulkanSDK()