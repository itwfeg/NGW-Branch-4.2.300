# -*- coding: utf-8 -*-

import datetime
import corona
import pygal
import os
import time

# delete session command
# python web2py.py -S PillarSync -M -R scripts/sessions2trash.py -A -o -x 0


def index():

    if not session.interpolate_type:
        session.interpolate_type = 'none'

    if not session.group_by:
        session.group_by = 'day'

    if not session.power_start_date:
        session.power_start_date = datetime.datetime.now().replace(microsecond=0)

    if not session.power_end_date:
        session.power_end_date = datetime.datetime.now().replace(microsecond=0) + datetime.timedelta(days=14)

    if not session.voltage_start_date:
        session.voltage_start_date = datetime.datetime.now().replace(microsecond=0)

    if not session.voltage_end_date:
        session.voltage_end_date = datetime.datetime.now().replace(microsecond=0) + datetime.timedelta(days=14)

    if not session.current_start_date:
        session.current_start_date = datetime.datetime.now().replace(microsecond=0)

    if not session.current_end_date:
        session.current_end_date = datetime.datetime.now().replace(microsecond=0) + datetime.timedelta(days=14)

    if not session.frequency_start_date:
        session.frequency_start_date = datetime.datetime.now().replace(microsecond=0)

    if not session.frequency_end_date:
        session.frequency_end_date = datetime.datetime.now().replace(microsecond=0) + datetime.timedelta(days=14)

    if not session.faults_start_date:
        session.faults_start_date = datetime.datetime.now().replace(microsecond=0)

    if not session.faults_end_date:
        session.faults_end_date = datetime.datetime.now().replace(microsecond=0) + datetime.timedelta(days=14)

    if not session.supply_levels_start_date:
        session.supply_levels_start_date = datetime.datetime.now().replace(microsecond=0)

    if not session.supply_levels_end_date:
        session.supply_levels_end_date = datetime.datetime.now().replace(microsecond=0) + datetime.timedelta(days=14)

    power_form = FORM(
        INPUT(_name='power_start_date', _class='datetime', _maxlength=19, _size=19,
              requires=[IS_NOT_EMPTY(), IS_DATETIME()], value=session.power_start_date), LABEL("-"),
        INPUT(_name='power_end_date', _class='datetime', maxlength=19, _size=19,
              requires=[IS_NOT_EMPTY(), IS_DATETIME()], _value=session.power_end_date),BR(),
        INPUT(_type='submit', _value='Plot', _style="font-size:0.90em"),
        INPUT(_type='submit', _name='zoom', _value='Zoom', _style="font-size:0.90em"),
        _name='plot_power_form')

    voltage_form = FORM(
        INPUT(_name='voltage_start_date', _class='datetime', _maxlength=19, _size=19,
              requires=[IS_NOT_EMPTY(), IS_DATETIME()], value=session.voltage_start_date), LABEL("-"),
        INPUT(_name='voltage_end_date', _class='datetime', maxlength=19, _size=19,
              requires=[IS_NOT_EMPTY(), IS_DATETIME()], _value=session.voltage_end_date),BR(),
        INPUT(_type='submit', _value='Plot', _style="font-size:0.90em"),
        INPUT(_type='submit', _name='zoom', _value='Zoom', _style="font-size:0.90em"),
        _name='plot_voltage_form')

    current_form = FORM(
        INPUT(_name='current_start_date', _class='datetime', _maxlength=19, _size=19,
              requires=[IS_NOT_EMPTY(), IS_DATETIME()], value=session.current_start_date), LABEL("-"),
        INPUT(_name='current_end_date', _class='datetime', maxlength=19, _size=19,
              requires=[IS_NOT_EMPTY(), IS_DATETIME()], _value=session.current_end_date),BR(),
        INPUT(_type='submit', _value='Plot', _style="font-size:0.90em"),
        INPUT(_type='submit', _name='zoom', _value='Zoom', _style="font-size:0.90em"),
        _name='plot_current_form')

    frequency_form = FORM(
        INPUT(_name='frequency_start_date', _class='datetime', _maxlength=19, _size=19,
              requires=[IS_NOT_EMPTY(), IS_DATETIME()], value=session.frequency_start_date), LABEL("-"),
        INPUT(_name='frequency_end_date', _class='datetime', maxlength=19, _size=19,
              requires=[IS_NOT_EMPTY(), IS_DATETIME()], _value=session.frequency_end_date),BR(),
        INPUT(_type='submit', _value='Plot', _style="font-size:0.90em"),
        INPUT(_type='submit', _name='zoom', _value='Zoom', _style="font-size:0.90em"),
        _name='plot_frequency_form')

    faults_form = FORM(
        INPUT(_name='faults_start_date', _class='datetime', _maxlength=19, _size=19,
              requires=[IS_NOT_EMPTY(), IS_DATETIME()], value=session.faults_start_date), LABEL("-"),
        INPUT(_name='faults_end_date', _class='datetime', maxlength=19, _size=19,
              requires=[IS_NOT_EMPTY(), IS_DATETIME()], _value=session.faults_end_date),BR(),
        INPUT(_type='submit', _value='Plot', _style="font-size:0.90em"),
        INPUT(_type='submit', _name='zoom', _value='Zoom', _style="font-size:0.90em"),
        _name='plot_faults_form')

    supply_levels_form = FORM(
        INPUT(_name='supply_levels_start_date', _class='datetime', _maxlength=19, _size=19,
              requires=[IS_NOT_EMPTY(), IS_DATETIME()], value=session.supply_levels_start_date), LABEL("-"),
        INPUT(_name='supply_levels_end_date', _class='datetime', maxlength=19, _size=19,
              requires=[IS_NOT_EMPTY(), IS_DATETIME()], _value=session.supply_levels_end_date),BR(),
        INPUT(_type='submit', _value='Plot', _style="font-size:0.90em"),
        INPUT(_type='submit', _name='zoom', _value='Zoom', _style="font-size:0.90em"),
        _name='plot_supply_levels_form')

    interpolate_form = FORM(LABEL("Interpolation Type:", _style="font-size:0.50em"),
                            SELECT('none', 'cubic', _name="interpolate_type", _style="font-size:0.50em",
                                   value=session.interpolate_type),
                            INPUT(_type='submit', _value='Plot', _style="font-size:0.50em"),
                            LABEL("Plots that use cubic interpolation will take longer to complete.",
                                  _style="font-size:0.50em"),
                            _name='plot_interpolate_form')

    group_by_form = FORM(LABEL("Group by:", _style="font-size:0.50em"),
                         SELECT('day', 'hour', 'minute', _name="group_by", _style="font-size:0.50em",
                                value=session.group_by),
                         INPUT(_type='submit', _value='Plot', _style="font-size:0.50em"),
                         LABEL("Maximum of 15 days, 24 hours, or 60 minutes is plotted from start date",
                               _style="font-size:0.50em"),
                         _name='plot_group_by_form')

    if power_form.accepts(request.vars, keepvalues=True, session=session, formname='plot_power_form'):
        session.power_start_date = power_form.vars.power_start_date
        session.power_end_date = power_form.vars.power_end_date
        if request.vars.zoom:
            redirect(URL('default', 'plot_zoom', vars=dict(plot_type='power')))

    if voltage_form.accepts(request.vars, keepvalues=True, session=session, formname='plot_voltage_form'):
        session.voltage_start_date = voltage_form.vars.voltage_start_date
        session.voltage_end_date = voltage_form.vars.voltage_end_date
        if request.vars.zoom:
            redirect(URL('default', 'plot_zoom', vars=dict(plot_type='voltage')))

    if current_form.accepts(request.vars, keepvalues=True, session=session, formname='plot_current_form'):
        session.current_start_date = current_form.vars.current_start_date
        session.current_end_date = current_form.vars.current_end_date
        if request.vars.zoom:
            redirect(URL('default', 'plot_zoom', vars=dict(plot_type='current')))

    if frequency_form.accepts(request.vars, keepvalues=True, session=session, formname='plot_frequency_form'):
        session.frequency_start_date = frequency_form.vars.frequency_start_date
        session.frequency_end_date = frequency_form.vars.frequency_end_date
        if request.vars.zoom:
            redirect(URL('default', 'plot_zoom', vars=dict(plot_type='frequency')))

    if faults_form.accepts(request.vars, keepvalues=True, session=session, formname='plot_faults_form'):
        session.faults_start_date = faults_form.vars.faults_start_date
        session.faults_end_date = faults_form.vars.faults_end_date
        if request.vars.zoom:
            redirect(URL('default', 'plot_zoom', vars=dict(plot_type='faults')))

    if supply_levels_form.accepts(request.vars, keepvalues=True, session=session, formname='plot_supply_levels_form'):
        session.supply_levels_start_date = supply_levels_form.vars.supply_levels_start_date
        session.supply_levels_end_date = supply_levels_form.vars.supply_levels_end_date
        if request.vars.zoom:
            redirect(URL('default', 'plot_zoom', vars=dict(plot_type='supply_levels')))

    if interpolate_form.accepts(request.vars, keepvalues=True, session=session, formname='plot_interpolate_form'):
        session.interpolate_type = interpolate_form.vars.interpolate_type

    if group_by_form.accepts(request.vars, keepvalues=True, session=session, formname='plot_group_by_form'):
        session.group_by = group_by_form.vars.group_by

    message = 'Plotting data...'
    status, integrity_response = corona.check_usd_card_integrity()
    if status == 'error':
        message = integrity_response
    response.flash = message
    power_plot = URL('default', 'power_plot')
    voltage_plot = URL('default', 'voltage_plot')
    current_plot = URL('default', 'current_plot')
    frequency_plot = URL('default', 'frequency_plot')
    faults_plot = URL('default', 'faults_plot')
    supply_levels_plot = URL('default', 'supply_levels_plot')
    return locals()


