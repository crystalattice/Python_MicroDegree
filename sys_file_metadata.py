from datetime import datetime
import shutil

from pathlib import Path

# Make temporary directory
tmp_dir = Path("/home/cody/tmp")
try:
    tmp_dir.mkdir()
except FileExistsError:
    print("Temp dir already exists")

# Copy file to temp dir
shutil.copy("malicious_file.txt", "/home/cody/tmp")
if not Path("/home/cody/tmp/malicious_file.txt").exists():
    print("File did not copy")


working_file = Path("/home/cody/tmp/malicious_file.txt")  # Path for remaining commands

working_file.chmod(0o444)  # Change file mode
file_mode = oct(working_file.stat().st_mode)  # Convert bits to octal
print(file_mode[-3:])  # Get last three numbers (normal file mode view)

# If file mode not 777, change mode
if file_mode[-3:] != 777:
    working_file.chmod(0o777)
    print(oct(working_file.stat().st_mode)[-3:])  # Confirm change

print(working_file.name)  # Confirm file name

# Check if path is file
if working_file.is_dir():
    print("Path is a directory")
elif working_file.is_symlink():
    print("Path is a symlink")
elif working_file.is_file():
    print("Path is a file")
else:
    print("Path is something else")

# Get file size
size = working_file.stat().st_size
if size < 1024:
    print(f"{size} bytes")
elif size < 1048576:
    print(f"{size / 1024:.2f} KB")
else:
    print(f"{size / 1048576:.2f} MB")

print(working_file.owner())  # Check owner
print(working_file.group())  # Check group

print(datetime.fromtimestamp(working_file.stat().st_atime))  # Most recent access time
print(datetime.fromtimestamp(working_file.stat().st_mtime))  # Most recent modification
print(datetime.fromtimestamp(working_file.stat().st_ctime))  # Most recent metadata change (Unix); creation time (Windows)