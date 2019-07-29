import os
from pathlib import Path

# Using os library
print(f"Current directory: {os.getcwd()}\n")  # Get current directory
print(f"OS information: {os.uname()}\n")  # Get current OS information
print(f"List current path entries: {os.listdir('.')}\n")  # List the entries in the designated path

try:
    os.mkdir("new_dir")  # Make a new directory
    os.chdir("new_dir")  # Change to new directory
    print(f"Current directory: {os.getcwd()}\n")

    os.chdir("..")
    print(f"Current directory: {os.getcwd()}")
    print(f"List current path entries: {os.listdir('.')}\n")

    os.rename("new_dir", "new_dir2")  # Rename directory
    os.chdir("new_dir2")
    print(f"Current directory: {os.getcwd()}\n")
    os.chdir("..")
    os.rmdir("new_dir2")  # Delete directory

except FileExistsError:
    os.rmdir("new_dir")  # Remove existing directory

print("*" * 10)  # Library output separator

# Using pathlib library
path = Path.cwd()  # Get current directory

try:
    path = path / "another_dir"  # Make new directory
    path.mkdir()
    print(path)

    path.rename("another_dir2")
    print((Path.cwd() / "another_dir2").exists())  # New directory exists?

except FileExistsError:
    path.rmdir()  # Delete "another_dir" if it already exists
