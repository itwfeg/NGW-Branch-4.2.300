import sys, os, string

def syncHostname(hostname):
    exitCode = 0
    if os.name == 'posix':
        import shellCommand
        # LINUX OS
        try:

            cmd = []
            cmd.append("/usr/bin/hostname")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            print((str(stdout.strip())))

            hostname = stdout.rstrip()
            # now update /etc/hostname for permanent change to take place
            hostnameFileHandle = open("/etc/hostname", 'w')
            hostnameFileHandle.write(hostname+ "\n")
            hostnameFileHandle.close()
            cmd = []
            cmd.append("hostname")
            cmd.append("-F")
            cmd.append('/etc/hostname')
            exitCode, stdout, stderr = shellCommand.execute(cmd)

        except:
            exitCode = 1
    return exitCode, hostname
            

if __name__ == "__main__":

    exitCode, hostname  = syncHostname("")
    #print(str(hostname.strip()))
    sys.exit(exitCode)
