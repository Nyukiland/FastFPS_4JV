import os
import subprocess
import threading
import time
from pathlib import Path
import unreal


def get_git_root(path):
    """Get the root directory of the git repository."""
    try:
        return subprocess.check_output(['git', '-C', path, 'rev-parse', '--show-toplevel'],
                                       stderr=subprocess.STDOUT).decode().strip()
    except subprocess.CalledProcessError as e:
        unreal.log_error("Error: Not a git repository or unable to fetch root.")
        unreal.log_error(e.output.decode())
        return None


def get_current_user():
    """Get the current Git user."""
    try:
        return subprocess.check_output(['git', 'config', 'user.name'],
                                       stderr=subprocess.STDOUT).decode().strip()
    except subprocess.CalledProcessError:
        return "Unknown"


def get_modified_files(git_root):
    """Get the list of modified files in the Git repository."""
    try:
        output = subprocess.check_output(['git', '-C', git_root, 'status', '--porcelain'],
                                         stderr=subprocess.STDOUT).decode().strip()
        modified_files = []
        for line in output.splitlines():
            if line.startswith(('M', 'A', 'R', 'C', 'U')):  # Handle modified, added, renamed, etc.
                full_path = os.path.join(git_root, line[3:].strip())
                modified_files.append(full_path)
        return modified_files
    except subprocess.CalledProcessError:
        return []


def commit_and_push_changes(git_root, message):
    """Commit and push changes to Git."""
    try:
        subprocess.run(['git', '-C', git_root, 'add', '.'], check=True)
        subprocess.run(['git', '-C', git_root, 'commit', '-m', message], check=True)
        subprocess.run(['git', '-C', git_root, 'push'], check=True)
        unreal.log("Successfully committed and pushed changes.")
    except subprocess.CalledProcessError as e:
        unreal.log_error(f"Git error: {e}")


def lock_files_with_unreal(git_root, unreal_project_path, control_file, script_path):
    """Locks files listed in the control file using Unreal's API and Git."""
    control_file_path = Path(unreal_project_path) / control_file

    if not control_file_path.exists():
        # Create the control file if it doesn't exist
        control_file_path.touch()

    current_user = get_current_user()
    content_folder = Path(unreal_project_path) / "Content"

    # Read the current control file
    locked_files = {}
    with open(control_file_path, 'r') as file:
        for line in file:
            if line.strip():
                rel_path, owner = line.split(',', 1)
                locked_files[rel_path.strip()] = owner.strip()

    # Update the control file based on Git changes
    modified_files = get_modified_files(git_root)
    updated_locked_files = {}

    for rel_path, owner in locked_files.items():
        full_path = os.path.join(unreal_project_path, rel_path)
        # Keep files locked unless they are no longer modified or owned by the current user
        if owner != current_user or full_path in modified_files:
            updated_locked_files[rel_path] = owner

    # Add newly modified files to the lock list
    for modified_file in modified_files:
        rel_path = os.path.relpath(modified_file, unreal_project_path)
        if rel_path.startswith("Content") and rel_path not in updated_locked_files:
            updated_locked_files[rel_path] = current_user

    # Exclude control file and script from locking
    control_file_rel_path = os.path.relpath(control_file_path, unreal_project_path)
    script_file_rel_path = os.path.relpath(script_path, unreal_project_path)
    updated_locked_files.pop(control_file_rel_path, None)
    updated_locked_files.pop(script_file_rel_path, None)

    # Write the updated lock list back to the control file
    with open(control_file_path, 'w') as file:
        for rel_path, owner in updated_locked_files.items():
            file.write(f"{rel_path},{owner}\n")

    # Push changes to Git
    commit_message = "[UPD] Update control script"  # Replace with your desired commit message
    commit_and_push_changes(git_root, commit_message)

    # Apply locks to files
    for rel_path, owner in updated_locked_files.items():
        full_path = os.path.join(unreal_project_path, rel_path)
        if not os.path.exists(full_path):
            continue
        if owner != current_user:
            try:
                # Set the file as read-only for Windows
                os.chmod(full_path, 0o444)
                unreal.log(f"Locked file for others: {rel_path}")
            except Exception as e:
                unreal.log_error(f"Error locking file '{rel_path}': {e}")
        else:
            try:
                # Set the file as writable for the current user
                os.chmod(full_path, 0o666)
                unreal.log(f"Unlocked file for current user: {rel_path}")
            except Exception as e:
                unreal.log_error(f"Error unlocking file '{rel_path}': {e}")


def run_continuous_check(script_path):
    """Runs the script continuously checking for file modifications."""
    unreal_project_path = unreal.Paths.project_dir()
    control_file = "files_to_lock.txt"
    git_root = get_git_root(unreal_project_path)
    if not git_root:
        unreal.log_error("Unable to find the git root for the specified path.")
        return

    unreal.log("Starting continuous check for file modifications...")

    while True:
        try:
            # Check for file modifications
            modified_files = get_modified_files(git_root)
            if modified_files:
                lock_files_with_unreal(git_root, unreal_project_path, control_file, script_path)
            # Wait for a short interval before checking again
            time.sleep(10)
        except Exception as e:
            unreal.log_error(f"Error in continuous check: {e}")
            break


def main():
    """Main entry point for running the script continuously."""
    # Get the script path using Unreal's utilities
    script_path = unreal.Paths.get_base_filename(__file__) if '__file__' in globals() else __file__
    monitoring_thread = threading.Thread(target=run_continuous_check, args=(script_path,), daemon=True)
    monitoring_thread.start()
    unreal.log("Monitoring thread started.")


if __name__ == "__main__":
    main()