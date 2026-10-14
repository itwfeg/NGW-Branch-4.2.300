import json
import os
import subprocess
import socket
import shutil
import base64
import math
import datetime
import time
from datetime import date

import logging
import logging.config

STATUS_WAITING = 1
STATUS_IN_PROGRESS = 2
STATUS_READY = 3
STATUS_DELIVERED = 4
STATUS_CANCELLED = 5

ORIGIN_ONLINE = 1
ORIGIN_KIOSK = 2
ORIGIN_NUMBER = 3

LEGAL_CHARS = "1234567890-=qwertyuiop[]asdfghjklzxcvbnm. _()+QWERTYUIOPASDFGHJKLZXCVBNM"

HOST_IMAGE_PATH = "/var/www/web2py/applications/OnlineOrdering/static/images/"
SYSTEM_IMAGE_PATH = "/usr/local/hobart/images/system/"

logger = logging.getLogger( request.application )
logger.setLevel( logging.INFO )

def __nice_time(s):
    h,m,s = str(s).split(" ")[1].split(":")
    am_pm = "AM" if int(h) < 12 else "PM"
    if int(h) > 12:
        h = str(int(h) - 12)
    if int(h) == 0:
        h = "12"
    
    return h + ":" + m + " " + am_pm
    
def __update_order_item_count(OrderNumber):
    zones_str = "select ZoneNum, DeptNum from orderassignments where OrderNumber = %(OrderNumber)s;"
    zones = db.executesql(zones_str, as_dict=True, placeholders={"OrderNumber":OrderNumber})
    
    for part in zones:
        part["OrderNumber"] = OrderNumber
        count_str = "select count(*) from orderdetails where OrderNumber = %(OrderNumber)s and ZoneNum = %(ZoneNum)s and DeptNum = %(DeptNum)s;"
        part["ItemCount"] = int(db.executesql(count_str, as_dict=True, placeholders=part)[0]["count(*)"])
    
        #update_str = "update orderassignments set ItemCount = %(ItemCount)s where OrderNumber = %(OrderNumber)s and ZoneNum = %(ZoneNum)s and DeptNum = %(DeptNum)s;"
        #raise HTTP(500, str(part))
        insert("orderassignments", part, update=True, parse_json=False)
        #db.executesql(update_str, as_dict=True, placeholders={"OrderNumber":OrderNumber,"ZoneNum":ZoneNum,"DeptNum":DeptNum, "ItemCount":ItemCount})

def __get_order_status(OrderNumber):
    order = get("orders", {"OrderNumber":OrderNumber})
    if len(order) == 1:
        return int(order[0]["OrderStatus"])
    else:
        return 0
        
def __get_part_status(OrderNumber, DeptNum, ZoneNum):
    order = get("orderassignments", {"OrderNumber":OrderNumber, "ZoneNum": ZoneNum, "DeptNum": DeptNum})
    if len(order) == 1:
        return int(order[0]["PartStatus"])
    else:
        return 0

def get_setting(s):
    current_settings = None
    
    sql_str = "select * from settings order by id desc limit 1;"
    
    try:
        current_settings = db.executesql(sql_str, as_dict=True)
        return current_settings[0][s]
    except:
        raise HTTP(500, "failed to query database for settings")
    
def send_sms(number, message):
    # would be faster to use popen but it doesn't work consistently?
    #subprocess.Popen(["python", "/root/sendSMS.py", "6144670010", message])
    if number and message:
        logger.info( "send_sms() - message \"%s\" to %s" % (message, number) )
        subprocess.call(["python", "/usr/local/hobart/python/sendSMS.py", number, message])

def delete(tablename, conditions):
    compares = {"gt": ">", "gte": ">=", "lt": "<", "lte": "<=", "eq": "=", "neq": "!="}
    sql_str = f"delete from {tablename} "
    wheres = []
    
    for k in conditions.keys():
        if "." in k:
            op = k.split('.')[1]
            column = k.split('.')[0]
            wheres.append(column + " " + compares[op] + " %(" + k + ")s")
        else:
            wheres.append(k + " = %(" + k + ")s")
        
    if wheres:
        sql_str += " where "
        
    sql_str += " and ".join(wheres)
    
    sql_str += ";"
    
    try:
        db.executesql(sql_str, placeholders=conditions)
    except Exception as e:
        db.rollback()
        logger.info( "insert() - DB Insert: FAILED : %s : conditions: %s" (str(e).replace("\n"," \t"), conditions) )
        raise HTTP(500, "failed to query database")
        
    db.commit()
    
def is_col_name(tablename, col):
    return col in [c[0] for c in db.executesql(f'select COLUMN_NAME from INFORMATION_SCHEMA.COLUMNS where TABLE_NAME = "{tablename}";')]

def validate_args(tablename, args):
    cols = set([c[0] for c in db.executesql(f'select COLUMN_NAME from INFORMATION_SCHEMA.COLUMNS where TABLE_NAME = "{tablename}";')])
    cols.update(["sort", "dir", "limit", "before", "after", "status"])
    return { k: args[k] for k in args.keys() if (k in cols) or ('.' in k and (k.split('.')[0] in cols)) }

# conditions is a dict with entries like {"DeptNum": "10", "OrderNumber.gte": 10 } 
# if it has a '.' in it, the last part used for various kinds of operators or matching
def get(tablename, conditions):
    data = []
    sql_str = f"select * from {tablename}"
    wheres = []
    compares = {"gt": ">", "gte": ">=", "lt": "<", "lte": "<=", "eq": "=", "neq": "!="}
    sort = None
    limit = None
    sort_dir = None
    
    if "before" in conditions.keys() and tablename == "orders":
        conditions["OrderPickup.lt"] = conditions["before"]
        del conditions["before"]
        
    if "after" in conditions.keys() and tablename == "orders":
        conditions["OrderPickup.gt"] = conditions["after"]
        del conditions["after"]
        
    if "status" in conditions.keys() and tablename == "orders":
        conditions["OrderStatus"] = conditions["status"]
        del conditions["status"]
    
    if "sort" in conditions.keys() and is_col_name(tablename, conditions["sort"]):
        sort = conditions["sort"]
        del conditions["sort"]
    
    if "dir" in conditions.keys() and conditions["dir"] in ["asc", "desc"]:
        sort_dir = conditions["dir"]
        del conditions["dir"]
        
    if "limit" in conditions.keys():
        try:
            limit = int(conditions["limit"])
            del conditions["limit"]
        except:
            raise HTTP(400, "invalid request")
    
    for k in conditions.keys():
        if "." in k:
            op = k.split('.')[-1]
            column = ".".join(k.split('.')[:-1])
            if op == "search":
                wheres.append("lower(" + column + ") regexp '[[:<:]]" + conditions[k].lower() + "'")
            else:
                wheres.append(column + " " + compares[op] + " %(" + k + ")s")
        else:
            wheres.append(k + " = %(" + k + ")s")
    
    if wheres:
        sql_str += " where " + " and ".join(wheres)
        
    if sort:
        sql_str += " order by " + sort
        if sort_dir:
            sql_str += " " + sort_dir
            
    if limit:
        sql_str += " limit " + str(limit)
    
    sql_str += ";"
    
    try:
        data = db.executesql(sql_str, placeholders=conditions, as_dict=True)
    except Exception as e:
        #raise HTTP(500, sql_str + " WITH CONDITIONS " + conditions + " AND THEN " + str(e))
        logger.info( "get() - DB Get: FAILED : " + str(e) + " : conditions: " + str(conditions) )
        raise HTTP(500, "db failure on get")
    
    return data

