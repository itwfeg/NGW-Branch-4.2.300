#!/usr/bin/env python3

# fetches the current position of the ISS from a web API
# places a dot at the approprate location over a map of the earth

import json
import requests
import sys

def get_position():
	req = requests.get("http://api.open-notify.org/iss-now.json")

	if (req.status_code != 200):
		print("Failed to get API")
		sys.exit(-1)

	response = req.json()
	return (float(response['iss_position']['longitude']),float(response['iss_position']['latitude']))

if __name__ == "__main__":
	pos = get_position()

	print("image (50,50) 100 100 stretch /usr/local/hobart/user/softkeyscripts/graphics/equirectangular_earth.png")

	# convert lat and long into percent offsets from the middle of our image
	# n.b. equirectangular map project makes this very simple
	x = 50.0 + (pos[0] / 180.0) * 50.0
	y = 50.0 - (pos[1] / 90.0) * 50.0
	
	print("circle (%d,%d) 3 #ff0000" % (x,y))
	print("text (50,80) 10 #ffffff ISS Position")