def plot_zoom():

    message = 'Plotting data...'
    status, integrity_response = corona.check_usd_card_integrity()
    if status == 'error':
        message = integrity_response
    response.flash = message

    if request.vars.plot_type == 'frequency':
        plot_image = URL('default', 'frequency_plot', vars=dict(zoom='1'))
        plot_type = "FREQUENCY LEVELS"
    elif request.vars.plot_type == 'current':
        plot_image = URL('default', 'current_plot', vars=dict(zoom='1'))
        plot_type = "CURRENT LEVELS"
    elif request.vars.plot_type == 'voltage':
        plot_image = URL('default', 'voltage_plot', vars=dict(zoom='1'))
        plot_type = "VOLTAGE LEVELS"
    elif request.vars.plot_type == 'power':
        plot_image = URL('default', 'power_plot', vars=dict(zoom='1'))
        plot_type = "POWER LEVELS"
    elif request.vars.plot_type == 'faults':
        plot_image = URL('default', 'faults_plot', vars=dict(zoom='1'))
        plot_type = "FAULTS"
    elif request.vars.plot_type == 'supply_levels':
        plot_image = URL('default', 'supply_levels_plot', vars=dict(zoom='1'))
        plot_type = "SUPPLY LEVELS"
    return locals()


def help_dashboard():

    message = ''
    status, integrity_response = corona.check_usd_card_integrity()
    if status == 'error':
        message = integrity_response
        response.flash = message

    return locals()


