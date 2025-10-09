import os
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

    print("Git submodules are up to date.")

    # Check Vulkan SDK
    if not VulkanSetup.ValidateVulkanSDK():
        print("Vulkan SDK validation failed.")
        if not Utils.AskYesOrNo("Do you want to install the Vulkan SDK?"):
            return False

        if not InstallVulkan.InstallVulkanSDK():
            print("Failed to install Vulkan SDK.")
            return False
        
        if not VulkanSetup.ValidateVulkanSDK():
            print("Vulkan SDK validation failed after installation - try installing the Vulkan SDK manually.")
            return False
    
    print("Vulkan SDK is properly set up.")

    # Build dependencies in the Originals folder

    currDir = os.getcwd()
    print("Building dependencies...")
    try:
        os.chdir('Tridium/Originals')
        subprocess.check_call(["BuildAll.bat"], shell=True)
    except subprocess.CalledProcessError as e:
        print(f"Failed to build dependencies: {e}")
        os.chdir(currDir)
        return False
    
    os.chdir(currDir)
    
    print("Dependencies built successfully.")

    print("===================================================")
    print("Dependencies setup complete.")
    print("===================================================")

    return True
