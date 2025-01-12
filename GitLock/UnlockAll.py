import os
import subprocess
from pathlib import Path

def get_git_root(path):
    """Get the root directory of the git repository."""
    try:
        result = subprocess.check_output(['git', '-C', path, 'rev-parse', '--show-toplevel'],
                                         stderr=subprocess.STDOUT).decode().strip()
        return Path(result)
    except subprocess.CalledProcessError as e:
        print("Error: Not a git repository or unable to fetch root.")
        print(e.output.decode())
        return None

def is_locked(file_path):
    """Check if the file is locked (read-only)."""
    # Check if the file is read-only (locked)
    if os.name == 'nt':  # For Windows
        return not os.access(file_path, os.W_OK)
    else:  # For Unix-like systems
        return oct(file_path.stat().st_mode)[-3:] == '444'

def unlock_file(file_path):
    """Unlock the file by changing its permissions."""
    try:
        if os.name == 'nt':  # For Windows
            os.chmod(file_path, 0o666)  # Make it writable (rw-rw-rw)
        else:  # For Unix-like systems
            os.chmod(file_path, 0o666)  # Make it writable (rw-rw-rw)
        print(f"[DEBUG] Unlocked file: {file_path}")
    except Exception as e:
        print(f"[DEBUG] Error unlocking file '{file_path}': {e}")

def unlock_files_in_folder(folder_path):
    """Unlock all files within the specified folder."""
    # Ensure the folder path is a Path object
    folder_path = Path(folder_path).resolve()

    # Iterate through all files in the Content folder
    for file_path in folder_path.rglob('*'):  # rglob will iterate over all subfolders and files
        if file_path.is_file():  # Ensure it's a file (not a directory)
            if is_locked(file_path):
                unlock_file(file_path)

def main():
    current_dir = Path(__file__).resolve().parent
    git_root = get_git_root(current_dir)
    
    if not git_root:
        print("Unable to find the git root for the specified path.")
        return
    
    content_folder = git_root / "Content"
    
    if not content_folder.exists():
        print(f"Error: 'Content' folder not found at {content_folder}")
        return
    
    unlock_files_in_folder(content_folder)
    print("All locked files in the Content folder have been unlocked.")

if __name__ == "__main__":
    main()