def help_history():

    message = ''
    status, integrity_response = corona.check_usd_card_integrity()
    if status == 'error':
        message = integrity_response
        response.flash = message

    return locals()


def power_plot():
    response.files.append(URL('default', 'static/js/pygal-tooltips.min.js'))
    response.headers['Content-Type'] = 'image/svg+xml'

    interpolate_type = session.interpolate_type

    if interpolate_type == 'none':
        interpolate_type = None

    group_by = session.group_by

    plot_settings = corona.get_plot_settings(zoom=request.vars.zoom, group_by=group_by)

    line_chart = pygal.Line(interpolate=interpolate_type,
                            height=plot_settings['height'],
                            width=plot_settings['width'],
                            spacing=plot_settings['spacing'],
                            style=plot_settings['style'],
                            x_title=plot_settings['x_title'],
                            y_title="Power Levels (Watts)",
                            show_legend=True,
                            x_label_rotation=45,
                            no_data_text='No History',
                            show_x_guides=True)

    power_start_date = corona.sql_datetime_to_string(session.power_start_date)
    power_end_date = corona.sql_datetime_to_string(session.power_end_date)

    line_chart.title = 'Period: %s - %s, Group by %s' % (power_start_date, power_end_date, group_by)
    date_labels = []
    min_power_levels = []
    max_power_levels = []
    avg_power_levels = []

    sql = corona.generate_plot_sql_query('power', group_by, power_start_date, power_end_date)

    current_row = 0

    for row in db.executesql(sql,
                             as_dict=True):

        date_labels.append(str(row['creation_date']))
        min_power_levels.append(row['min_level'])
        max_power_levels.append(row['max_level'])
        avg_power_levels.append(row['avg_level'])
        current_row = current_row + 1
        if (group_by == 'day' and row == 15) or \
            (group_by == 'hour' and current_row == 24) or \
            (group_by == 'minute' and current_row == 60):
            break

    line_chart.x_labels = date_labels
    line_chart.add('Min', min_power_levels)
    line_chart.add('Max', max_power_levels)
    line_chart.add('AVG', avg_power_levels)
    return line_chart.render()


