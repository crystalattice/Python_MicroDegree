import subprocess

print(subprocess.run("ls"))  # Simple Bash command call
print("*" * 10)

print(subprocess.run(["ls", "-l", "/dev/null"]))  # Bash command w/ parameters
print("*" * 10)

ls_result = subprocess.run(["ls", "-l", "/dev/null"], capture_output=True)  # Bash command w/ arguments
print(ls_result)