def insert(tablename, items, update=False, parse_json=True):
    if parse_json:
        try:
            items = json.loads(items)
        except:
            raise HTTP(400, "failed to parse JSON")
        
    # make sure it's a list for convenience
    if type(items) != list:
        items = [items]
        
    # if we don't want to update rows, check if they exist first
    if not update:
        for item in items:
            # if it's possible for there to be a conflict, check first
            # (there can't be a conflict if a primary key is auto-increment)
            if len(db.executesql(f"select COLUMN_NAME from INFORMATION_SCHEMA.COLUMNS where TABLE_NAME = '{tablename}' and COLUMN_KEY = 'PRI' and EXTRA = 'auto_increment'")) == 0:
                # get the primary keys for the requested table, but not auto-increment ones
                pks = db.executesql(f"select COLUMN_NAME from INFORMATION_SCHEMA.KEY_COLUMN_USAGE where TABLE_NAME = '{tablename}' and CONSTRAINT_NAME = 'PRIMARY'")
                # get the primary keys from the item to be inserted
                pks = {k[0]: item[k[0]] for k in pks}
                if len(get(tablename, pks)) != 0:
                    raise HTTP(400, "item already exists")
        
    # get all column names from the table
    cols = set([c[0] for c in db.executesql(f'select COLUMN_NAME from INFORMATION_SCHEMA.COLUMNS where TABLE_NAME = "{tablename}";')])
    
    for item in items:
        sql_str = "insert into " + tablename + " ("
        # ignore anything from the item that isn't a valid column name
        fields_to_insert = { k: item[k] for k in item.keys() if k in cols }

        sql_str += " , ".join(fields_to_insert.keys())
        sql_str += ") values (";
        sql_str += " , ".join(["%(" + k + ")s" for k in fields_to_insert.keys()])
        sql_str += ")"
        if update:
            sql_str += " on duplicate key update "
            sql_str += " , ".join([k + " = %(" + k + ")s" for k in fields_to_insert.keys()])
        sql_str += ";"
        
        try:
            db.executesql(sql_str, placeholders=item)
        except Exception as e:
            #raise HTTP(400, str(e) + " ################# " + str(item))
            #raise HTTP(400, str(item))
            db.rollback()
            logger.info( "insert() - DB Insert: FAILED : " + str(e).replace("\n"," \t") )
            #raise HTTP(400, "SQL failure " + str(e))
            raise HTTP(500, "db failure on insert")
            
    db.commit()
    return len(items)

def order_details_change_plu(OrderNumber, OrderNumId, DeptNum, new_plu):
    new_prod = db.executesql("select * from prods where PrNum = %(new_plu)s", placeholders=locals(), as_dict=True)

    if not new_prod:
        raise HTTP(404, "no such product")
    else:
        new_prod = new_prod[0]
        
    old_plu = db.executesql("select PrNum from orderdetails where DeptNum = %(DeptNum)s and OrderNumId = %(OrderNumId)s and OrderNumber = %(OrderNumber)s", placeholders=locals(), as_dict=True)[0]
    
    new_details = {"DeptNum": DeptNum, "OrderNumId": OrderNumId, "OrderNumber": OrderNumber, "PluPrice": new_prod["PrPrice"], "PluDescription": new_prod["PrDesc"], "Substituted": 1, "Filled": 0, "OriginalPrNum": old_plu["PrNum"], "PrNum": new_plu }
    
    logger.info( "order_details_change_plu() - Insert Order Details: %s " % new_details )
    return insert("orderdetails", new_details, update=True, parse_json=False)

def order_details_reset_substitution(OrderNumber, OrderNumId, DeptNum):
    details = db.executesql("select * from orderdetails where OrderNumber = %(OrderNumber)s and OrderNumId = %(OrderNumId)s and DeptNum = %(DeptNum)s", placeholders=locals(), as_dict=True)

    if details and details[0] and details[0]['Substituted']:
        PrNum = str(details[0]['PrNum'])
        
        if type(PrNum) not in [int, str] and type(DeptNum) not in [int,str]:
            raise HTTP(500, "reset failure")
            
        old_prod = db.executesql("select * from prods where PrNum = %(PrNum)s and DeptNum = %(DeptNum)s", placeholders={"DeptNum":str(DeptNum), "PrNum": str(PrNum)}, as_dict=True)
        
        PluDescription = old_prod[0]['PrDesc']
        
        sql_str = "update orderdetails set"
        sql_str += " PrNum = %(PrNum)s, PluDescription = %(PluDescription)s, "
        sql_str += " OriginalPrNum = NULL where OrderNumber = %(OrderNumber)s and OrderNumId = %(OrderNumId)s and DeptNum = %(DeptNum)s"
        
        logger.info( "order_details_reset_substitution() - DeptNum: %s PrNum: %s OrderNumId: %s OrderNumber: %s" % (str(DeptNum), str(PrNum), str(OrderNumId), str(OrderNumber)))
        db.executesql(sql_str, placeholders={"DeptNum": str(DeptNum), "PrNum": str(PrNum), "PluDescription": str(PluDescription), "OrderNumber": str(OrderNumber), "OrderNumId": str(OrderNumId), "DeptNum": str(DeptNum)})
    
def order_details_reset_all_substitution(OrderNumber, DeptNum):
    order_num_ids = db.executesql("select OrderNumId from orderdetails where OrderNumber = %(OrderNumber)s and DeptNum = %(DeptNum)s", placeholders=locals(), as_dict=True)
    order_num_ids = [x['OrderNumId'] for x in order_num_ids]
    
    for num_id in order_num_ids:
        order_details_reset_substitution(OrderNumber, num_id, DeptNum)

def order_details_reset(data, args):
    placeholders = {}
    OrderNumber = None
    OrderNumId = None

    DeptNum = 0
    sql_str = "update orderdetails set ReasonCancelled = \"\", Cancelled = 0, Filled = 0, Substituted = 0, OriginalPrNum = NULL, FinalPrice = NULL, FinalWeight = NULL, FinalCount = NULL where OrderNumber = %(OrderNumber)s"
    
    # if we want to reset a whole order
    if len(args) == 2:
        OrderNumber = int(args[0])
        logger.info( "order_details_reset() #1 - OrderNumber: %s DeptNum: %s " % (str(OrderNumber), str(DeptNum)) )
        order_details_reset_all_substitution(OrderNumber, DeptNum)
    # if we want to reset just a single details
    elif len(args) == 3:
        OrderNumber = int(args[0])
        OrderNumId = int(args[1])
        sql_str += " and OrderNumId = %(OrderNumId)s"
        # reset any substitution, if needed
        logger.info( "order_details_reset() #2 - OrderNumber: %s OrderNumId: %s DeptNum: %s" % (OrderNumber, OrderNumId, DeptNum) )
        order_details_reset_substitution(OrderNumber, OrderNumId, DeptNum)
    else:
        raise HTTP(400, "invalid reset")
        
    db.executesql(sql_str, placeholders=locals())
    
    return response.json({})

