#!/bin/env python3

# push local data to blackbox, or pull data from blackbox
# overwrites on collision!

import os
import sys
import sqlite3
import requests
import json
import argparse
from contextlib import closing

# number of items to bundle together per API write
CHUNK_SIZE = 20

def chunks(lst, n):
    """Yield successive n-sized chunks from lst."""
    for i in range(0, len(lst), n):
        yield lst[i:i + n]

def guess_encoding(x):
    # tries to decode text from the database using various common encodings
    r = None
    
    if len(x) == 0:
        return ''
    
    for enc in ['utf-8', 'iso-8859-1', 'utf-16', 'utf-32', 'latin1', 'cp1252', 'iso-8859-2']:
        try:
            r = x.decode(enc)
            if enc != 'utf-8':
                print("warning: using %s to decode string \"%s\"" % (enc, x))
            break
        except:
            continue
            
    # if nothing worked, force it into utf-8
    if not r and x != b'':
        r = x.decode('utf-8', 'ignore')
        print("warning: dropping characters to decode string \"%s\"" % x)
        
    return r
    
def send(url, api_endpoint, data):
    r = requests.put(url, json=data)
    
    if r.status_code != 200:
        print("unexpected response status code for %s: %s" % (api_endpoint, r.status_code))
        print(r.content)
        return False
    else:
        return True

def push(args, db_conn, api_endpoint, db_table):
    data = []
    url = "http://%s/api/v1/%s" % (args.address, api_endpoint)
    count = 0
    
    if args.conflict == "replace":
        requests.delete(url)
    
    with closing(db_conn.cursor()) as cursor:
        cursor.execute("select * from " + db_table)
        
        chunk = cursor.fetchmany(CHUNK_SIZE)
        while chunk:
            count += len(chunk)
            data = []
            
            for row in chunk:
                data.append({k: dict(row)[k] for k in dict(row).keys()})
                
            if not send(url, api_endpoint, data):
                return -1
                        
            chunk = cursor.fetchmany(CHUNK_SIZE)
            
    print("pushed %s to %s at %s" % (count, api_endpoint, args.address))
    
    return 0

def pull(args, db_conn, api_endpoint, db_table):
    r = requests.get("http://%s/api/v1/%s" % (args.address, api_endpoint))
    results = r.json()
    
    if r.status_code != 200:
        print("unexpected response status code for %s: %s" % (api_endpoint, r.status_code))
        return -1
        
    print("pulled %s from %s at %s" % (len(results), api_endpoint, args.address))
    
    with closing(db_conn.cursor()) as cursor:
        if args.conflict == "replace":
            cursor.execute("delete * from " + db_table)
        
        # do a trivial select so we can get the column names
        cursor.execute("select * from (%s) limit 0" % db_table)
        column_names = [column[0] for column in cursor.description]
            
        for result in results:
            # filter to only insert things that fit the DB schema
            result = {k: result[k] for k in result.keys() if k in column_names}
            
            keys, values = zip(*result.items())
            sql = "insert or replace into %s (%s) values (%s)" % (db_table, ",".join(keys), ",".join(['?']*len(keys)))
            cursor.execute(sql, values)
            
    return 0
    
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="synchronizes DB data to/from an online ordering BB via the API")
    parser.add_argument("--operators", action='store_true', help="sync operators")
    parser.add_argument("--products", action='store_true', help="sync products")
    parser.add_argument("--departments", action='store_true', help="sync departments")
    parser.add_argument('--db-file', nargs=1, help="alternate DB file to load from")
    parser.add_argument("--address", help="address of the BB to interact with (defaults to onlineOrderingServer from config in db)")
    parser.add_argument("action", choices=["push","pull"], help="either push to or pull from the BB")
    parser.add_argument("conflict", choices=["replace","merge"], help="replace (delete existing) or merge (keep existing, replace on conflict)")

    args = parser.parse_args()
    
    action = None
    if args.action == "push":
        action = push
    else:
        action = pull
    
    result = 0
    db_path = None
    if args.db_file:
        db_path = args.db_file[0]

    if not db_path:
        with open("/etc/hobart/defaults_linux.ini") as f:
          for line in f:
            if "db.SQLite.path=" in line:
              db_path = line.split("=")[1].strip()

    db_conn = sqlite3.connect(db_path)
    db_conn.row_factory = sqlite3.Row
    # this lets us handle unknown text encodings from the customer data
    db_conn.text_factory = guess_encoding
    
    if not args.address:
        with closing(db_conn.cursor()) as cursor:
            sql = 'select CoValue from Configs where CoName = "onlineOrderingServer"'
            cursor.execute(sql)
            args.address = cursor.fetchall()[0][0]
    
    if args.operators:
        result += action(args, db_conn, "operators", "Opers")
        
    if args.products:
        result += action(args, db_conn, "products", "Prods")
        
    if args.departments:
        result += action(args, db_conn, "departments", "Depts")

    db_conn.commit()
        
    db_conn.close()
    
    sys.exit(result)
