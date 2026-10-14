import argparse
import re
import shlex
import subprocess
import sys
import os

import imx6_sbc_leds


# usage:
# make sure memtester is in /root on scale filesystem
# python run_command_trigger_event


def run_command_until_trigger(command, trigger, log, blink_leds):
    trigger_pattern = re.compile(trigger)

    led_blinker = None

    if blink_leds:
        led_blinker = imx6_sbc_leds.LedControl()
        led_blinker.blink_led(color='red', duration=1, state='on')
        led_blinker.blink_led(color="yellow_green", duration=1, state='on')

    # if blink_leds:
    #     blink_command = "/usr/bin/python /root/imx6_sbc_leds.py"
    #     blink_process = subprocess.Popen(shlex.split(blink_command), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    process = subprocess.Popen(shlex.split(command), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    if log:
        import logging
        # setup log file
        logging.basicConfig(filename=log, filemode='w', level=logging.DEBUG, format='%(asctime)s %(message)s',
                            datefmt='%m/%d/%Y %I:%M:%S %p')
        logging.info("Command executed: %s, pattern stop: %s" % (command, trigger))

    while True:

        if process.poll() is not None:
            break

        output = process.stdout.readline()
        
        if output == '' and process.poll() is not None:
            break
        if output:
            command_output = output.strip()
            if log:
                logging.info(command_output)
            # print command_output
            if re.search(trigger_pattern, command_output):
                # print 'found trigger %s in output, stopping command' % trigger
                process.kill()
                os.system("dmesg >& dmesg.txt")
                if blink_leds:
                    # blink_process.kill()
                    led_blinker.blink_led(color='red', state='off')
                    led_blinker.blink_led(color='yellow_green', state='off')

    if process.poll() is not None:
        os.system("dmesg >& dmesg.txt")
        led_blinker.blink_led(color='red', state='off')
        led_blinker.blink_led(color='yellow_green', state='off')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Run command until pattern found in output stream")
    parser.add_argument("command", type=str, help='command to run')
    parser.add_argument("pattern", type=str, help='pattern to match and stop running command')
    parser.add_argument("--log", type=str, help="output to log file")
    parser.add_argument("--blink", help='blink board leds during test', action='store_true')
    args = parser.parse_args()
    sys.exit(run_command_until_trigger(args.command, args.pattern, args.log, args.blink))
