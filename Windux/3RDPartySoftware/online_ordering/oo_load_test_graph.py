#!/bin/env python3

# this is a load test for the online ordering system
# it will perform most realistically if there is some order data in the mysql DB

# on our devices, install requirements via:
# MULTIDICT_NO_EXTENSIONS=1 YARL_NO_EXTENSIONS=1 /usr/local/bin/pip3 install aiohttp asyncio plotext

# only tested on a quad BCM board running at 1.2 GHz
# device paths may be different for other boards

# this can be run nicely on the scale (or run this from startScaleApplication):
# DISPLAY=:0 lxterminal --geometry=120x40 --title="Quad Online Ordering Test" --command="python3 /root/oo_load_test_graph.py --beep --load 3" 

import asyncio
import aiohttp
import time
import os
import argparse
import math
import plotext as plt

# beeps if the freq is less than this
ok_freq_threshold = 1000000

urls = ["/OnlineOrdering/default/deligo.html", "/api/v1/orders/?sort=OrderNumber&limit=100", "/OnlineOrdering/default/settings.html", "/api/v1/orderdetails/20", "/OnlineOrdering/static/css/bootstrap.min.css", "/OnlineOrdering/static/images/refresh_icon.svg", "/OnlineOrdering/static/js/deligo.js", "/api/v1/settings/", "/api/v1/orders/?sort=OrderPickup&limit=50&OrderPickup.neq=null"]

async def get(url, session):
    try:
        async with session.get(url=url) as response:
            resp = await response.read()
    except Exception as e:
        print("get failed: %s %s" % (url, e.__class__))

async def run_test(urls):
    async with aiohttp.ClientSession() as session:
        ret = await asyncio.gather(*[get(url, session) for url in urls])

def main_loop(beep, multiplier, test_length,):
    start = time.time()
    loop = asyncio.get_event_loop()
    temps = []
    reqs_count = 0

    while time.time() - start < test_length:
        loop.run_until_complete(run_test(urls*multiplier))
        reqs_count += len(urls) * multiplier

        # if we're running locally, graph temperatures and beep as desired
        if "127.0.0.1" in urls[0]:
            throttled = False
            
            # read the current CPU speed
            with open("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq", 'r') as f:
                freq = int(f.read())
                if freq < ok_freq_threshold:
                    throttled = True
                    
            if throttled and beep:
                os.system("echo 230 > /sys/class/backlight/buzzer/brightness && sleep 0.25 && echo 0 > /sys/class/backlight/buzzer/brightness")
                    
            # read the temperatures
            with open("/sys/devices/virtual/thermal/thermal_zone0/temp", 'r') as f:
                temps.append(int(f.read())/1000.0)
            
            # set the title of the terminal window
            title_str = None
            
            if throttled:
                title_str = "\33]0;throttled ********** " + "{:3.1f}".format(temps[-1]) + " ********** throttled\a"
            else:
                title_str = "\33]0;" + "{:3.1f}".format(temps[-1]) + "\a"
            
            print(title_str, end='', flush=True)
            
            # show the graph of temps
            plt.clp()
            plt.clt()
            plt.scatter(temps, point_marker="*")
            plt.ylim(55,100)
            plt.yticks([55,60,70,80,90,95,100])
            plt.frame(False)
            plt.nocolor()
            plt.axes(False,False)
            plt.show()
        else:
            print(".", end='', flush=True)

    print("average reqs/second: {:3.1f}".format(reqs_count / (time.time() - start)))
            
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="load test tool to target an online ordering server")
    parser.add_argument("--beep", help="beep when CPU is throttled", action="store_true")
    parser.add_argument("--load", help="load level (3 is around the target real-world max load)", type=int, default=3, choices=[1,2,3,4,5,6,7])
    parser.add_argument("--length", help="number of seconds to run test for (otherwise infinite)", type=int, default=math.inf)
    parser.add_argument("--host", help="host to run against (default 127.0.0.1) (temperature checks and beeps only work locally)", type=str, default="127.0.0.1")
    
    args = parser.parse_args()
    
    urls = ["http://" + args.host + url for url in urls]
    
    main_loop(args.beep, args.load, args.length)
