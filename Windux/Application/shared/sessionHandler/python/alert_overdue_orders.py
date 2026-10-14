#!/usr/bin/env python3

# periodically fetches orders and alerts configured number via SMS

import sys
import requests
import json
import time
from datetime import datetime
from datetime import timedelta
from sendSMS import main as sendSMS

# number of seconds to sleep between checking orders
SLEEP_TIME = 60

def ts():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S")

def run(overdue_orders):
    bb_address = "127.0.0.1"
    
    try:
        settings_url = "http://%s/api/v1/settings" % (bb_address)
        settings_req = requests.get(settings_url, timeout=(5, 5))
        
        if settings_req.status_code != 200:
            return
        
        settings = settings_req.json()[0]
        
        if not settings["OverdueSMSAlert"] or not settings["AlertPhone"]:
            return
        
        url = "http://%s/api/v1/orders?OrderStatus.gte=1&OrderStatus.lte=2&OrderPickup.neq=null" % (bb_address)

        req = requests.get(url,timeout=(5, 5))
        
        orders = []
        
        if req.status_code != 200:
            return
            
        orders = req.json()
    except Exception as e:
        print("%s failed to get orders/settings from BB: %s" % (ts(), e))
        return
    
    new_overdue = set()
    
    for order in orders:
        order_pickup = datetime.strptime(order['OrderPickup'], "%Y-%m-%d %H:%M:%S")
        if datetime.now() >= order_pickup:
            new_overdue.add((order['OrderNumber'], order['DeptNum']))
            
    new_overdue = new_overdue - overdue_orders
    
    if len(new_overdue) == 0:
        return
    
    print("%s %s newly overdue orders" % (ts(), len(new_overdue)))
    
    msg_to_send = ""
    
    if len(new_overdue) == 1:
        msg_to_send =  "order %s, dept. %s was not filled on time" % next(iter(new_overdue))
    elif len(new_overdue) > 1:
        msg_to_send = "%s orders were not filled on time" % len(new_overdue)
            
    print("%s sending alert to %s" % (ts(), settings["AlertPhone"]))
    
    if not sendSMS(settings["AlertPhone"], msg_to_send):
        print("%s failed to send alert!" % ts())
    
    overdue_orders |= new_overdue
    
if __name__ == "__main__":
    overdue_orders = set()
    
    while True:
        run(overdue_orders)
        time.sleep(SLEEP_TIME)