@request.restful()
def orderdetails():
    response.headers["Access-Control-Allow-Origin"] = '*'
    
    def GET(*args, **kwargs):
        if len(args) > 4:
            raise HTTP(400, "invalid number of arguments")
        
        try:
            if len(args) >= 1:
                kwargs["OrderNumber"] = int(args[0])
            if len(args) >= 2:
                kwargs["DeptNum"] = int(args[1])
            if len(args) >= 3:
                kwargs["ZoneNum"] = int(args[2])
            if len(args) >= 4:
                kwargs["OrderNumId"] = int(args[3])
        except:
            raise HTTP(400, "invalid arguments")
                        
        return response.json(get("orderdetails", kwargs))
        
    def POST(*args, **kwargs):
        raise HTTP(405, "posting orderdetails not supported")
        
    def PUT(*args, **kwargs):
        data = request.body.read()

        try:
            data = json.loads(data)
        except:
            raise HTTP(400, "invalid JSON")

        if args and args[-1] == "reset":
            return order_details_reset(data, args)
        elif args and args[-1] == "substitute":
            return response.json(order_details_change_plu(data["OrderNumber"], data["OrderNumId"], data["DeptNum"], data["NewPlu"]))
        
        if type(data) != list:
            data = [data]

        try:
            for item in data:
                if len(args) >= 1:
                    item["OrderNumber"] = int(args[0])
                if len(args) >= 2:
                    item["DeptNum"] = int(args[1])
                if len(args) >= 3:
                    item["ZoneNum"] = int(args[2])
                if len(args) >= 4:
                    item["OrderNumId"] = int(args[3])
        except:
            raise HTTP(400, "invalid arguments")
            
        r = []
        
        for item in data:
            __add_item(item)
            
            r.append(item)
            
        if len(r) == 1:
            r = r[0]
        
        return response.json(r)
        
    def DELETE(*args, **kwargs):
        conditions = {}
        
        if len(args) != 2:
            raise HTTP(400, "invalid arguments")
        
        try:
            conditions = {"OrderNumber": int(args[0]), "OrderNumId": int(args[1])}
        except:
            raise HTTP(400, "invalid arguments")
        
        logger.info( "orderdetails() - Delete Order: %s " % conditions["OrderNumber"] )
        delete("orderdetails", conditions)
        
        __update_order_item_count(conditions["OrderNumber"])
        
        return response.json(conditions)
        
    return locals()
    
def __new_order_sms(order):
    if "OrderSMS" in order and "OrderPickup" in order and get_setting("EnableCustomerSMS") == 1:
        send_sms(order["OrderSMS"], "Your order has been accepted (#%s).  Est. completion: %s" % (order["OrderNumber"], __nice_time(order["OrderPickup"])))

def __order_accepted(order):
    __new_order_sms(order)

def __order_cancelled(order):
    if "OrderSMS" in order and get_setting("EnableCustomerSMS") == 1:
        send_sms(order["OrderSMS"], "Your order has been cancelled (#%s)." % order["OrderNumber"])
    
def __order_ready(order):
    if "OrderSMS" in order and get_setting("EnableCustomerSMS") == 1:
        send_sms(order["OrderSMS"], "Your order is now ready (#%s)." % order["OrderNumber"])

def __order_started(order):
    pass

def __add_item(item):
    # supply an ordernumid if it doesn't have one
    if "OrderNumId" not in item:
        last_id = db.executesql("select max(OrderNumId) from orderdetails where OrderNumber = %(OrderNumber)s", placeholders={"OrderNumber": item["OrderNumber"]}, as_dict=True)
        if last_id[0]['max(OrderNumId)'] is None:
            item["OrderNumId"] = 1
        else:
            item["OrderNumId"] = int(last_id[0]['max(OrderNumId)']) + 1
            
    # if this item already exists, load what we've got first
    old_item = get("orderdetails", {"OrderNumber": item["OrderNumber"], "OrderNumId": item["OrderNumId"]})

    if len(old_item) == 1:
        old_item[0].update(item)
        item = old_item[0]
        
    if not ("PrNum" in item and "DeptNum" in item):
        raise HTTP(400, "item must have a PrNum and DeptNum")
        
    # if a zone isn't provided, try to get it from the product
    if "ZoneNum" not in item:
        item["ZoneNum"] = db.executesql("select ZoneNum from prods where PrNum = %(PrNum)s and DeptNum = %(DeptNum)s;", placeholders=item, as_dict=True)[0]["ZoneNum"]
    
    assignment = {"ZoneNum": item["ZoneNum"], "DeptNum": item["DeptNum"], "OrderNumber": item["OrderNumber"], "PartStatus": STATUS_WAITING}
    
    # see if we have a suborder for this zone already
    if not db.executesql("select * from orderassignments where OrderNumber = %(OrderNumber)s and DeptNum = %(DeptNum)s and ZoneNum = %(ZoneNum)s", placeholders=assignment,as_dict=True):
        logger.info( "__add_item() - creating new orderassignments for OrderNumber %s DeptNum %s Zone %s" %  (item["OrderNumber"], item["DeptNum"], item["ZoneNum"]))
        insert("orderassignments", assignment, parse_json=False, update=False)
    
    # supply a description and product type if it doesn't have one
    if "PrNum" in item and ("PluDescription" not in item or "ProductType" not in item or "PluPrice" not in item):
        desc = db.executesql("select PrDesc, PrType, PrPrice from prods where DeptNum = %(DeptNum)s and PrNum = %(PrNum)s", placeholders={"DeptNum": item["DeptNum"], "PrNum": item["PrNum"]}, as_dict=True)
        if desc and "PrDesc" in desc[0]:
            item["PluDescription"] = desc[0]["PrDesc"]
            item["ProductType"] = desc[0]["PrType"]
            item["PluPrice"] = desc[0]["PrPrice"]
        else:
            raise HTTP(400, "unknown product")
    
    logger.info( "__add_item() - adding new item to OrderNumber %s OrderNumId %s PrNum %s" %  (item["OrderNumber"], item["OrderNumId"], item["PrNum"]))
    insert("orderdetails", item, update=True, parse_json=False)
    
    __update_order_item_count(item["OrderNumber"])
    
    return item["OrderNumId"]

def __now():
    return db.executesql("select CURRENT_TIMESTAMP", as_dict=True)[0]["CURRENT_TIMESTAMP"]

def __add_order(order, update=False):
    if "OrderStatus" not in order:
        order["OrderStatus"] = STATUS_WAITING
    
    # provide a creation time (now) if needed
    if "OrderCreation" not in order:
        order["OrderCreation"] = __now()
    
    # pull the items out to avoid confusing the DB
    items = []
    if "Items" in order:
        items = order["Items"]
        order.pop("Items")
    
    if "OrderNumber" in order:
        old_status = __get_order_status(order["OrderNumber"])

    insert("orders", order, parse_json=False, update=update)

    # if there was no ordernumber provided, the DB autoincremented one
    # so get it after the insertion
    new_number = None
    
    if "OrderNumber" in order:
        new_number = order["OrderNumber"]
    else:
        new_number = int(db.executesql("select LAST_INSERT_ID();", as_dict=True)[0]["LAST_INSERT_ID()"])
        order["OrderNumber"] = new_number
        
    logger.info( "orders() #1 - Insert Order: %s " % new_number )
    
    # add any items (this creates orderparts by dept/zone)
    for item in items:
        if "OrderNumber" not in item:
            item["OrderNumber"] = order["OrderNumber"]
        __add_item(item)
            
    # if it's a take a number order being created, create an empty part in zone 0
    if len(items) == 0 and "OrderOrigin" in order and order["OrderOrigin"] == ORIGIN_NUMBER:
        logger.info( "orders() - adding empty part for new Now Serving order OrderNumber %s" % new_number )
        DeptNum = order["DeptNum"] if "DeptNum" in order else 0
        new_part = {"OrderNumber": order["OrderNumber"], "DeptNum": DeptNum, "ZoneNum": 0, "PartStatus": STATUS_WAITING}
        insert("orderassignments", new_part, update=True, parse_json=False)
    
    __update_order_item_count(new_number)

    # run notifications/hooks if needed
    new_status = order["OrderStatus"]
    
    if not update:
        if new_status == STATUS_WAITING:
            __order_accepted(order)
    
    if update:
        if old_status is None and new_status == STATUS_WAITING:
            __order_accepted(order)
        
        if old_status == STATUS_WAITING and new_status in [STATUS_READY,STATUS_IN_PROGRESS]:
            __order_ready(order)
        
        if old_status in [STATUS_WAITING,STATUS_READY,STATUS_IN_PROGRESS] and new_status == STATUS_CANCELLED:
            __order_cancelled(order)
            
            # also cancel all parts of this order
            logger.info( "orders() - order cancelled, cancelling all parts of the order OrderNumber %s" % new_number )
            db.executesql("update orderassignments set PartStatus = %(PartStatus)s where OrderNumber = %(OrderNumber)s;", placeholders={"PartStatus": STATUS_CANCELLED, "OrderNumber":OrderNumber})
    
    return new_number

