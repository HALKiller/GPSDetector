import os
import shutil
from datetime import datetime

def create_backup():
    """
    Copies specified folders to a new backup directory.

    The backup directory is created one level up from the script's location,
    inside a 'BUP' folder, and is named with the current date and time.
    """
    # Get the current directory where the script resides
    current_script_dir = os.path.dirname(os.path.abspath(__file__))
    print(f"Current script directory: {current_script_dir}")

    # Define the folders to be copied (relative to the current_script_dir)
    folders_to_copy = ["src", "include"]

    # Construct the parent directory (one level up from current_script_dir)
    parent_dir = os.path.abspath(os.path.join(current_script_dir, os.pardir))
    print(f"Parent directory: {parent_dir}")

    # Define the base backup directory (BUP folder)
    bup_dir = os.path.join(parent_dir, "BUP")

    # Create a timestamp for the backup folder name (e.g., 1306_08072025)
    # Using 'DDMMYYYY_HHMMSS' format for uniqueness and sorting
    timestamp = datetime.now().strftime("%H%M_%d%m%Y")
    destination_folder_name = timestamp
    destination_path = os.path.join(bup_dir, destination_folder_name)

    print(f"Destination backup path: {destination_path}")

    try:
        # Create the destination directory if it does not exist
        os.makedirs(destination_path, exist_ok=True)
        print(f"Created backup directory: {destination_path}")

        # Copy each specified folder
        for folder_name in folders_to_copy:
            source_path = os.path.join(current_script_dir, folder_name)
            target_path = os.path.join(destination_path, folder_name)

            if os.path.exists(source_path):
                print(f"Copying '{source_path}' to '{target_path}'...")
                # shutil.copytree copies the entire directory tree
                shutil.copytree(source_path, target_path)
                print(f"Successfully copied '{folder_name}'.")
            else:
                print(f"Warning: Source folder '{source_path}' does not exist. Skipping.")

        print("\nBackup process completed successfully!")

    except Exception as e:
        print(f"\nAn error occurred during the backup process: {e}")
        # Clean up the partially created directory if an error occurs
        if os.path.exists(destination_path) and not os.listdir(destination_path):
            os.rmdir(destination_path)
            print(f"Cleaned up empty destination directory: {destination_path}")

# Run the backup function when the script is executed
if __name__ == "__main__":
    create_backup()
