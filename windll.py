import ctypes
import string

from pathlib import Path

WINDOWS_DISK_SUFFIX = ":\\"


def get_windows_disks():
    """ Return disks available on Windows machine"""
    disks = []
    kernel32 = ctypes.WinDLL('kernel32')  # Import Windows kernel DLL
    SEM_FAILCRITICALERRORS = 0x0001  # Don't display critical error message box
    SEM_NOOPENFILEERRORBOX = 0x8000  # Don't display OpenFile error message box
    SEM_FAIL = SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS  # Set error type
    oldmode = ctypes.c_uint()  # Set previous error mode
    kernel32.SetThreadErrorMode(SEM_FAIL, ctypes.byref(oldmode))  # Set Windows error mode

    for s in string.ascii_uppercase:
        n = s + WINDOWS_DISK_SUFFIX  # Set drive letter
        path = Path(n)  # Set Windows path
        if path.exists():
            disks.append(n)  # If Windows path exists, add to list

    kernel32.SetThreadErrorMode(oldmode, ctypes.byref(oldmode))  # Set Windows error mode

    return disks


if __name__ == "__main__":
    print(get_windows_disks())