def voltage_plot():
    response.files.append(URL('default', 'static/js/pygal-tooltips.min.js'))
    response.headers['Content-Type'] = 'image/svg+xml'

    interpolate_type = session.interpolate_type
    if interpolate_type == 'none':
        interpolate_type = None
    group_by = session.group_by

    plot_settings = corona.get_plot_settings(zoom=request.vars.zoom, group_by=group_by)

    line_chart = pygal.Line(interpolate=interpolate_type,
                            height=plot_settings['height'],
                            width=plot_settings['width'],
                            spacing=plot_settings['spacing'],
                            style=plot_settings['style'],
                            x_title=plot_settings['x_title'],
                            y_title="Voltage Levels (Volts)",
                            show_legend=True,
                            x_label_rotation=45,
                            no_data_text="No History",
                            show_x_guides=True)

    voltage_start_date = corona.sql_datetime_to_string(session.voltage_start_date)
    voltage_end_date = corona.sql_datetime_to_string(session.voltage_end_date)

    line_chart.title = 'Period: %s - %s, Group by %s' % (voltage_start_date, voltage_end_date, group_by)
    date_labels = []
    min_voltage_levels = []
    max_voltage_levels = []
    avg_voltage_levels = []

    sql = corona.generate_plot_sql_query('voltage', group_by, voltage_start_date, voltage_end_date)

    current_row = 0

    for row in db.executesql(sql,
                             as_dict=True):

        date_labels.append(str(row['creation_date']))
        min_voltage_levels.append(row['min_level'])
        max_voltage_levels.append(row['max_level'])
        avg_voltage_levels.append(row['avg_level'])
        current_row += 1
        if (group_by == 'day' and row == 15) or \
            (group_by == 'hour' and current_row == 24) or \
            (group_by == 'minute' and current_row == 60):
            break

    line_chart.x_labels = date_labels
    line_chart.add('Min', min_voltage_levels)
    line_chart.add('Max', max_voltage_levels)
    line_chart.add('AVG', avg_voltage_levels)

    return line_chart.render()


def current_plot():
    response.files.append(URL('default', 'static/js/pygal-tooltips.min.js'))
    response.headers['Content-Type'] = 'image/svg+xml'

    interpolate_type = session.interpolate_type
    if interpolate_type == 'none':
        interpolate_type = None
    group_by = session.group_by

    plot_settings = corona.get_plot_settings(zoom=request.vars.zoom, group_by=group_by)

    line_chart = pygal.Line(interpolate=interpolate_type,
                            height=plot_settings['height'],
                            width=plot_settings['width'],
                            spacing=plot_settings['spacing'],
                            style=plot_settings['style'],
                            x_title=plot_settings['x_title'],
                            y_title="Current Levels (Amps)",
                            show_legend=True,
                            x_label_rotation=45,
                            no_data_text="No History",
                            show_x_guides=True)

    current_start_date = corona.sql_datetime_to_string(session.current_start_date)
    current_end_date = corona.sql_datetime_to_string(session.current_end_date)

    line_chart.title = 'Period: %s - %s, Group by %s' % (current_start_date, current_end_date, group_by)
    date_labels = []
    min_current_levels = []
    max_current_levels = []
    avg_current_levels = []

    sql = corona.generate_plot_sql_query('current', group_by, current_start_date, current_end_date)

    current_row = 0

    for row in db.executesql(sql,
                             as_dict=True):

        date_labels.append(str(row['creation_date']))
        min_current_levels.append(row['min_level'])
        max_current_levels.append(row['max_level'])
        avg_current_levels.append(row['avg_level'])
        current_row += 1
        if (group_by == 'day' and row == 15) or \
            (group_by == 'hour' and current_row == 24) or \
            (group_by == 'minute' and current_row == 60):
            break

    line_chart.x_labels = date_labels
    line_chart.add('Min', min_current_levels)
    line_chart.add('Max', max_current_levels)
    line_chart.add('AVG', avg_current_levels)
    return line_chart.render()


