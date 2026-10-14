#!/usr/bin/env python3

# displays IP address, RAM %, disk usage, software version, number of PLUs in DB

from getIpAddress import getIpAddress
import shutil
import sqlite3
from contextlib import closing
import configInfo
import os

def percent_to_color(x):
  if x > 90:
    return "#ff0000"
  elif x > 75:
    return "#ffff00"
  else:
    return "#00ff00"

# read IP address
ip = getIpAddress()[1]

# get RAM usage
total_mem, _, _, _, _, avail_mem = list(map(int, os.popen('free -m').readlines()[1].split()[1:]))
mem_usage = 100 - int(100*(avail_mem / total_mem))

# read disk space
disk_usage = int((shutil.disk_usage("/").used / shutil.disk_usage("/").total) * 100)

# read software version
version = ""
with open("/SOFTWARE_VERSION_INFO") as f:
  version = f.readline().replace("Release ID: ", "").strip()

# read how many PLUs are in the DB
db_path = ""
with open("/etc/hobart/defaults_linux.ini") as f:
  db_path = [x for x in f if "db.SQLite.path=" in x][0]

db_path = configInfo.getRootFolder() + os.path.join(os.sep, "db", "scale", "dbfiles", db_path.split("/")[-1]).strip()
db_conn = sqlite3.connect(db_path)
db_conn.row_factory = sqlite3.Row

plu_count = 0
with closing(db_conn.cursor()) as cursor:
  sql = "select count(*) from Prods"
  cursor.execute(sql)
  plu_count = cursor.fetchall()[0][0]

print("text (50,7) 15 #ffffff Diagnostics")
print("text (50,22) 12 #ffffff IP: %s" % str(ip))
print("text (50,37) 12 #ffffff Vers.: %s" % version)
print("text (50,52) 12 #ffffff PLU Count: %s" % plu_count)

# print RAM
mem_color = percent_to_color(mem_usage)
mem_usage_color = "#ffffff" if mem_usage > 90 or mem_usage < 75 else "#000000"
print("rectangle (50,70) 100 15 #000000")
print("rectangle (%s,70) %s 15 %s" % (int(mem_usage/2), mem_usage, mem_color))
print("text (50,69) 12 %s RAM: %s%%" % (mem_usage_color, mem_usage))

# print disk
disk_color = percent_to_color(disk_usage)
disk_usage_text_color = "#ffffff" if disk_usage > 90 or disk_usage < 75 else "#000000"
print("rectangle (50,87) 100 15 #000000")
print("rectangle (%s,87) %s 15 %s" % (int(disk_usage/2), disk_usage, disk_color)) 
print("text (50,86) 12 %s Disk: %s%%" % (disk_usage_text_color, disk_usage))