@request.restful()
def orders():
    response.headers["Access-Control-Allow-Origin"] = '*'
    
    def GET(*args, **kwargs):
        response_data = []
        conditions = {}
        
        # "orders/ordernumber" returns one order
        if len(args) == 1:
            try:
                conditions = {"OrderNumber": int(args[0])}
            except:
                raise HTTP(400, "invalid argument")
        # "orders/" returns all orders (maybe filtered)
        elif len(args) == 0:
            pass
        else:
            raise HTTP(400, "invalid request")

        # add argument conditions
        conditions.update(validate_args("orders", kwargs))
        
        response_data = get("orders", conditions)
        
        # since orders will now be GET'd infrequently, recompute these as needed
        for order in response_data:
            # get item count for this order
            count_str = "select count(*) from orderdetails where OrderNumber = %(OrderNumber)s"
            order["ItemCount"] = int(db.executesql(count_str, as_dict=True, placeholders=order)[0]["count(*)"])
            
            # collate all ops who worked on parts of this order
            ops = db.executesql("select Operator from orderassignments where Operator is not null and OrderNumber = %(OrderNumber)s", as_dict=True, placeholders=order)
            order["Operators"] = [op["Operator"] for op in ops]
            
            # get minimum order start and maximum order finish for order parts
            last_finish = db.executesql("select PartFinish from orderassignments where OrderNumber = %(OrderNumber)s order by PartFinish desc limit 1;", as_dict=True, placeholders=order)
            order["OrderFinish"] = last_finish[0]["PartFinish"] if last_finish else None
            
            first_start = db.executesql("select PartStart from orderassignments where OrderNumber = %(OrderNumber)s order by PartStart asc limit 1;", as_dict=True, placeholders=order)
            order["OrderStart"] = first_start[0]["PartStart"] if first_start else None
            
            # if the order isn't finished, don't return a finish time
            if order["OrderStatus"] in [STATUS_WAITING, STATUS_IN_PROGRESS]:
                order["OrderFinish"] = None
                
            # fetch the items for the order
            order["Items"] = get("orderdetails", {"OrderNumber": order["OrderNumber"]})
        
        return response.json(response_data)

    def POST(*args, **kwargs):
        data = request.body.read()
        
        try:
            data = json.loads(data)
        except:
            raise HTTP(400, "invalid json")
        
        if len(args) > 1:
            raise HTTP(400, "too many arguments")
        
        # if we have one order, one arg can be ordernumber
        if type(data) != list:
            if len(args) == 1:
                try:
                    data["OrderNumber"] = int(args[0])
                except:
                    raise HTTP(400, "invalid argument")
            elif len(args) != 0:
                raise HTTP(400, "too many arguments")
            data = [data]
        # if we have multiple orders and any args, complain
        elif len(args) != 0:
            raise HTTP(400, "too many arguments")
        
        orders_added = []
        
        for order in data:
            orders_added.append(__add_order(order))
        
        return response.json(orders_added)
        
    def PUT(*args, **kwargs):
        data = request.body.read()

        try:
            orders = json.loads(data)
        except:
            raise HTTP(400, "invalid json")
            
        if type(orders) is not list:
            if len(args) == 1:
                try:
                    orders["OrderNumber"] = int(args[0])
                except:
                    raise HTTP(400, "invalid argument")
            elif len(args) != 0:
                raise HTTP(400, "too many arguments")
            orders = [orders]
            
        orders_updated = []
        
        for order in orders:
            old_order = get("orders", {"OrderNumber": order["OrderNumber"]})
            if old_order:
                old_order = old_order[0]
                
            orders_updated.append(__add_order(order, update=True))
            
            # if we just added an SMS number to the order and it's waiting, alert
            if __get_order_status(order["OrderNumber"]) == STATUS_WAITING and "OrderSMS" in order:
                if (("OrderSMS" not in old_order) or (old_order["OrderSMS"] is None)):
                    __new_order_sms(get("orders", {"OrderNumber": order["OrderNumber"]})[0])
        
        return response.json(orders_updated)
        
    def DELETE(*args, **kwargs):
        raise HTTP(405, "delete not supported")
       
    return locals()

@request.restful()
def products():
    response.headers["Access-Control-Allow-Origin"] = '*'
    
    def GET(*args, **kwargs):
        response_data = ""
        conditions = {}
        
        if len(args) == 2 and args[1] == "popular":
            # select up to 48 most common products from orderdetails
            # TODO: this is probably slow, cache the results
            try:
                DeptNum = int(args[0])
            except:
                raise HTTP(400, "invalid argument")
                
            sql_str = "select a.* from prods a inner join ( select PrNum, count(PrNum) as `pop` from orderdetails where DeptNum = %(DeptNum)s group by PrNum order by `pop` desc limit 48 ) b on a.PrNum = b.PrNum;"
            try:
                response_data = db.executesql(sql_str, as_dict=True, placeholders={"DeptNum": DeptNum})
            except:
                raise HTTP(400, "failed to query database")
        
            return response.json(response_data, default=str)
        elif len(args) == 2:
            try:
                conditions = {"DeptNum": int(args[0]), "PrNum": int(args[1])}
            except:
                raise HTTP(400, "invalid arguments")
        elif len(args) == 1 and args[0] == "all":
            pass
        elif len(args) != 0:
            raise HTTP(400, "invalid arguments")
            
        conditions.update(validate_args("prods", kwargs))
        
        response_data = get("prods", conditions)
        
        return response.json(response_data)

    def POST(*args, **kwargs):
        data = request.body.read()

        try:
            data = json.loads(data)
        except:
            raise HTTP(400, "invalid data")
            
        if type(data) != list:
            data = [data]
        
        for prod in data:
            if "PrNum" not in prod:
                raise HTTP(400, "invalid prod")
            logger.info( "products() #1 - POST PLU: %s " % prod["PrNum"] )
            
        inserted = insert("prods", data, update=False, parse_json=False)
        
        return response.json({"products_inserted": inserted})

    def PUT(*args, **kwargs):
        data = request.body.read()

        try:
            data = json.loads(data)
        except:
            raise HTTP(400, "invalid data")
            
        if type(data) != list:
            data = [data]
        
        for prod in data:
            if "PrNum" not in prod:
                raise HTTP(400, "invalid prod")
            logger.info( "products() #1 - PUT PLU: %s " % prod["PrNum"] )
            
        inserted = insert("prods", data, update=True, parse_json=False)
        
        return response.json({"products_updated": inserted})

    def DELETE(*args, **kwargs):
        conditions = {}

        if len(args) == 2:
            try:
                conditions = {"DeptNum": int(args[0]), "PrNum": int(args[1])}
            except:
                raise HTTP(400, "invalid arguments")
        elif len(args) == 1 and args[0] == "all":
            pass
        else:
            raise HTTP(400, "invalid arguments")
        
        logger.info( "products() - Delete Product: %s " % conditions )
        delete("prods", conditions)
        
        return response.json(conditions)
        
    return locals()

