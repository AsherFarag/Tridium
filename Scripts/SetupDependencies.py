import subprocess
import Utils
import SetupVulkanSDK as VulkanSetup
import InstallVulkan

def SetupDependencies():
    print("Setting up dependencies...")

    # Initialize and update Git submodules
    try:
        print("Initializing Git submodules...")
        subprocess.check_call(["git", "submodule", "init"])
        subprocess.check_call(["git", "submodule", "update", "--init", "--recursive"])
        print("Git submodules initialized.")
    except subprocess.CalledProcessError as e:
        print(f"Failed to initialize Git submodules: {e}")
        return False

    # Check Vulkan SDK
    if not VulkanSetup.ValidateVulkanSDK():
        print("Vulkan SDK validation failed.")
        if Utils.AskYesOrNo("Do you want to install the Vulkan SDK?"):
            if not InstallVulkan.InstallVulkanSDK():
                print("Failed to install Vulkan SDK.")
                return False
        return False

    print("Dependencies setup complete.")
    return True