def frequency_plot():
    response.files.append(URL('default', 'static/js/pygal-tooltips.min.js'))
    response.headers['Content-Type'] = 'image/svg+xml'

    interpolate_type = session.interpolate_type
    if interpolate_type == 'none':
        interpolate_type = None
    group_by = session.group_by

    plot_settings = corona.get_plot_settings(zoom=request.vars.zoom, group_by=group_by)

    line_chart = pygal.Line(interpolate=interpolate_type,
                            height=plot_settings['height'],
                            width=plot_settings['width'],
                            spacing=plot_settings['spacing'],
                            style=plot_settings['style'],
                            x_title=plot_settings['x_title'],
                            y_title="Frequency Levels (kHz)",
                            show_legend=True,
                            x_label_rotation=45,
                            no_data_text="No History",
                            show_x_guides=True)

    frequency_start_date = corona.sql_datetime_to_string(session.frequency_start_date)
    frequency_end_date = corona.sql_datetime_to_string(session.frequency_end_date)

    line_chart.title = 'Period: %s - %s, Group by %s' % (frequency_start_date, frequency_end_date, group_by)
    date_labels = []
    min_frequency_levels = []
    max_frequency_levels = []
    avg_frequency_levels = []

    sql = corona.generate_plot_sql_query('frequency', group_by, frequency_start_date, frequency_end_date)

    current_row = 0

    for row in db.executesql(sql,
                             as_dict=True):

        date_labels.append(str(row['creation_date']))
        min_frequency_levels.append(row['min_level'])
        max_frequency_levels.append(row['max_level'])
        avg_frequency_levels.append(row['avg_level'])
        current_row += 1
        if (group_by == 'day' and row == 15) or \
            (group_by == 'hour' and current_row == 24) or \
            (group_by == 'minute' and current_row == 60):
            break

    line_chart.x_labels = date_labels
    line_chart.add('Min', min_frequency_levels)
    line_chart.add('Max', max_frequency_levels)
    line_chart.add('AVG', avg_frequency_levels)
    return line_chart.render()


def supply_levels_plot():
    response.files.append(URL('default', 'static/js/pygal-tooltips.min.js'))
    response.headers['Content-Type'] = 'image/svg+xml'

    interpolate_type = session.interpolate_type
    if interpolate_type == 'none':
        interpolate_type = None
    group_by = session.group_by

    plot_settings = corona.get_plot_settings(zoom=request.vars.zoom, group_by=group_by)

    line_chart = pygal.Line(interpolate=interpolate_type,
                            height=plot_settings['height'],
                            width=plot_settings['width'],
                            spacing=plot_settings['spacing'],
                            style=plot_settings['style'],
                            x_title=plot_settings['x_title'],
                            y_title="Average Supply Levels",
                            show_legend=True,
                            x_label_rotation=45,
                            no_data_text="No History",
                            show_x_guides=True)

    supply_levels_start_date = corona.sql_datetime_to_string(session.supply_levels_start_date)
    supply_levels_end_date = corona.sql_datetime_to_string(session.supply_levels_end_date)

    line_chart.title = 'Period: %s - %s, Group by %s' % (supply_levels_start_date, supply_levels_end_date, group_by)
    date_labels = []
    power_levels = []
    frequency_levels = []
    current_levels = []
    voltage_levels = []

    sql = corona.generate_plot_sql_query('supply', group_by, supply_levels_start_date, supply_levels_end_date)

    current_row = 0

    for row in db.executesql(sql,
                             as_dict=True):

        date_labels.append(str(row['creation_date']))
        power_levels.append(row['avg_power_level'])
        voltage_levels.append(row['avg_voltage_level'])
        frequency_levels.append(row['avg_frequency_level'])
        current_levels.append(row['avg_current_level'])
        current_row += 1
        if (group_by == 'day' and row == 15) or \
            (group_by == 'hour' and current_row == 24) or \
            (group_by == 'minute' and current_row == 60):
            break

    line_chart.x_labels = date_labels
    line_chart.add('Power', power_levels)
    line_chart.add('Voltage', voltage_levels)
    line_chart.add('Current', current_levels)
    line_chart.add('Frequency', frequency_levels)

    return line_chart.render()


