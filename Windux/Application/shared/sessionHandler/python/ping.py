# what is the point of this file? doesn't output anything or change any global
# state or anything.

import sys
import os
import shellCommand
import argparse

if os.name == 'posix':
    WAIT_TIMEOUT = 1
else:
    WAIT_TIMEOUT = 1000

def pingSystem(host,  count,  wait=WAIT_TIMEOUT):
    cmd = []

    if os.name == "posix":
        cmd.append("ping")
        cmd.append("-c")
        cmd.append(count)
        cmd.append("-W")
        # linux wait parameter is in seconds
        cmd.append(str(wait))
        cmd.append(host)
        
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    else:
        # WINDOWS BLOCK GOES HERE
        errorReplies = ('Request timed out.',
                        'Destination host unreachable.',
                        'TTL expired in transit.',
                        )
        cmd.append("ping")
        cmd.append("-n")
        cmd.append(count)
        cmd.append("-w")
        # windows wait parameter is in milliseconds
        cmd.append(str(wait))
        cmd.append(host)

        exitCode, stdout, stderr = shellCommand.execute(cmd)

        #TODO: May need to add this check to linux as well.
        for errorReply in errorReplies:
            if errorReply in stdout or errorReply in stderr:
                exitCode = 1
                break

        # get success or failure and store as exitcode
    #print(str(exitCode))
    return (exitCode,  stdout)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="ping host or ipaddress count number of times")
    parser.add_argument("-c","--count",
                        action="store",
                        type=int,
                        choices=list(range(0,31)),
                        default=3,
                        help="number of times to ping host, default = 3")

    if os.name == 'posix':
        parser.add_argument("-w","--wait",
                            action="store",
                            type=int,
                            default=WAIT_TIMEOUT,
                            help="number of seconds before timeout waiting for reply, default = %d" % WAIT_TIMEOUT)
    else:
        parser.add_argument("-w","--wait",
                            action="store",
                            type=int,
                            default=WAIT_TIMEOUT,
                            help="number of milliseconds before timeout waiting for reply, default = %d" % WAIT_TIMEOUT)

    parser.add_argument("host",
                        action="store",
                        help="hostname or ip address of computing system to ping")

    args = parser.parse_args()

    exitCode, stdout = pingSystem(args.host,  str(args.count),  str(args.wait))

    sys.exit(exitCode)