@request.restful()
def settings():
    response.headers["Access-Control-Allow-Origin"] = '*'
    
    def GET(*args, **kwargs):
        response_data = {}
        count = 1
        defaults = False
        
        if len(args) == 1:
            if args[0] == "history":
                count = 10
            elif args[0] == "defaults":
                defaults = True
        elif len(args) != 0:
            raise HTTP(400, "invalid arguments")
            
        sql_str = "select * from settings"
        
        if defaults:
            sql_str += " where id = 1"
            
        sql_str += " order by id desc limit %s;" % count
        
        try:
            response_data = db.executesql(sql_str, as_dict=True)
        except:
            raise HTTP(500, "failed to query database")
        
        return response.json(response_data, default=str)
        
    def POST(*args, **kwargs):
        if len(args) != 0:
            raise HTTP(400, "invalid arguments")
            
        try:
            data = json.loads(request.body.read())
        except:
            raise HTTP(400, "invalid json")
        
        logger.info( "settings() - Insert Settings: %s " % data)
        inserted = insert("settings", data, update=True, parse_json=False)
        
        return response.json({"items_inserted": inserted})
        
    def PUT(*args, **kwargs):
        raise HTTP(405, "put not supported")
        
    def DELETE(*args, **kwargs):
        raise HTTP(405, "delete not supported")
        
    return locals()
    
@request.restful()
def operators():
    response.headers["Access-Control-Allow-Origin"] = '*'
    def GET(*args, **kwargs):
        conditions = kwargs
        
        if len(args) == 1 and args[0] == "all":
            pass
        elif len(args) == 1:
            try:
                conditions["OpNum"] = int(args[0])
            except:
                raise HTTP(400, "invalid arguments")
        
        return response.json(get('opers', conditions))

    def POST(*args, **kwargs):
        if len(args) != 0:
            raise HTTP(400, "invalid arguments")
        
        data = request.body.read()
        
        try:
            data = json.loads(data)
        except:
            raise HTTP(400, "invalid json")
        
        if type(data) != list:
            data = [data]
        
        logger.info( "operators() #1 - POST OpNum: %s " % data )
        insert("opers", data, parse_json=False)
        
        return response.json({"operators_added": len(data)})

    def PUT(*args, **kwargs):
        data = request.body.read()
        
        try:
            data = json.loads(data)
            if len(args) == 1:
                if type(data) == list:
                    raise HTTP(400, "invalid arguments")
                data["OpNum"] = int(args[0])
            elif len(args) != 0:
                raise HTTP(400, "invalid arguments")
        except:
            raise HTTP(400, "invalid arguments")

        if type(data) != list:
            data = [data]
        
        logger.info( "operators() #2 - PUT OpNum: %s " % data )
        insert("opers", data, parse_json=False, update=True)
        
        return response.json({"operators_updated": 1})

    def DELETE(*args, **kwargs):
        conditions = {}
        if len(args) > 1:
            raise HTTP(400, "invalid arguments")
        if len(args) == 1 and args[0] == "all":
            pass
        elif len(args) == 1:
            try:
                conditions = {"OpNum": int(args[0])}
            except:
                raise HTTP(400, "invalid arguments")
        
        logger.info( "operators() - DELETE OpNum: %s " % conditions )
        delete("opers", conditions)
        return response.json(conditions)
    
    return locals()
    
@request.restful()
def departments():
    response.headers["Access-Control-Allow-Origin"] = '*'
    def GET(*args, **kwargs):
        conditions = kwargs
        
        if len(args) == 1 and args[0] != "all":
            try:
                conditions["DeptNum"] = int(args[0])
            except:
                raise HTTP(400, "invalid arguments")
        
        return response.json(get('depts', conditions))

    def POST(*args, **kwargs):
        if len(args) != 0:
            raise HTTP(400, "invalid arguments")
        
        data = request.body.read()
        
        try:
            data = json.loads(data)
        except:
            raise HTTP(400, "invalid json")
        
        if type(data) != list:
            data = [data]
            
        for dept in data:
            if "DeptNum" not in dept:
                raise HTTP(400, "invalid dept")
            logger.info( "departments() #1 - POST DeptNum: %s " % dept["DeptNum"] )
        
        insert("depts", data, parse_json=False)
        
        for dept in data:
            insert("zones", {"DeptNum": dept["DeptNum"], "ZoneNum": 0, "ZoneName": "Default Zone"}, update=True, parse_json=False)
        
        return response.json({"depts_added": len(data)})

    def PUT(*args, **kwargs):
        data = request.body.read()
        
        try:
            data = json.loads(data)
            if len(args) == 1:
                if type(data) == list:
                    raise HTTP(400, "invalid arguments")
                data["DeptNum"] = int(args[0])
            elif len(args) != 0:
                raise HTTP(400, "invalid arguments")
        except:
            raise HTTP(400, "invalid arguments")
            
        if type(data) != list:
            data = [data]
        
        for dept in data:
            if "DeptNum" not in dept:
                raise HTTP(400, "invalid dept")
            logger.info( "departments() #1 - PUT DeptNum: %s " % dept["DeptNum"] )
            
        insert("depts", data, parse_json=False, update=True)
        
        for dept in data:
            insert("zones", {"DeptNum": dept["DeptNum"], "ZoneNum": 0, "ZoneName": "Default Zone"}, update=True, parse_json=False)
        
        return response.json({"depts_updated": len(data)})

    def DELETE(*args, **kwargs):
        # never delete department zero
        conditions = {"DeptNum.neq": "0"}
        
        if len(args) > 1:
            raise HTTP(400, "invalid arguments")
        if len(args) == 1 and args[0] == "all":
            pass
        elif len(args) == 1:
            try:
                conditions = {"DeptNum": int(args[0])}
            except:
                raise HTTP(400, "invalid arguments")
                
        logger.info( "departments() - DELETE DeptNum: %s " % conditions )
        delete("depts", conditions)
        
        return response.json([depts_deleted])
    
    return locals()
    
def ip():
    return request.client
    
def version():
    response.headers["Access-Control-Allow-Origin"] = '*'
    
    config = get('blackboxconfig', {})
    out = {"deviceType": "BlackBox"}
    out["applicationVersion"] = [item["ColValue"] for item in config if item["ColName"] == "applicationVersion"][0]
    out["databaseVersion"] = [item["ColValue"] for item in config if item["ColName"] == "databaseVersion"][0]
    return response.json(out)

