import argparse
import logging
import shlex
import subprocess
import sys
import time

import imx6_sbc_leds


# usage:
# make sure memtester is in /root on scale filesystem

def run_cyclic_load_command(command, load_time, idle_time, log, blink_leds):
    led_blinker = None

    # setup log file
    logging.basicConfig(filename=log, filemode='w', level=logging.DEBUG, format='%(asctime)s %(message)s',
                        datefmt='%m/%d/%Y %I:%M:%S %p')
    logging.info("Command executed: %s, load_time: %d, idle_time: %d" % (command, load_time, idle_time))

    while True:
        load_processor = True

        if blink_leds:
            led_blinker = imx6_sbc_leds.LedControl()
            led_blinker.blink_led(color='red', duration=1, state='on')
            led_blinker.blink_led(color="yellow_green", duration=1, state='on')

        start_time = time.time()

        process = subprocess.Popen(shlex.split(command), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

        while load_processor:
            output = process.stdout.readline()
            if output == '' and process.poll() is not None:
                break
            if output:
                command_output = output.strip()

                if log:
                    logging.info(command_output)

                print("elapsed time = %f secs" % (time.time() - start_time))
                if (time.time() - start_time) >= float(load_time) * 60.0:
                    print('load time: %f mins exceeded, stopping command after %f mins of execution.' %
                          (load_time, ((time.time() - start_time) / 60.0)))
                    process.kill()
                    if blink_leds:
                        # blink_process.kill()
                        led_blinker.blink_led(color='red', state='off')
                        led_blinker.blink_led(color='yellow_green', state='off')
                    load_processor = False

        rc = process.poll()
        print('going idle for %f mins' % idle_time)
        time.sleep(float(idle_time) * 60.0)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="Run command for [load_time] minutes, sleep [idle_time] mins, then repeat the cycle")
    parser.add_argument("command", type=str, help='command to run')
    parser.add_argument("load_time", type=int, help='time in minutes to load processor with specified command')
    parser.add_argument("idle_time", type=int,
                        help='time in minutes to let processor sit idle, i.e. command not running')
    parser.add_argument("--log", type=str, help="output to log file")
    parser.add_argument("--blink", help='blink board leds during test', action='store_true')
    args = parser.parse_args()
    sys.exit(run_cyclic_load_command(args.command, args.load_time, args.idle_time, args.log, args.blink))
