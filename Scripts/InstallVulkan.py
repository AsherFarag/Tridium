import os
from pathlib import Path

import Utils

VulkanInstallVersion = '1.4.321.1'
VulkanSDKInstallerURL = f'https://sdk.lunarg.com/sdk/download/{VulkanInstallVersion}/windows/vulkansdk-windows-X64-{VulkanInstallVersion}.exe'
VulkanSDKLocalPath = '../Tridium/Dependencies/VulkanSDK'
VulkanSDKExePath = f'{VulkanSDKLocalPath}/VulkanSDK.exe'

def InstallVulkanSDK():
    Path(VulkanSDKLocalPath).mkdir(parents=True, exist_ok=True)

    print("Downloading Vulkan SDK installer...")
    try:
        Utils.DownloadFile(VulkanSDKInstallerURL, VulkanSDKExePath)
    except Exception as e:
        print(f"Failed to download Vulkan SDK installer: {e}")
        return False

    if not os.path.isfile(VulkanSDKExePath):
        print("Downloaded installer not found. Aborting.")
        return False

    print("Successfully downloaded to", VulkanSDKExePath)

    print("Validating installer file size...")
    FileSize = os.path.getsize(VulkanSDKExePath)
    if FileSize < 1024 * 1024:  # Less than 1MB is suspicious
        print("Installer file size is too small. Possible download error.")
        return False

    print("Running Vulkan SDK installer...")
    try:
        os.startfile(os.path.abspath(VulkanSDKExePath))
    except Exception as e:
        print(f"Failed to run Vulkan SDK installer: {e}")
        return False

    print("Please run SetupVulkanSDK.bat to complete the installation.")
    return True


if __name__ == "__main__":
    InstallVulkanSDK()