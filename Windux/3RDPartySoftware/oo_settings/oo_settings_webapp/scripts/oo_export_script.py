# @file oo_export_script.py
# @author Tony Shelton-McGaha
# interacts with blackbox, retrieves online order data, exports to CSV file

import sys
import csv
import requests
import json
from requests.exceptions import ConnectionError, Timeout

def bb_ok(bb_address):
    url = "http://%s/api/v1/version" % (bb_address)
    req = requests.get( url, timeout=(1,30))

    if req.status_code != 200:
        return False
        
    try:
        version = req.json()
        if not "deviceType" in version:
            return False
    except:
        return False

    return True

def get_orders(bb_address, dir_path):
    url = "http://%s/api/v1/orders" % (bb_address)
    req = requests.get( url, timeout=(1, 30) )
    
    if req.status_code != 200:
        raise ConnectionError

    orders = req.json()

    header = ["OrderNumber","OrderPickup","OrderStatus","CustomerName","OrderSMS","OrderOrigin","StoreNumber","OrderCreation","AllowSubstitutions","ItemCount","Operators","OrderFinish","OrderStart","OrderInstructions"]

    orders_new = []

    for o in orders:
        orders_new.append({k: o[k] for k in o.keys() if k in header})

    with open(dir_path + '/orders_export_data.csv', 'w', encoding="UTF8") as f:
        writer = csv.DictWriter(f, fieldnames=header)
        writer.writeheader()
        writer.writerows(orders_new)

def get_order_details(bb_address, dir_path):
    details_header = ["OrderNumber", "DeptNum", "ZoneNum", "OrderNumId", "PrNum", "ProductType", "PluByCount", "PluWeight", "PluInstructions", "ReasonCancelled", "Cancelled", "Filled", "Substituted", "OriginalPluNumber","FinalPrice", "FinalWeight","FinalCount","AllowSubstitutions"]

    order_details_new = []

    details_url = "http://%s/api/v1/orderdetails/" % (bb_address)
    details_req = requests.get(details_url, timeout=(1,30))

    if details_req.status_code != 200:
        raise ConnectionError

    details = details_req.json()

    for od in details:
        order_details_new.append({k: od[k] for k in od.keys() if k in details_header})

    with open(dir_path + '/order_details_export_data.csv', 'w', encoding="UTF8") as f:
        writer = csv.DictWriter(f, fieldnames=details_header)
        writer.writeheader()
        writer.writerows(order_details_new)

def get_logs(bb_address, dir_path):
    logs_url = "http://%s/api/v1/logs/" % (bb_address)
    logs_req = requests.get(logs_url, timeout=(1,30))

    if logs_req.status_code != 200:
        raise ConnectionError

    with open(dir_path + '/oo_log.txt', 'wb') as f:
        for chunk in logs_req.iter_content(chunk_size=1024*1024):
            f.write(chunk)

def run_script():
    bb_address = sys.argv[1]
    dir_path = sys.argv[2]

    if not bb_ok(bb_address):
        print("invalid address failure")
        sys.exit(-1)
    
    get_orders(bb_address, dir_path)
    get_order_details(bb_address, dir_path)
    get_logs(bb_address, dir_path)

    print( "success" )

if __name__ == "__main__":
    run_script()
    sys.exit(0)
    try:
        run_script()
    except ConnectionError:
        print( "connection failure" )
        sys.exit(-1)
    except Timeout:
        print( "timeout failure" )
        sys.exit(-1)

