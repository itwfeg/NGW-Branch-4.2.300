#!/usr/bin/env python3

# Delete orders from the orders table that are more than X days old, based on the OrderPickup date/time

import sys
import requests
import json
import pymysql

# Retrieve the value from the settings table using the column specified
def get_value(configSetting):
    
    bb_address = "127.0.0.1"
    
    rval = "None"
    
    try:
        settings_url = "http://%s/api/v1/settings" % (bb_address)
        settings_req = requests.get(settings_url, timeout=(5, 5))
        
        if settings_req.status_code != 200:
            rval = "None"
        else:
            settings = settings_req.json()[0]
            
            if not settings[configSetting]:
                rval = "None"
            else:
                rval = settings[configSetting]
            
    except Exception as e:
        rval = "None"

    return rval
    
def delete_records():
    
    numberOfDays = get_value("OrderHistoryDays")
    #print(numberOfDays)
    
    if numberOfDays != "None":
    
        connection = pymysql.connect(host='localhost',
                                 user='hobart',
                                 password='hobart',
                                 database='online_ordering',
                                 charset='utf8mb4',
                                 cursorclass=pymysql.cursors.DictCursor)
                                 
        try:

            with connection.cursor() as cursor:
         
                sql = "delete from orders where datediff(now(), orders.OrderPickup) > %s;" % (numberOfDays)
                #print(sql)
            
                cursor.execute(sql)
                
            connection.commit()
         
        finally:
        
            connection.close()
            
            exit(0)

    
if __name__ == "__main__":

    # Delete the old records
    delete_records()
 