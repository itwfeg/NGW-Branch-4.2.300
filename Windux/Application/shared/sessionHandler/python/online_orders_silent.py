#!/usr/bin/env python3
# same as online_orders_script.py but silent

from online_orders_script import run_script, show_error
    
if __name__ == "__main__":
    try:
        run_script(sound=False)
    except:
        show_error()