@request.restful()
def departmentsettings():
    response.headers["Access-Control-Allow-Origin"] = '*'
    def GET(*args, **kwargs):
        conditions = kwargs
        
        if len(args) == 1:
            try:
                conditions["DeptNum"] = int(args[0])
            except:
                raise HTTP(400, "invalid arguments")
        
        response_data = get('deptssettings', conditions)
        
        if type(response_data) != list:
            response_data = [response_data]
        
        for dept in response_data:
            dept_name_str = "select DeptName1 from depts where DeptNum = %s;" % dept['DeptNum']
            dept["DeptName1"] = db.executesql(dept_name_str, as_dict=True)[0]["DeptName1"]
        
        return response.json(response_data, default=str)

    def POST(*args, **kwargs):
        if len(args) != 0:
            raise HTTP(400, "invalid arguments")
        
        data = request.body.read()
        
        try:
            data = json.loads(data)
        except:
            raise HTTP(400, "invalid json")
        
        if type(data) != list:
            data = [data]
            
        for dept in data:
            if "DeptNum" not in dept:
                raise HTTP(400, "invalid dept")
            logger.info( "departmentsettings() #1 - POST DeptNum: %s " % dept["DeptNum"] )
        
        insert("deptssettings", data, parse_json=False)
        
        return response.json({"deptssettings_added": len(data)})

    def PUT(*args, **kwargs):
        data = request.body.read()
        
        try:
            data = json.loads(data)
            if len(args) == 1:
                if type(data) == list:
                    raise HTTP(400, "invalid arguments")
                data["DeptNum"] = int(args[0])
            elif len(args) != 0:
                raise HTTP(400, "invalid arguments")
        except:
            raise HTTP(400, "invalid arguments")
            
        if type(data) != list:
            data = [data]
        
        for dept in data:
            if "DeptNum" not in dept:
                raise HTTP(400, "invalid dept")
            logger.info( "departmentsettings() #2 - PUT DeptNum: %s " % dept["DeptNum"] )
            
        insert("deptssettings", data, parse_json=False, update=True)
        
        return response.json({"deptssettings_updated": len(data)})

    def DELETE(*args, **kwargs):
        conditions = {}
        if len(args) > 1:
            raise HTTP(400, "invalid arguments")
        
        elif len(args) == 1:
            try:
                conditions = {"DeptNum": int(args[0])}
            except:
                raise HTTP(400, "invalid arguments")
        
        if "DeptNum" not in conditions:
            raise HTTP(400, "invalid dept")
        
        logger.info( "departmentsettings() #3 - DELETE DeptNum: %s " % conditions["DeptNum"] )
        
        delete("deptssettings", conditions)
        return response.json(conditions)
    
    return locals()

@request.restful()
def categories():
    response.headers["Access-Control-Allow-Origin"] = '*'
    
    def GET(*args, **kwargs):
        conditions = kwargs
        
        if len(args) == 1 and args[0] == "all":
            pass
        elif len(args) == 1:
           try:
                conditions["DeptNum"] = int(args[0])
           except:
                raise HTTP(400, "invalid arguments")
        elif len(args) == 2:
            try:
                conditions["DeptNum"] = int(args[0])
                conditions["CaNum"] = int(args[1])
            except:
                raise HTTP(400, "invalid arguments")
        elif len(args) > 0:
            raise HTTP(400, "invalid arguments")
        
        return response.json(get('categrs', conditions))

    def POST(*args, **kwargs):
        if len(args) != 0:
            raise HTTP(400, "invalid arguments")
        
        data = request.body.read()
        
        try:
            data = json.loads(data)
        except:
            raise HTTP(400, "invalid json")
        
        if type(data) != list:
            data = [data]
            
        for category in data:
            if "CaNum" not in category:
                raise HTTTP(400, "no CaNum")
            logger.info( "categories() #1 - POST CaNum: %s " % category["CaNum"] )
            
        insert("categrs", data, parse_json=False, update=False)
        
        return response.json({"categories_added": len(data)})

    def PUT(*args, **kwargs):
        if len(args) != 0:
            raise HTTP(400, "invalid arguments")
        
        data = request.body.read()
        
        try:
            data = json.loads(data)
        except:
            raise HTTP(400, "invalid json")
        
        if type(data) != list:
            data = [data]
        
        for category in data:
            if "CaNum" not in category:
                raise HTTTP(400, "no CaNum")
            logger.info( "categories() #1 - PUT CaNum: %s " % category["CaNum"] )
            
        insert("categrs", data, parse_json=False, update=True)
        
        return response.json({"categories_added": len(data)})

    def DELETE(*args, **kwargs):
        conditions = {}
        
        if len(args) == 1 and args[0] == "all":
            pass
        elif len(args) == 1:
            try:
                conditions["DeptNum"] = int(args[0])
            except:
                raise HTTP(400, "invalid arguments")
        elif len(args) == 2:
            try:
                conditions["DeptNum"] = int(args[0])
                conditions["CaNum"] = int(args[1])
            except:
                raise HTTP(400, "invalid arguments")
        else:
            raise HTTP(400, "invalid arguments")
        
        logger.info( "categories() - DELETE category: %s " % conditions )
        delete("categrs", conditions)
        return response.json(conditions)
    
    return locals()

def __legal_file_name(name):
    return all([c in LEGAL_CHARS for c in name])

def __legal_image_path(p):
    if p.count("/") > 1:
        return False
        
    if p.split("/")[0] not in  ["categories", "products", "uploads"]:
        return False
        
    if not __legal_file_name(p.split("/")[1]):
        return False
    
    return True

@request.restful()
def images():
    response.headers["Access-Control-Allow-Origin"] = '*'
    
    # allow POSTing the image as formdata?
    '''def POST(*args, **kwargs):
        if len(args) != 1 or "data" not in request.vars:
            raise HTTP(400)
        
        location = args[0]
        
        if location not in ["products", "categories"]:
            raise HTTP(400)
        
        filename = request.post_vars["data"].filename
        
        if not __legal_file_name(filename):
            raise HTTP(400)
        
        # save the file, overwrite if something is already there
        with open("/var/www/web2py/applications/OnlineOrdering/static/images/"+location+"/"+filename, "wb") as f:
            f.seek(0)
            f.write(request.post_vars["data"].file.read())
            f.truncate()
        
        return response.json({"files_added": 1})'''
        
    # comment: having to go through web2py is 10x slower than just lighttpd
    # so get the images from the actual static location when possible
    def GET(*args, **kwargs):
        if len(args) == 2 and args[0] in ["products", "categories", "uploads"]:
            if args[1] == "all":
                # only return user-uploaded images, not system ones
                system_images = set(os.listdir(SYSTEM_IMAGE_PATH + args[0]+ "/")) if args[0] != "uploads" else set()
                all_images = set(os.listdir(HOST_IMAGE_PATH + args[0]+ "/"))
                return response.json(list(all_images - system_images))
            # try to return the actual image if the name is given
            # n.b. raw_args preserves spaces and doesn't split the args
            elif __legal_image_path(request.raw_args):
                try:
                    return response.stream(open(HOST_IMAGE_PATH+request.raw_args,'rb'), filename=request.raw_args.split("/")[1])
                except:
                    raise HTTP(404, "no such image")
            else:
                raise HTTP(400, "invalid arguments")
        
        if len(args) != 3:
            raise HTTP(400, "invalid arguments")
        
        location = args[0]
        if location not in ["products", "categories", "uploads"]:
            raise HTTP(404, "invalid image type")
        
        DeptNum = args[1]
        ThingNum = args[2]
        
        image = None
        
        # look up the image name
        if location == "categories":
            sql_str = "select CaImage from categrs where DeptNum = %(DeptNum)s and CaNum = %(CaNum)s;"
            image = db.executesql(sql_str, placeholders={"DeptNum": DeptNum, "CaNum": ThingNum}, as_dict=True)[0]["CaImage"]
        elif location == "products":
            sql_str = "select PrImage from prods where DeptNum = %(DeptNum)s and PrNum = %(PrNum)s;"
            image = db.executesql(sql_str, placeholders={"DeptNum": DeptNum, "PrNum": ThingNum}, as_dict=True)[0]["PrImage"]
        elif location == "uploads":
            image = ThingNum
        
        # send a redirect to the actual location
        redirect("/OnlineOrdering/static/images/"+location+"/"+image)
        
    def PUT(*args, **kwargs):
        raise HTTP(405, "PUT not supported on images")
        
    def POST(*args, **kwargs):        
        if len(args) != 1:
            raise HTTP(400, "invalid arguments")
        
        location = args[0]
        
        if location not in ["products", "categories", "uploads"]:
            raise HTTP(400, "invalid location")
            
        data = request.body.read()
        try:
            data = json.loads(data)
        except:
            raise HTTP(400, "invalid json")
            
        if "filename" not in data or "image" not in data:
            raise HTTP(400, "invalid filename")
            
        if not __legal_file_name(data["filename"]):
            raise HTTP(400, "invalid filename")
        
        image = None
        try:
            image = base64.b64decode(data["image"])
        except:
            raise HTTP(400, "invalid image data")
        
        # save the file, overwrite if something is already there
        target = HOST_IMAGE_PATH+location+"/"+data["filename"]
        with open(target, "wb") as f:
            f.seek(0)
            f.write(image)
            f.truncate()
            
        logger.info( "images() - POST: filename %s saved to location %s" % (data["filename"], target) )
        
        return response.json({"files_added": 1})
        
    def DELETE(*args, **kwargs):
        if len(args) != 2:
            raise HTTP(400, "invalid arguments")
            
        location = args[0]
        
        if location not in ["products", "categories", "uploads"]:
            raise HTTP(400, "invalid location")

        filenames = []
        
        if args[1] == "all":
            filenames = os.listdir(HOST_IMAGE_PATH + args[0]+ "/")
        else:
            if not __legal_image_path(request.raw_args):
                raise HTTP(400, "invalid image name: " + request.raw_args)
            filenames = [request.raw_args.split("/")[1]]
        
        for filename in filenames:
            current_file_path = HOST_IMAGE_PATH+location+"/"+filename
            possible_system_path = SYSTEM_IMAGE_PATH+location+"/"+filename
            
            if os.path.isfile(current_file_path):
                logger.info( "images() - DELETE: filename %s deleted" % (location+"/"+filename))
                os.remove(current_file_path)
            
            # if we have a system file of the same name, copy it back
            if os.path.isfile(possible_system_path):
                logger.info( "images() - DELETE: restoring system file %s" % possible_system_path)
                shutil.copyfile(possible_system_path, HOST_IMAGE_PATH+location+"/"+filename)
                
        return response.json({"images_deleted":len(filenames)})
    
    return locals()
    
