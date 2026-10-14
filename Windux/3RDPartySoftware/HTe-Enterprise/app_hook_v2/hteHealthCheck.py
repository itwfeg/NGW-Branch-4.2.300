
import argparse
import sendDeviceEventMessage

parser = argparse.ArgumentParser('Queue a health check job to HTe Event Controller web service.')
parser.add_argument("-i","--interval",action="store_true",help="execute an INTERVAL health check")

args = parser.parse_args()
jobType="MANUAL"

if (args.interval):
    jobType="INTERVAL"

# perform hook action
sendDeviceEventMessage.sendMessage(server="localhost", port=6000, message="RTDO" + chr(0x1F) + "HAHealthCheck|" + jobType + chr(0x1E))
