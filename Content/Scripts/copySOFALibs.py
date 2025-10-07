import os
import shutil
import sys

# List of subpaths to sync (relative to sofa_path/sofaUE_path)
SUBPATHS = [
    "thirdparty/lib",
    "bin/Release",
]

def sync_folder(src, dst):
    """
    Copy contents of src into dst.
    Overwrites files in dst if they already exist.
    Creates directories as needed.
    """
    for dirpath, _, filenames in os.walk(src):
        rel_path = os.path.relpath(dirpath, src)
        target_dir = os.path.join(dst, rel_path)

        os.makedirs(target_dir, exist_ok=True)

        for filename in filenames:
            src_file = os.path.join(dirpath, filename)
            dst_file = os.path.join(target_dir, filename)
            shutil.copy2(src_file, dst_file)
            print(f"Copied {src_file} -> {dst_file}")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python copySOFALibs.py SOFA_BUILD_PATH SOFAUE5_PLUGIN_PATH Debug_Mode(optional)")
        sys.exit(1)

    sofa_path, sofaUE_path = sys.argv[1], sys.argv[2]
    
    debugMode = False
    if len(sys.argv) == 4:
        debugMode = sys.argv[3]
        
    #sofa_path = "C:\projects\sofa-build\"
    #sofaUE_path = "C:\projects\UE5\SofaIntegration\Plugins\SofaUE5\"

    if not os.path.isdir(sofa_path):
        print(f"Error: {sofa_path} is not a valid directory")
        sys.exit(1)

    #os.makedirs(sofaUE_path, exist_ok=True)
    
    # first copy dll
    if (debugMode):
        srcPath = os.path.join(sofa_path, "bin/Debug/")
        dstPath = os.path.join(sofaUE_path, "Binaries/ThirdParty/SofaUE5Library/Win64/Debug/")
    else:
        srcPath = os.path.join(sofa_path, "bin/Release/")
        dstPath = os.path.join(sofaUE_path, "Binaries/ThirdParty/SofaUE5Library/Win64/Release/")
        
    print(f"Syncing {srcPath} -> {dstPath}")
    
    if not os.path.exists(srcPath):
        print(f"Warning: {srcPath} does not exist, skipping")

    sync_folder(srcPath, dstPath)    


    # copy lib
    if (debugMode):
        srcPath = os.path.join(sofa_path, "lib/Debug/")
        dstPath = os.path.join(sofaUE_path, "Source/ThirdParty/SofaUE5Library/x64/Debug/")
    else:
        srcPath = os.path.join(sofa_path, "lib/Release/")
        dstPath = os.path.join(sofaUE_path, "Source/ThirdParty/SofaUE5Library/x64/Release/")
        
    print(f"Syncing {srcPath} -> {dstPath}")
    
    if not os.path.exists(srcPath):
        print(f"Warning: {srcPath} does not exist, skipping")

    sync_folder(srcPath, dstPath)    


    print("All selected folders synced ✅")
