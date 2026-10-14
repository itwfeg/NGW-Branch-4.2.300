import sys
import shellCommand
import setPassword

def createSafewayUser():
    errorLog = open( "/var/log/safeway-log3.txt",  "wb")

    #check for existance of an administrator user
    cmd = []
    cmd.append("id")
    cmd.append("administrator")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    errorLog.write("id administrator: " + str(exitCode) + "\n")
    if exitCode != 0:

        #create the administrator user
        cmd = []
        cmd.append("useradd")

        # Comment
        cmd.append("-c")
        cmd.append('"Administrator"')
        cmd.append("administrator")

        # Home directory
        cmd.append("-d")
        cmd.append("/home/Hobart")

        # Create home directory if it does not exist.  Do we need to do this here?
        #cmd.append("-m")

        # Skeleton directory
        #cmd.append("-k")
        #cmd.append("/etc/skel")

        # User login shell name
        cmd.append("-s")
        cmd.append("/bin/sh")

        # UID
        cmd.append("-u")
        cmd.append("1003")  # Is 1003 in Windux

        # Group ID
        cmd.append("-g")
        cmd.append("root")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        errorLog.write("administrator user: " + str(exitCode) + "\n")

        # Set the administrator password
        exitCode = setPassword.setPassword("administrator", "Hobart45374")
        errorLog.write("set administrator password: " + str(exitCode) + "\n")

if __name__ == "__main__":
    createSafewayUser()
    sys.exit(0)