def faults_plot():
    response.files.append(URL('default', 'static/js/pygal-tooltips.min.js'))
    response.headers['Content-Type'] = 'image/svg+xml'

    faults_start_date = corona.sql_datetime_to_string(session.faults_start_date)
    faults_end_date = corona.sql_datetime_to_string(session.faults_end_date)
    group_by = session.group_by

    plot_settings = corona.get_plot_settings(zoom=request.vars.zoom, group_by=group_by)

    date_labels = []
    high_temp_faults = []
    interlock_faults = []
    inverter_on_faults = []
    overvolt_faults = []
    current_trip_faults = []
    dc_bus_faults = []

    line_chart = pygal.Bar(height=plot_settings['height'],
                           width=plot_settings['width'],
                           spacing=plot_settings['spacing'],
                           style=plot_settings['style'],
                           x_title=plot_settings['x_title'],
                           y_title="Faults",
                           show_legend=True,
                           x_label_rotation=45,
                           no_data_text="No History",
                           show_x_guides=True)

    line_chart.title = 'Period: %s - %s, Group by %s' % (faults_start_date, faults_end_date, group_by)

    sql = corona.generate_plot_sql_query('faults', group_by, faults_start_date, faults_end_date)

    current_row = 0

    for row in db.executesql(sql,
                             as_dict=True):

        date_labels.append(str(row['creation_date']))
        high_temp_faults.append(row['number_high_temp_faults'])
        interlock_faults.append(row['number_interlock_faults'])
        inverter_on_faults.append(row['number_inverter_on_faults'])
        overvolt_faults.append(row['number_overvolt_faults'])
        current_trip_faults.append(row['number_current_trip_faults'])
        dc_bus_faults.append(row['number_dc_bus_faults'])

        current_row += 1

        if (group_by == 'day' and row == 15) or \
            (group_by == 'hour' and current_row == 24) or \
            (group_by == 'minute' and current_row == 60):
            break

    line_chart.x_labels = date_labels

    line_chart.add('High Temp', high_temp_faults)
    line_chart.add('Interlock', interlock_faults)
    line_chart.add('Inverter On', inverter_on_faults)
    line_chart.add('Overvolt', overvolt_faults)
    line_chart.add('Current Trip', current_trip_faults)
    line_chart.add('DC Bus', dc_bus_faults)

    return line_chart.render()


def format_usd_card():
    # close database so the uSD card can be formatted
    db.close()
    time.sleep(3.0)
    status = corona.format_usd_card_and_db()
    session.flash = "Format of micro SD card and database operation complete."
    redirect(URL('default', 'index'))
    return locals()


def delete_all_history():

    message = ''
    status, integrity_response = corona.check_usd_card_integrity()
    if status == 'error':
        message = integrity_response
    else:
        # stop modbus consumer serial communications and database recording
        corona.stop_communication_and_recording()
        db.psu_data.truncate()
        db.commit()
        db.close()
        time.sleep(3.0)
        # start modbus consumer serial communication and database recording
        corona.start_communication_and_recording()
        message = "All History Data Deleted."
    session.flash = message
    redirect(URL('default', 'history'))
    return locals()


def history():
    message = ''
    status, integrity_response = corona.check_usd_card_integrity()
    if status == 'error':
        message = integrity_response
        session.flash = message

    # db.psu_data.id.readable = False
    form = SQLFORM.grid(db.psu_data, paginate=15, orderby=db.psu_data.created_on,
                        create=False, editable=False, deletable=True)
    return locals()


# ---- API (example) -----
@auth.requires_login()
def api_get_user_email():
    if not request.env.request_method == 'GET': raise HTTP(403)
    return response.json({'status': 'success', 'email': auth.user.email})


# ---- Smart Grid (example) -----
@auth.requires_membership('admin')  # can only be accessed by members of admin groupd
def grid():
    response.view = 'generic.html'  # use a generic view
    tablename = request.args(0)
    if not tablename in db.tables: raise HTTP(403)
    grid = SQLFORM.smartgrid(db[tablename], args=[tablename], deletable=False, editable=False)
    return dict(grid=grid)


# ---- Embedded wiki (example) ----
def wiki():
    auth.wikimenu()  # add the wiki to the menu
    return auth.wiki()


# ---- Action for login/register/etc (required for auth) -----
def user():
    """
    exposes:
    http://..../[app]/default/user/login
    http://..../[app]/default/user/logout
    http://..../[app]/default/user/register
    http://..../[app]/default/user/profile
    http://..../[app]/default/user/retrieve_password
    http://..../[app]/default/user/change_password
    http://..../[app]/default/user/bulk_register
    use @auth.requires_login()
        @auth.requires_membership('group name')
        @auth.requires_permission('read','table name',record_id)
    to decorate functions that need access control
    also notice there is http://..../[app]/appadmin/manage/auth to allow administrator to manage users
    """
    return dict(form=auth())


# ---- action to server uploaded static content (required) ---
@cache.action()
def download():
    """
    allows downloading of uploaded files
    http://..../[app]/default/download/[filename]
    """
    return response.download(request, db)
