import os
import subprocess
from pathlib import Path
import time

def normalize_path(path_str):
    """Normalize paths to ensure consistent forward slashes for internal usage."""
    return path_str.replace("\\", "/")

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

def get_git_user():
    """Retrieve the Git username."""
    try:
        result = subprocess.check_output(['git', 'config', '--get', 'user.name'],
                                         stderr=subprocess.STDOUT).decode().strip()
        print(f"Git user: {result}")
        return result
    except subprocess.CalledProcessError:
        return "Unknown"

def get_modified_files(git_root):
    """Get the list of modified files in the Git repository."""
    try:
        output = subprocess.check_output(['git', '-C', git_root, 'status', '--porcelain'],
                                         stderr=subprocess.STDOUT).decode().strip()
        modified_files = []
        doOnce = False
        for line in output.splitlines():
            status = line[:2].strip()  # Get the first two characters and strip spaces
            if status.startswith('M'):  # Handle modified, added, renamed, etc.
                relative_path = normalize_path(line[3:].strip())  # Normalize path
                if doOnce == False:
                    relative_path = "C" + relative_path
                    doOnce = True
                full_path = (Path(git_root) / relative_path).resolve()
                modified_files.append(full_path)
        return modified_files
    except subprocess.CalledProcessError:
        return []

def commit_and_push_changes(file_path, git_root):
    """Commit and push the user's control file to Git."""
    try:
        subprocess.run(['git', '-C', git_root, 'add', str(file_path)], check=True)
        subprocess.run(['git', '-C', git_root, 'commit', '-m', f"[UPD] Update lock file for {file_path.name}"], check=True)
        subprocess.run(['git', '-C', git_root, 'push'], check=True)
        print(f"[DEBUG] Successfully committed and pushed changes for {file_path}.")
    except subprocess.CalledProcessError as e:
        print(f"[DEBUG] Git error: {e}")

def update_lockFile(git_root, unreal_project_path, control_dir, user_name):
    """Locks files listed in the control files using Git."""
    user_control_file = control_dir / f"{user_name}_control.txt"

    modified_files = get_modified_files(git_root)
    updated_user_locked_files = {}

    # Ensure Content folder is included when processing paths
    content_folder = Path(unreal_project_path)

    # Manually fix the relative path for modified files
    for modified_file in modified_files:
        # Fix the path and ensure it starts with "Content"
        rel_path = str(modified_file.relative_to(content_folder))
        
        if rel_path.startswith("Content") and rel_path not in updated_user_locked_files:
            updated_user_locked_files[rel_path] = user_name

    # Write the updated lock list back to the user's control file
    with open(user_control_file, 'w') as file:
        for rel_path, owner in updated_user_locked_files.items():
            file.write(f"{rel_path},{owner}\n")

    commit_and_push_changes(user_control_file, git_root)

def lock_all(git_root, unreal_project_path, control_dir, user_name):
    """Locks files listed in the control files using Git."""
    user_control_file = control_dir / f"{user_name}_control.txt"
    all_control_files = list(control_dir.glob("*_control.txt"))

    # Lock files based on all control files
    for control_file in all_control_files:
        if (control_file != user_control_file):
            with open(control_file, 'r') as file:
                for line in file:
                    if line.strip():
                        rel_path, owner = line.split(',', 1)
                        full_path = normalize_path(str((Path(unreal_project_path) / rel_path.strip()).resolve()))
                        try:
                            os.chmod(full_path, 0o444)  # Lock the file for others
                            print(f"[DEBUG] Locked file : {rel_path}")
                        except Exception as e:
                            print(f"[DEBUG] Error locking file '{rel_path}': {e}")

def end_unlock_all(git_root, unreal_project_path, control_dir, user_name):
    """Locks files listed in the control files using Git."""
    user_control_file = control_dir / f"{user_name}_control.txt"
    all_control_files = list(control_dir.glob("*_control.txt"))

    # Unlock all files first
    for control_file in all_control_files:
        with open(control_file, 'r') as file:
            for line in file:
                if line.strip():
                    rel_path, _ = line.split(',', 1)
                    full_path = normalize_path((Path(unreal_project_path) / rel_path.strip()).resolve())
                    try:
                        os.chmod(full_path, 0o666)  # Unlock the file
                        print(f"[DEBUG] Unlocked file: {rel_path}")
                    except Exception as e:
                        print(f"[DEBUG] Error unlocking file '{rel_path}': {e}")

def run_continuous_check(script_path):
    """Runs the script continuously checking for file modifications."""
    script_dir = Path(script_path).resolve().parent
    git_root = get_git_root(script_dir)
    if not git_root:
        print("Unable to find the git root for the specified path.")
        return

    # Paths
    control_dir = script_dir / "control_files"
    control_dir.mkdir(exist_ok=True)
    unreal_project_path = git_root  # Assuming Unreal project path matches git root

    # Get Git user
    user_name = get_git_user()

    lock_all(git_root, unreal_project_path, control_dir, user_name)

    while True:
        try:
            update_lockFile(git_root, unreal_project_path, control_dir, user_name)
            time.sleep(5)  # Sleep before next check
        except KeyboardInterrupt:
            end_unlock_all(git_root, unreal_project_path, control_dir, user_name)
            break
        except Exception as e:
            #end_unlock_all(git_root, unreal_project_path, control_dir, user_name)
            break

if __name__ == "__main__":
    main_script_path = os.path.abspath(__file__)
    run_continuous_check(main_script_path)