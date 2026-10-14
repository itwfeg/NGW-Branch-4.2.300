# @file now_serving_fk_script.py
# @author Tony Shelton-McGaha
# interacts with blackbox, retrieves Now Serving queue size. Returns command strings

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

def get_config(config):
    with open("/etc/hobart/defaults_linux.ini") as f:
        db_path = [x for x in f if "db.SQLite.path=" in x][0]

        db_path = configInfo.getRootFolder() + os.path.join(os.sep, "db", "scale", "dbfiles", db_path.split("/")[-1]).strip()
        db_conn = sqlite3.connect(db_path)
        db_conn.row_factory = sqlite3.Row

        with closing(db_conn.cursor()) as cursor:
            sql = 'select CoValue from Configs where CoName = "%s"' % config
            cursor.execute(sql)
            return cursor.fetchall()[0][0]

def run_script():
	bb_address = get_config("onlineOrderingServer")
	url = "http://%s/api/v1/queue/%s" % (bb_address, get_config("mostRecentActiveDeptNum"))
	req = requests.get( url )

	if req.status_code != 200:
		print( "Error: The queue size URL provided returned a status code of " + str(req.status_code) )
		sys.exit(-1)
		
	queue_size = req.json()["QueueLength"]

	# red square with white number in Times New Roman font at top left of flashkey
	cmd = "font Times New Roman\n"
	draw_corner("#007bff", "#006fe6", str(queue_size))
	overdue_orders = 0

	settings_url = "http://%s/api/v1/settings" % (bb_address)
	settings_req = requests.get( settings_url )
	if settings_req.status_code != 200:
		print( "Error: The settings URL provided returned a status code of " + str(settings_req.status_code) )
		sys.exit(-1)
	settings = settings_req.json()  

	if queue_size == 0:
		print( "no-op" )
	elif queue_size != 0 and settings[0]['NewNowServing'] != 1:
		print( cmd )
	elif queue_size != 0 and settings[0]['NewNowServing'] == 1:
		orders_url = "http://%s/api/v1/orders?OrderOrigin=3&OrderStatus=1" % (bb_address)
		orders_req = requests.get( orders_url )
		
		if orders_req.status_code != 200:
			print( "Error: The orders URL provided returned a status code of " + str(orders_req.status_code) )
			sys.exit(-1)
		orders = orders_req.json()
		
		for order in orders:
			order_creation = datetime.strptime( order['OrderCreation'], "%Y-%m-%d %H:%M:%S" )
			order_complete_time_limit = order_creation + timedelta( minutes = settings[0]['NowServingWaitTime'] )
			if datetime.now() >= order_complete_time_limit:
				overdue_orders += 1
				
		if overdue_orders > 0:
			print( "flash 500 0\n" + cmd )
		else:
			print( cmd )
		
if __name__ == "__main__":
    try:
    	run_script()
    except ConnectionError:
    	print( "Error: Unable to connect using specified IP address." )
    	sys.exit(-1)

