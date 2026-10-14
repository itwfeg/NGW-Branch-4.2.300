from gluon import current, URL
from pygal.style import DarkStyle
import time
import modbus_consumer
import os

def sql_month_day(datetime_type):
    return str(datetime_type)[5:10]


def get_plot_settings(zoom, group_by):
    settings = {'height': 600,
                'width': 800,
                'spacing': 10,
                'interpolation': 'cubic',
                'style': None,
                'x_title': 'Date'}

    font_size = 20.0

    if zoom == '1':
        settings = {'height': 600 * 2,
                    'width': 800 * 2,
                    'spacing': 10 * 2,
                    'x_title': 'Date'}
        font_size = font_size * 2.0

    if group_by == 'hour':
        settings['x_title'] = 'Date + Hour'
    elif group_by == 'minute':
        settings['x_title'] = 'Date + Hour:Minute'

    DarkStyle.title_font_size = font_size
    DarkStyle.label_font_size = font_size
    DarkStyle.legend_font_size = font_size
    DarkStyle.tooltip_font_size = font_size
    DarkStyle.major_label_font_size = font_size
    DarkStyle.value_font_size = font_size
    settings['style'] = DarkStyle

    return settings


def sql_datetime_to_string(date_time_value, datetime_format="%Y-%m-%d %H:%M:%S"):
    return date_time_value.strftime(datetime_format)


# select strftime('%H', datetime(created_on)) as creation_date, MIN(frequency_level) AS min_level,
# MAX(frequency_level) AS max_level, AVG(frequency_level) AS avg_level FROM psu_data WHERE datetime(created_on)
# between  datetime('2020-01-21 00:00:00') and datetime('2020-01-21 23:59:59') group by strftime("%H", created_on);

def generate_plot_sql_query(plot_type, group_by, start_date, end_date):
    sql = None

    datetime_format = None
    if group_by == 'day':
        display_by_time_format = '%m-%d-%Y'
        group_by_time_format = '%Y-%m-%d'
    elif group_by == 'hour':
        display_by_time_format = "%m-%d-%Y %H"
        group_by_time_format = "%Y-%m-%d %H"
    elif group_by == 'minute':
        display_by_time_format = "%m-%d-%Y %H:%M"
        group_by_time_format = "%Y-%m-%d %H:%M"

    if plot_type == 'supply':

        sql = "SELECT strftime('%s', datetime(created_on)) AS creation_date, AVG(power_level) AS avg_power_level, " \
              "AVG(voltage_level) AS avg_voltage_level, AVG(frequency_level) AS avg_frequency_level, " \
              "AVG(current_level) AS avg_current_level FROM psu_data WHERE " \
              "datetime(created_on) BETWEEN datetime('%s') AND datetime('%s')" \
              " GROUP BY strftime('%s', created_on) order by datetime(created_on);" % \
              (display_by_time_format, start_date, end_date, group_by_time_format)

    elif plot_type == 'faults':

        sql = "select strftime('%s', datetime(created_on)) as creation_date, " \
              "sum(case high_temp_fault when 'T' then 1 else 0 end) as number_high_temp_faults, " \
              "sum(case dc_bus_fault when 'T' then 1 else 0 end) as number_dc_bus_faults," \
              "sum(case interlock_fault when 'T' then 1 else 0 end) as number_interlock_faults, " \
              "sum(case inverter_on_fault when 'T' then 1 else 0 end) as number_inverter_on_faults, " \
              "sum(case overvolt_fault when 'T' then 1 else 0 end) as number_overvolt_faults, " \
              "sum(case current_trip_fault when 'T' then 1 else 0 end) as number_current_trip_faults " \
              "from psu_data where " \
              "datetime(created_on) between datetime('%s') and datetime('%s') group by strftime('%s', created_on) " \
              "order by datetime(created_on);" % \
              (display_by_time_format, start_date, end_date, group_by_time_format)
    else:

        sql = "select strftime('%s', datetime(created_on)) as creation_date, MIN(%s_level) AS min_level," \
              "MAX(%s_level) AS max_level, AVG(%s_level) AS avg_level FROM " \
              "psu_data WHERE datetime(created_on) between datetime('%s') and " \
              "datetime('%s') group by strftime('%s', created_on) order by datetime(created_on);" % (
              display_by_time_format,
              plot_type,
              plot_type,
              plot_type,
              start_date,
              end_date,
              group_by_time_format)
    return sql


def check_usd_card_integrity():
    status, response = modbus_consumer.check_usd_card_integrity()
    return status, response


def format_usd_card_and_db():
    # stop recording by sending signal to modbus_consumer process
    modbus_consumer.stop_modbus_consumer()
    # get the external uSD card device name
    internal_sd_card_device, external_sd_card_device = modbus_consumer.get_sd_card_device_names()
    # format the external SD card
    modbus_consumer.format_usd_card(external_sd_card_device)
    # create database storage links
    # modbus_consumer.create_db_storage_links()
    # start recording and create database
    modbus_consumer.start_modbus_consumer()


def restart_server():
    os.system("/sbin/reboot")


def stop_communication_and_recording():
    modbus_consumer.stop_modbus_consumer()


def start_communication_and_recording():
    modbus_consumer.start_modbus_consumer()

