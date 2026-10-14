#!/usr/bin/env python3

# @file online_orders_script.py
# @author Tony Shelton-McGaha
# interacts with blackbox, retrieves online orders in "waiting" status. Returns command string

import sys
import requests
import json
import sqlite3
import os
import configInfo
from contextlib import closing
from datetime import datetime
from datetime import timedelta
from requests.exceptions import ConnectionError

def draw_corner(fg_color, bg_color, value):
    print("oval (100,0) 40 40 " + bg_color)
    print("oval (100,0) 38 38 " + fg_color)
    print("text (86,14) 18 #ffffff " + value)

def show_error(text=""):
    draw_corner("#dc3545", "#bf2e3c", "ERR")
    
    if text:
        print("text (50,80) 15 #ff0000 " + text)
    
def get_config(config):
    try:
        with open("/etc/hobart/defaults_linux.ini") as f:
            db_path = [x for x in f if "db.SQLite.path=" in x][0]

            db_path = configInfo.getRootFolder() + os.path.join(os.sep, "db", "scale", "dbfiles", db_path.split("/")[-1]).strip()
            db_conn = sqlite3.connect(db_path)
            db_conn.row_factory = sqlite3.Row
            
            with closing(db_conn.cursor()) as cursor:
                sql = 'select CoValue from Configs where CoName = "%s"' % config
                cursor.execute(sql)
                return cursor.fetchall()[0][0]
    except:
        show_error("Scale Configs")
        sys.exit(0)

def run_script(sound):
    bb_address = get_config("onlineOrderingServer")
    dept = get_config("mostRecentActiveDeptNum")
    zone = get_config("onlineOrderingZone")
    url = "http://%s/api/v1/orderparts?PartStatus.gte=1&PartStatus.lte=2&OrderPickup.neq=null&DeptNum=%s&ZoneNum=%s" % (bb_address, dept, zone)

    try:
        req = requests.get(url,timeout=(5, 5))
    except requests.exceptions.RequestException:
        show_error("Orders Conn")
        return
    
    orders = []
    
    if req.status_code != 200:
        show_error("Orders Response")
        return
    
    try:
        orders = req.json()
    except:
        show_error("Orders JSON")
        return
    
    display_count = str(len(orders)) if len(orders) < 100 else "99+"
    
    cmd = ""
    
    settings_url = "http://%s/api/v1/settings" % (bb_address)
    
    try:
        settings_req = requests.get(settings_url, timeout=(5, 5))
    except requests.exceptions.RequestException:
        show_error("Settings Conn")
        return
    
    if settings_req.status_code != 200:
        show_error("Settings Response")
        return
    
    try:
        settings = settings_req.json()
    except:
        show_error("Settings JSON")
        return
    
    if not isinstance(settings, list):
        show_error("Settings Data")
        return
    
    if len(settings) < 1 and "WarningTime" not in settings[0]:
        show_error("Settings Data")
        return
    
    pickup_soon = 0
    overdue = 0
    audible_alert = False
    
    if not isinstance(orders, list):
        show_error("Orders Data")
        return
    
    for order in orders:
        if not isinstance(order, dict):
            show_error("Order Data")
            return
        
        if "OrderPickup" not in order:
            show_error("Order Data")
            return
            
        order_pickup = datetime.strptime( order['OrderPickup'], "%Y-%m-%d %H:%M:%S" )
        pickup_warning_time = order_pickup - timedelta( minutes = settings[0]['WarningTime'] )
        if datetime.now() >= pickup_warning_time:
            pickup_soon += 1
        if datetime.now() >= order_pickup:
            overdue += 1

    if overdue > 0:
        draw_corner("#dc3545", "#bf2e3c", display_count)
        print("flash 250 0")
        audible_alert = True
    elif pickup_soon > 0:
        draw_corner("#ffc107", "#e6ae07", display_count)
        print("flash 250 0")
        audible_alert = True
    else:
        draw_corner("#007bff", "#006fe6", display_count)
        print("flash 500 1")
        
    if sound and audible_alert:
        os.system("aplay --duration 1 /usr/local/hobart/system/audio/online-ordering-beep.wav") 

if __name__ == "__main__":
    try:
        run_script(sound=True)
    except:
        show_error()

