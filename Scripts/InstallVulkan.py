import os
from pathlib import Path
import sys

sys.path.append(os.path.dirname(os.path.abspath(__file__)))
import Utils

VulkanInstallVersion = '1.4.321.1'
VulkanSDKLocalPath = str(Path('..') / 'Tridium' / 'Dependencies' / 'VulkanSDK')
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
    file_size = os.path.getsize(VulkanSDKExePath)
    if file_size < 1024 * 1024:  # Less than 1MB is suspicious
        print("Installer file size is too small. Possible download error.")
        return False

    print("Running Vulkan SDK installer...")
    try:
        if sys.platform.startswith('win'):
            os.startfile(os.path.abspath(VulkanSDKExePath))
        else:
            print("Automatic installer launch is only supported on Windows. Please run the installer manually:")
            print(os.path.abspath(VulkanSDKExePath))
    except Exception as e:
        print(f"Failed to run Vulkan SDK installer: {e}")
        return False

    setup_bat_path = os.path.abspath(os.path.join(VulkanSDKLocalPath, "SetupVulkanSDK.bat"))
    print(f"Please run '{setup_bat_path}' to complete the Vulkan SDK installation.")
    return True


if __name__ == "__main__":
    InstallVulkanSDK()