#!/bin/env python3

# push local operators into blackbox, or pull operators from blackbox
# overwrites on collision!

import os
import sys
import sqlite3
import configInfo
import requests
import json
from contextlib import closing

def pull(db_conn, url, replace):
    r = requests.get(url)
    operators = r.json()
    
    if r.status_code != 200:
        print("unexpected response status code: %s" % r.status_code)
        return -1
        
    print("pulled %s operators" % len(operators))
    
    with closing(db_conn.cursor()) as cursor:
        if replace:
            cursor.execute("delete from Opers");
            
        for oper in operators:
            keys, values = zip(*oper.items())
            sql = "insert or replace into Opers (%s) values (%s)" % (",".join(keys),",".join(['?']*len(keys)))
            cursor.execute(sql, values)
        
    db_conn.commit()

    return 0
    
def push(db_conn, url, replace):
    operators = []
    
    with closing(db_conn.cursor()) as cursor:
        cursor.execute("select * from Opers")
        operators = [dict(row) for row in cursor.fetchall()]
        
    if replace:
        requests.delete(url)
        
    for oper in operators:
        r = requests.put(url, json=oper)
        
        if r.status_code != 200:
            print("unexpected response status code: %s" % r.status_code)
            return -1
            
    print("pushed %s operators" % len(operators))
    
    return 0

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("blackboxOpers.py push|pull merge|replace BLACKBOX_ADDRESS")
        sys.exit(-1)
        
    result = 0
    
    action = sys.argv[1]
    replace = sys.argv[2]
    bb_address = sys.argv[3]
    
    url = "http://%s/api/v1/operators" % bb_address

    db_path = None

    with open("/etc/hobart/defaults_linux.ini") as f:
      for line in f:
        if "db.SQLite.path=" in line:
          db_path = line.split("=")[1]

    db_conn = sqlite3.connect(db_path)
    db_conn.row_factory = sqlite3.Row
    
    if replace == "merge":
        replace = False
    elif replace == "replace":
        replace = True
    else:
        print("invalid argument: %s" % replace)
        sys.exit(-1)
    
    if action == "push":
        result = push(db_conn, url, replace)
    elif action == "pull":
        result = pull(db_conn, url, replace)
    else:
        print("invalid action: %s" % action)
        result = -1
    
    db_conn.close()
    
    sys.exit(result)
