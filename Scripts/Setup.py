from asyncio import subprocess
import SetupDependencies
import sys
import subprocess

def Setup():
    if not SetupDependencies.SetupDependencies():
        print("Failed to set up dependencies.")
        return False

    # If we are on windows, we can generate visual studio project files via the GenerateProjects.bat
    if sys.platform == 'win32':
        print("Generating Visual Studio project files...")
        try:
            subprocess.check_call(["GenerateProjects.bat"])
            print("Visual Studio project files generated successfully.")
        except subprocess.CalledProcessError as e:
            print(f"Failed to generate Visual Studio project files: {e}")
            return False

    return True

if __name__ == "__main__":
    if not Setup():
        print("Setup failed. Please check the output for errors.")
        sys.exit(1)
    else:
        print("Setup completed successfully.")
        sys.exit(0)