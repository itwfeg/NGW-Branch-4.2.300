# @file oo_settings_restore_script.py
# @author Tony Shelton-McGaha
# posts  online ordering settings data from json file

import requests
import sqlite3
import json
import sys

from requests.exceptions import ConnectionError

def run_script():
	
	bb_address = sys.argv[1]
	file_path = sys.argv[2]

	try:
		f = open(file_path)
	except:
		print("failure")
		sys.exit(-1)

	json_data = json.load(f)

	url = "http://%s/api/v1/version" % (bb_address)
	req = requests.get( url, timeout=(1,30))

	if req.status_code != 200:
		print( "failure" )
		sys.exit(-1)

	try:
		version = req.json()
	except:
		print("failure")
		sys.exit(-1)

	if not "deviceType" in version:
		print("failure" )
		sys.exit(-1)

	url = "http://%s/api/v1/settings" % (bb_address)

	headers = {"Content-Type":"application/json"}
	
	response = requests.post(url, data=json.dumps(json_data), headers=headers)
	
	response.json()

	print("success")

if __name__ == "__main__":
	
	try: 
		run_script()
	except:
		print("failure")