@request.restful()
def queue():
    response.headers["Access-Control-Allow-Origin"] = '*'
    
    def GET(*args, **kwargs):
        if len(args) != 1:
            raise HTTP(400, "too many arguments")
        
        dept = int(args[0])
        
        out = {}
        sql_str = "select count(*) from orderparts where OrderOrigin = 3 and PartStatus >=1 and PartStatus <= 2 and DeptNum = %(DeptNum)s"
        out["QueueLength"] = db.executesql(sql_str, placeholders={"DeptNum": dept}, as_dict=True)[0]["count(*)"]
        
        sql_str = "select OrderNumber from orderparts where OrderOrigin = 3 and PartStatus >=1 and PartStatus <= 2 and DeptNum = %(DeptNum)s order by OrderNumber asc"
        next_customer = db.executesql(sql_str, placeholders={"DeptNum": dept}, as_dict=True)
        
        if len(next_customer) > 0:
            out["NowServing"] = next_customer[0]["OrderNumber"]
        else:
            out["NowServing"] = 0
        
        # TODO: better estimation algorithm
        out["EstimatedWait"] = 3 * out["QueueLength"]
        
        return response.json(out)
        
    def PUT(*args, **kwargs):
        raise HTTP(405)
        
    def POST(*args, **kwargs):
        raise HTTP(405)
        
    def DELETE(*args, **kwargs):
        raise HTTP(405)
    
    return locals()

def __close_today():
    weekday_close = ["MondayClose","TuesdayClose","WednesdayClose","ThursdayClose","FridayClose","SaturdayClose","SundayClose"]
    
    close = str(get_setting(weekday_close[datetime.datetime.now().weekday()]))

    h,m,s = [int(i) for i in close.split(":")]
    today = datetime.datetime.today()
    
    return datetime.datetime(year=today.year, month=today.month, day=today.day, hour=h, minute=m, second=s)

@request.restful()
def timeslots():
    response.headers["Access-Control-Allow-Origin"] = '*'
    
    def GET(*args, **kwargs):
        if len(args) != 1:
            raise HTTP(400, "too many arguments")
            
        dept = int(args[0])
        
        slots_open = []
        
        # slots per hour
        slot_count = int(get_setting("HourTimeSlots"))
        
        # minutes per slot
        slot_minutes = math.floor(60/slot_count)

        orders_per_slot = int(get_setting("OrdersPerSlot"))
        
        time_increment = datetime.timedelta(minutes=slot_minutes)

        # gets all orderparts, grouped into timeslots, with a count per timeslot
        # returns nothing for empty timeslots
        sql_str = "select HOUR(`OrderPickup`) as hour, FLOOR(MINUTE(`OrderPickup`)/%(slot_minutes)s)*%(slot_minutes)s as minute, COUNT(*) as count from orderparts where DeptNum = %(DeptNum)s and DATE(`OrderPickup`) = CURDATE() group by DATE(`OrderPickup`), HOUR(`OrderPickup`), FLOOR(MINUTE(`OrderPickup`)/%(slot_minutes)s) order by OrderNumber asc;"
        slots_used = db.executesql(sql_str, placeholders={"slot_minutes": slot_minutes, "DeptNum": dept}, as_dict=True)
        
        # convert into dict like {"HH:MM:00": 7}
        slots_used = {str(x["hour"])+":"+str(x["minute"])+":00": x["count"] for x in slots_used}
        
        # convert a datetime into the start of its slot
        slotify = lambda i: datetime.time(i.hour, math.floor(i.minute / slot_minutes)*slot_minutes)
        
        # to offer slots, start with now + minimum wait setting
        next_time = datetime.datetime.now() + datetime.timedelta(minutes=int(get_setting("MinimumOrderWait")))
        
        close = __close_today()
        
        # iterate candidate times and check if the slots are open
        while next_time < close:
            maybe = next_time.strftime("%I:%M %p").lstrip("0")

            count = slots_used.get(slotify(next_time).strftime("%H:%M:00").lstrip("0"), -1)

            if count < orders_per_slot:
                slots_open.append([maybe, next_time.strftime("%Y-%m-%d %H:%M:%S")])
            next_time += time_increment
        
        # finally, return a list of things like [["4:00 PM", "2022-01-01 16:00:00"]]
        return response.json(slots_open)
        
    def PUT(*args, **kwargs):
        raise HTTP(405)
        
    def POST(*args, **kwargs):
        raise HTTP(405)
        
    def DELETE(*args, **kwargs):
        raise HTTP(405)
        
    return locals()

def __change_order_status(OrderNumber, new_status):
    insert("orders", {"OrderNumber": OrderNumber, "OrderStatus": new_status}, update=True, parse_json=False)

