# -*- coding: utf-8 -*-
# -------------------------------------------------------------------------
# This is a sample controller
# this file is released under public domain and you can use without limitations
# -------------------------------------------------------------------------

# ---- example index page ----
# def index():
#    return dict(message="Hello from Online Ordering w00t")

import json

# returns a string of a JSON object with the current settings
# this is included in the <head> of the "layout.html" template
def __get_current_settings():
    sql_str = "select * from settings order by id desc limit 1;"
    return db.executesql(sql_str, as_dict=True)[0]

def index():
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)

def orderList():
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)

def orderDetails():
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)

def settings():
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)

def deligo():
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)

def logs():
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)

def fillItemPopup():
    args = {"OrderNumber": int(request.args[0]), "OrderNumId": int(request.args[1])}
    details = db.executesql("select * from orderdetails where OrderNumId = %(OrderNumId)s and OrderNumber = %(OrderNumber)s", as_dict=True, placeholders=args)[0]
    order = db.executesql("select * from orders where OrderNumber = %(OrderNumber)s", as_dict=True, placeholders=args)[0]
    settings = __get_current_settings()
    return dict(details=details, currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings, order=order)

def orderSearch():
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)
    
def button_test():
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)
    
def orderEditor():
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)

