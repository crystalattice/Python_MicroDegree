import subprocess

ps = subprocess.Popen(["ps", "-A"], stdout=subprocess.PIPE)
output = subprocess.check_output(["grep", "vmware"], stdin=ps.stdout)
print(output)
