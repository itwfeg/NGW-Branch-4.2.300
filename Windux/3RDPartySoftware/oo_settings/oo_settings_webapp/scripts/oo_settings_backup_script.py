# @file oo_settings_backup_script.py
# @author Tony Shelton-McGaha
# interacts with blackbox, retrieves online ordering settings data, backs up to a JSON file

import sys
import requests
import json
from requests.exceptions import ConnectionError, Timeout

def run_script():
	
	bb_address = sys.argv[1]
	dir_path = sys.argv[2]

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

	url = "http://%s/api/v1/settings.html" % (bb_address)
	req = requests.get( url, timeout=(1, 30) )
	
	settings = req.json()

	with open(dir_path + '/online_ordering_settings.json', 'w') as outfile:
		json.dump(settings, outfile)

	print("success")

if __name__ == "__main__":
	try:
		run_script()
	except ConnectionError:
		print( "failure" )
		sys.exit(-1)

	except Timeout:
		print( "timeout failure" )
		sys.exit(-1)

