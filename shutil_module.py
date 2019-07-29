import platform
import shutil

from pathlib import Path

print(shutil.which("python"))  # Location of given command

if platform.system() == "Linux":  # OS = Linux
    try:  # Make a temp directory
        tmp_path = Path("/home/cody/tmp")
        tmp_path.mkdir()
    except FileExistsError:  # Ignore if temp dir already exists
        pass

    shutil.copy("/home/cody/PycharmProjects/EC-Council/Python_MicroDegree/shutil_module.py",
                "/home/cody/tmp/new_module.py")  # Copy Python file to temp dir and rename
    shutil.move("/home/cody/tmp/new_module.py",
                "/home/cody/PycharmProjects/EC-Council/Python_MicroDegree/new_module.py")  # Move renamed file back

    test_path = Path("/home/cody/PycharmProjects/EC-Council/Python_MicroDegree/new_module.py")  # Make new path object
    print(test_path.exists())  # Rename file exists?

    test_path.unlink()  # Delete file
    print(test_path.exists())  # Ensure file deleted
else:  # Assume Windows OS
    try:
        tmp_path = Path("C:/tmp")
        tmp_path.mkdir()
    except FileExistsError:
        pass
    shutil.copy("C:/Users/Cody/Desktop/shutil_module.py", "C:/tmp/new_module.py")
    shutil.move("C:/tmp/new_module.py", "C:/Users/Cody/Desktop/new_module.py")

    test_path = Path("C:/Users/Cody/Desktop/new_module.py")
    print(test_path.exists())

    test_path.unlink()
    print(test_path.exists())