@request.restful()
def orderparts():
    response.headers["Access-Control-Allow-Origin"] = '*'
    
    def GET(*args, **kwargs):
        try:
            if len(args) == 1:
                kwargs["OrderNumber"] = int(args[0])
            elif len(args) == 2:
                kwargs["OrderNumber"] = int(args[0])
                kwargs["DeptNum"] = int(args[1])
            elif len(args) == 3:
                kwargs["OrderNumber"] = int(args[0])
                kwargs["DeptNum"] = int(args[1])
                kwargs["ZoneNum"] = int(args[2])
            elif len(args) > 3:
                raise HTTP(400, "too many arguments")
        except:
            raise HTTP(400, "invalid argument")
        
        response_data = get('orderparts', kwargs)
        
        return response.json(response_data)
        
    def PUT(*args, **kwargs):
        data = request.body.read()
        OrderNumber = None
        
        try:
            data = json.loads(data)
        except:
            raise HTTP(400, "invalid json")
            
        if len(args) > 3:
            raise HTTP(400, "invalid arguments")

        # an orderparts must be fully specified to change
        try:
            if "OrderNumber" not in data:
                data["OrderNumber"] = int(args[0])
            if "DeptNum" not in data:
                data["DeptNum"] = int(args[1])
            if "ZoneNum" not in data:
                data["ZoneNum"] = int(args[2])
        except:
            raise HTTP(400, "invalid arguments")
            
        new_part_status = data["PartStatus"] if "PartStatus" in data else None

        try:
            old_part = get("orderassignments", {"OrderNumber": data["OrderNumber"], "ZoneNum": data["ZoneNum"], "DeptNum": data["DeptNum"]})[0]
            old_part_status = old_part["PartStatus"] if "PartStatus" in old_part else None
        except:
            raise HTTP(400, "no such part")
            

        if new_part_status is not None:
            # if we have no start and have started, get one
            if not ("PartStart" in old_part and old_part["PartStart"]) and new_part_status in [STATUS_IN_PROGRESS, STATUS_READY]:
                data["PartStart"] = __now()
            
            # if we have no finish and have finished, get one
            if not ("PartFinish" in old_part and old_part["PartFinish"]) and new_part_status in [STATUS_READY, STATUS_CANCELLED]:
                data["PartFinish"] = __now()
        
        logger.info( "orderparts() #2 - PUT Order: %s " % data )

        insert("orderassignments", data, update=True, parse_json=False)
        
        # if we're changing the part status, we need to propagate that around
        if new_part_status is not None:
            order = get("orders", {"OrderNumber": data["OrderNumber"]})[0]
            
            # if this part is newly in progress  
            if new_part_status == STATUS_IN_PROGRESS and old_part_status == STATUS_WAITING:
                # if the order isn't started
                if order["OrderStatus"] == STATUS_WAITING:
                    # start the order
                    __change_order_status(data["OrderNumber"], STATUS_IN_PROGRESS)
                    order["OrderStatus"] = STATUS_IN_PROGRESS
                    __order_started(order)
            
            # get all zone statuses
            zone_statuses = db.executesql("select OrderNumber, ZoneNum, PartStatus, DeptNum from orderassignments where OrderNumber = %(OrderNumber)s", as_dict=True, placeholders=data)
            
            # if this part is newly ready
            if new_part_status == STATUS_READY and old_part_status in [STATUS_WAITING, STATUS_IN_PROGRESS]:
                # if all parts are complete or cancelled
                if all([int(part["PartStatus"]) in [STATUS_READY, STATUS_CANCELLED] for part in zone_statuses]):
                    # if at least one part ISN'T cancelled
                    if any([int(part["PartStatus"]) != STATUS_CANCELLED for part in zone_statuses]):
                        # mark it as complete
                        __change_order_status(data["OrderNumber"], STATUS_READY)
                        order["OrderStatus"] = STATUS_READY
                        __order_ready(order)
                        
            # if this part is newly cancelled
            if new_part_status == STATUS_CANCELLED and old_part_status in [STATUS_WAITING, STATUS_IN_PROGRESS, STATUS_READY]:
                # if all parts are cancelled
                 if all([int(part["PartStatus"]) == STATUS_CANCELLED for part in zone_statuses]):
                    __change_order_status(data["OrderNumber"], STATUS_CANCELLED)
                    order["OrderStatus"] = STATUS_READY 
                    __order_cancelled(order)
            
            # if we are restarting this part
            if new_part_status in [STATUS_WAITING, STATUS_IN_PROGRESS] and old_part_status in [STATUS_CANCELLED, STATUS_READY]:
                # if all parts are waiting, make order waiting
                if all([int(part["PartStatus"]) == STATUS_WAITING for part in zone_statuses]):
                    __change_order_status(data["OrderNumber"], STATUS_WAITING)
                else:
                    __change_order_status(data["OrderNumber"], STATUS_IN_PROGRESS)

        return response.json([])
        
    # orderparts will always be autocreated
    def POST(*args, **kwargs):
        raise HTTP(405)
        
    def DELETE(*args, **kwargs):
        raise HTTP(405)
        
    return locals()

@request.restful()
def zones():
    # TODO this is helpful for dev stuff, consider implications
    response.headers["Access-Control-Allow-Origin"] = '*'
    
    def GET(*args, **kwargs):
        conditions = kwargs

        try:
            if not (len(args) == 1 and args[0] == "all"):
                if len(args) >= 1:
                    conditions["DeptNum"] = int(args[0])
                if len(args) >= 2:
                    conditions["ZoneNum"] = int(args[1])
        except:
            raise HTTP(400, "invalid arguments")
        
        if len(args) > 2:
            raise HTTP(400, "invalid arguments")
        
        return response.json(get('zones', conditions))

    def POST(*args, **kwargs):
        if len(args) != 0:
            raise HTTP(400, "invalid arguments")
        
        data = request.body.read()
        
        try:
            data = json.loads(data)
        except:
            raise HTTP(400, "invalid json")
        
        if type(data) != list:
            data = [data]
        
        for zone in data:
            logger.info( "zones() #1 - POST zone: %s " % zone )
        
        insert("zones", data, parse_json=False)
        
        return response.json({"zones_added": len(data)})

    def PUT(*args, **kwargs):
        data = request.body.read()
            
        if type(data) != list:
            data = [data]
            
        try:
            for zone in data:
                if len(args) >= 1:
                    zone["DeptNum"] = int(args[0])
                if len(args) >= 2:
                    zone["ZoneNum"] = int(args[1])
            if len(args) > 2:
                raise HTTP(400, "invalid arguments")
        except:
            raise HTTP(400, "invalid arguments")
            
        for zone in data:
            logger.info( "zones() #2 - PUT zone: %s " % zone )
            
        insert("zones", data, parse_json=False, update=True)
        
        return response.json({"zones_updated": len(data)})

    def DELETE(*args, **kwargs):
        # never delete zone zero
        conditions = kwargs
        conditions["ZoneNum.neq"] = 0
        
        if len(args) > 2:
            raise HTTP(400, "invalid arguments")
        if len(args) == 1 and args[0] == "all":
            pass
        elif len(args) == 1:
            try:
                conditions = {"DeptNum": int(args[0])}
            except:
                raise HTTP(400, "invalid arguments")
        elif len(args) == 2:
            try:
                conditions = {"DeptNum": int(args[0]), "ZoneNum": int(args[1])}
            except:
                raise HTTP(400, "invalid arguments")
        
        logger.info( "zones() - DELETE zone: %s" % conditions )
        delete("zones", conditions)
        return response.json([])
    
    return locals()

@request.restful()
def logs():
    # TODO this is helpful for dev stuff, consider implications
    response.headers["Access-Control-Allow-Origin"] = '*'
    
    def GET(*args, **kwargs):
        # TODO couldn't find a nice way to introspect the logger and get the filename
        # TODO so it's hard coded right now
        log_path = "/var/www/web2py/web2py.log"
        log_name = "oo-server-" + str(int(time.time())) + ".log"

        # TODO will this have issues if the file is huge?
        if len(args) == 1 and args[0] == "json":
            with open(log_path,'r') as f:
                return response.json(f.readlines())
        else:
            return response.stream(log_path, filename=log_name, attachment=True)
        
    def PUT(*args, **kwargs):
        raise HTTP(405, "PUT not supported")
        
    def POST(*args, **kwargs):
        raise HTTP(405, "POST not supported")
        
    def DELETE(*args, **kwargs):
        raise HTTP(405, "DELETE not supported")

    return locals()
