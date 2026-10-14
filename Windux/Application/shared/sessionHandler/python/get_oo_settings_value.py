#!/usr/bin/env python3

# Utility script to retrieve the value from the Online Ordering 'settings' table of the column specified

import sys
import requests
import json

def get_value(configSetting):
    
    bb_address = "127.0.0.1"
    
    try:
        settings_url = "http://%s/api/v1/settings" % (bb_address)
        settings_req = requests.get(settings_url, timeout=(5, 5))
        
        if settings_req.status_code != 200:
            #print("None")
            return("None")
        
        settings = settings_req.json()[0]
        
        if not settings[configSetting]:
            #print("None")
            return("None")
        else:
            #print(settings[configSetting])
            return(settings[configSetting])
            
    except Exception as e:
        print("Error")

    
if __name__ == "__main__":

    # Identifies what column in the settings table to retrieve
    if len(sys.argv) == 2:
        print(get_value(configSetting=sys.argv[1]))
    else:
        print("Error")