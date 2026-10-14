# -*- coding: utf-8 -*-

db.define_table('psu_data',
                Field('created_on', label='Created On', type='datetime', notnull=True, writable=False, readable=True),
                Field('power_level', label='Power (Watts)', type='double'),
                Field('voltage_level', label='Voltage (Volts)', type='integer'),
                Field('current_level', label='Current (Amps)', type='integer'),
                Field('frequency_level', label='Frequency (kHz)', type='integer'),
                Field('high_temp_fault', label='High Temp Fault', type='boolean'),
                Field('interlock_fault', label='Interlock Fault', type='boolean'),
                Field('dc_bus_fault', label='DC Bus Fault', type='boolean'),
                Field('overvolt_fault', label='Overvolt Fault', type='boolean'),
                Field('current_trip_fault', label='Current Trip Fault', type='boolean'),
                Field('inverter_on_fault', label='Inverter On Fault', type='boolean'))

db.executesql("CREATE INDEX IF NOT EXISTS CREATED_ON_INDEX ON PSU_DATA (CREATED_ON);")
db.executesql("PRAGMA journal_mode=WAL;")
# if db.executesql("PRAGMA journal_mode") != "wal":
#     db.executesql("PRAGMA journal_mode=WAL;")

# db.executesql("PRAGMA main.page_size = 4096;")
# db.executesql("PRAGMA main.cache_size=10000;")
# db.executesql("PRAGMA main.locking_mode=EXCLUSIVE;")
# db.executesql("PRAGMA main.synchronous=NORMAL;")

# db.executesql("PRAGMA main.cache_size=5000;")
