import argparse
import sys
import time


def set_backlight_brightness(state):
    operator_display_device = "/sys/class/backlight/backlight_lvds1/brightness"
    customer_display_device = "/sys/class/backlight/backlight_lvds2/brightness"

    if state == 'full':
        control_value = 10
    else:
        control_value = 1

    for device in [operator_display_device, customer_display_device]:
        with open(device, "w") as f:
            f.write(str(control_value))


def run_cycle_backlight(full_time, dim_time):
    while True:
        print('backlight set to full')
        set_backlight_brightness(state='full')
        time.sleep(float(full_time) * 60.0)

        print('backlight set to dim')
        set_backlight_brightness(state='dim')
        time.sleep(float(dim_time) * 60.0)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="Cycle backlight brightness from full to dim values and repeat, ARK board only")
    parser.add_argument("full_time", type=int, help='time to set full display brightness in minutes')
    parser.add_argument("dim_time", type=int, help='time to dim display brightness in minutes')
    args = parser.parse_args()
    sys.exit(run_cycle_backlight(args.full_time, args.dim_time))
