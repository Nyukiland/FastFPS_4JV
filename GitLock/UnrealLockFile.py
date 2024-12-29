import os
import subprocess
from pathlib import Path
import time

def get_git_root(path):
    """Get the root directory of the git repository."""
    try:
        result = subprocess.check_output(['git', '-C', path, 'rev-parse', '--show-toplevel'], 
                                         stderr=subprocess.STDOUT).decode().strip()
        return result
    except subprocess.CalledProcessError as e:
        print(f"Error: Not a git repository or unable to fetch root.")
        print(e.output.decode())
        return None

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

def manage_asset_control_files(git_root, modified_files, script_folder):
    """Create or remove control files for modified assets."""
    current_user = "Unknown"  # Optionally use 'git config user.name' to fetch the user's name
    asset_control_folder = Path(script_folder) / "asset_locks"

    # Ensure the control folder exists
    asset_control_folder.mkdir(exist_ok=True)

    # Maintain the lock files for modified assets
    for modified_file in modified_files:
        asset_relative_path = os.path.relpath(modified_file, git_root)
        control_file_path = asset_control_folder / f"{asset_relative_path.replace('/', '_')}.lock"

        # Create or update the control file
        with open(control_file_path, 'w') as control_file:
            control_file.write(f"{asset_relative_path},{current_user}\n")
            print(f"Created lock file: {control_file_path}")

    # Clean up stale control files (no longer modified assets)
    for control_file in asset_control_folder.iterdir():
        if control_file.suffix == '.lock':
            asset_relative_path = control_file.stem.replace('_', '/')
            if os.path.join(git_root, asset_relative_path) not in modified_files:
                try:
                    control_file.unlink()  # Remove the file
                    print(f"Removed stale lock file: {control_file}")
                except Exception as e:
                    print(f"Error removing lock file '{control_file}': {e}")

def commit_and_push_changes(git_root, control_file_path, message):
    """Commit and push only the control file to Git."""
    try:
        subprocess.run(['git', '-C', git_root, 'add', str(control_file_path)], check=True)
        subprocess.run(['git', '-C', git_root, 'commit', '-m', message], check=True)
        subprocess.run(['git', '-C', git_root, 'push'], check=True)
        print("Successfully committed and pushed changes for the control file.")
    except subprocess.CalledProcessError as e:
        print(f"Git error: {e}")

def run_continuous_check(script_path):
    """Runs the script continuously checking for file modifications."""
    git_root = get_git_root(script_path)
    if not git_root:
        print("Unable to find the git root for the specified path.")
        return

    print("Starting continuous check for file modifications...")

    while True:
        try:
            modified_files = get_modified_files(git_root)
            if modified_files:
                manage_asset_control_files(git_root, modified_files, Path(script_path).parent)
            time.sleep(10)
        except Exception as e:
            print(f"Error in continuous check: {e}")
            break

def main():
    """Main entry point for running the script."""
    script_path = os.path.abspath(__file__)  # Get the absolute path of the script
    run_continuous_check(script_path)

if __name__ == "__main__":
    main()