# -*- coding: utf-8 -*-

import json

def __get_current_settings():
    sql_str = "select * from settings order by id desc limit 1;"
    return db.executesql(sql_str, as_dict=True)[0]

def __get_dept_settings(DeptNum):
    sql_str = "select * from deptssettings where DeptNum = %s;" % DeptNum
    return db.executesql(sql_str, as_dict=True)[0]
    
# save the department number if it's a parameter, or default it to 0
def __set_dept():
    if "DeptNum" in request.vars:
        session.DeptNum = int(request.vars["DeptNum"])
    elif "DeptNum" not in session:
        session.DeptNum = 0

def index():
    __set_dept()
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)
    
def categories():
    __set_dept()
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)

def products():
    __set_dept()
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)
    
def item():
    __set_dept()
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)
    
def cart():
    __set_dept()
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)

def logo():
    __set_dept()
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)
    
def queue():
    __set_dept()
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)
    
def customer():
    __set_dept()
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)

def number():
    __set_dept()
    settings = __get_current_settings()
    return dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings)

# this is used to create a CSS file with configured values for colors, bg, etc.
def kiosk_configured_styles():
    settings = __get_current_settings()

    # if we have department, get the department-specific BG
    if "DeptNum" in session:
        deptssettings = __get_dept_settings(session.DeptNum)
        settings["KioskBGImage"] = deptssettings["KioskDeptBGImage"]
    
    return response.render(dict(currentSettingsJSON=json.dumps(settings, default=str), currentSettings=settings))

