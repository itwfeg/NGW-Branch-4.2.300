service_key_name = 'service'
service_key = 'hobartService12*$(Q)E43555#@BHJsdda'
service_filename = '%s.key' % (service_key_name)

manufacturing_key_name = 'manufacturing'
manufacturing_key = 'hobartManufacturing(2@AP#FMaasdf999'
manufacturing_filename = '%s.key' % (manufacturing_key_name)

selfservice_key_name = 'selfservice'
selfservice_key = 'hobartSelfService9294knKKQ$$RR$Q#$&*'
selfservice_filename = '%s.key' % (selfservice_key_name)

supervisor_key_name = 'supervisor'
supervisor_key = 'hobartSupervisor)@_Pav###avn123BAZ^*('
supervisor_filename = '%s.key' % (supervisor_key_name)

systemtest_key_name = 'systemtest'
systemtest_key = 'hobartSystemTest*2vK!@@)%&#Bbb$%^&aBEVM>'
systemtest_filename = '%s.key' % (systemtest_key_name)

all_keys = [supervisor_key_name, service_key_name, selfservice_key_name, systemtest_key_name, manufacturing_key_name]

# get info about key
def getKeyInfo(keyType):
    if keyType == service_key_name:
        key = service_key
        key_filename = service_filename
    elif keyType == manufacturing_key_name:
        key = manufacturing_key
        key_filename = manufacturing_filename
    elif keyType == supervisor_key_name:
        key = supervisor_key
        key_filename = supervisor_filename
    elif keyType == selfservice_key_name:
        key = selfservice_key
        key_filename = selfservice_filename
    elif keyType == systemtest_key_name:
        key = systemtest_key
        key_filename = systemtest_filename
    else:
        key = None
        key_filename = None
    return (key, key_filename)
