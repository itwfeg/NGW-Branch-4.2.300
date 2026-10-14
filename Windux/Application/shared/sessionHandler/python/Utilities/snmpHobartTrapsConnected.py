#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from pysnmp.entity import engine, config
from pysnmp.carrier.asyncore.dgram import udp
from pysnmp.entity.rfc3413 import ntforg
from pysnmp.proto.api import v2c

def getScaleSettingsValue(key):
    with open('/usr/local/hobart/db/settingswizard/scalesettings') as f:
        for line in f:
            line = line.rstrip()
            if key in line:
                tokens = line.split('=')
                try:
                    return tokens[1].strip()
                except:
                    return ''

ITW_FEG_TRAPS_PORT = int(getScaleSettingsValue("snmpTrapsPort"))
ITW_FEG_TRAPS_IP = getScaleSettingsValue("snmpTrapsIP") 

# Create SNMP engine instance
snmpEngine = engine.SnmpEngine()

# SecurityName <-> CommunityName mapping
config.addV1System(snmpEngine, 'my-area', 'public', transportTag='all-my-managers')

# Specify security settings per SecurityName (SNMPv2c -> 1)
config.addTargetParams(snmpEngine, 'my-creds', 'my-area', 'noAuthNoPriv', 1)

# Setup transport endpoint and bind it with security settings yielding
# a target name
config.addTransport(
    snmpEngine,
    udp.domainName,
    udp.UdpSocketTransport().openClientMode()
)
config.addTargetAddr(
    snmpEngine, 'my-nms',
    #udp.domainName, ('10.3.177.78', 162),
    udp.domainName, (ITW_FEG_TRAPS_IP, ITW_FEG_TRAPS_PORT),
    'my-creds',
    tagList='all-my-managers'
)

# Specify what kind of notification should be sent (TRAP or INFORM),
# to what targets (chosen by tag) and what filter should apply to
# the set of targets (selected by tag)
config.addNotificationTarget(
    snmpEngine, 'my-notification', 'my-filter', 'all-my-managers', 'trap'
)

# Allow NOTIFY access to Agent's MIB by this SNMP model (2), securityLevel
# and SecurityName
config.addContext(snmpEngine, '')
config.addVacmUser(snmpEngine, 2, 'my-area', 'noAuthNoPriv', (), (), (1, 3, 6))

# *** SNMP engine configuration is complete by this line ***

# Create Notification Originator App instance. 
ntfOrg = ntforg.NotificationOriginator()


# Error/confirmation receiver
# noinspection PyUnusedLocal,PyUnusedLocal,PyUnusedLocal,PyUnusedLocal,PyUnusedLocal
def cbFun(snmpEngine, sendRequestHandle, errorIndication,
          errorStatus, errorIndex, varBinds, cbCtx):
    print(('Notification %s, status - %s' % (
        sendRequestHandle, errorIndication and errorIndication or 'delivered'
    )
          ))


# Build and submit notification message to dispatcher
sendRequestHandle = ntfOrg.sendVarBinds(
    snmpEngine,
    # Notification targets
    'my-notification',  # notification targets
    None, '',  # contextEngineId, contextName
    # var-binds
    [
        # SNMPv2-SMI::snmpTrapOID.0 = SNMPv2-MIB::coldStart
        ((1, 3, 6, 1, 4, 1, 82253, 100, 1, 0), v2c.ObjectIdentifier((1, 3, 6, 1, 4, 1, 82253, 100, 1, 1))),
        # additional var-binds: ( (oid, value), ... )
        ((1, 3, 6, 1, 4, 1, 82253, 100, 1, 1), v2c.OctetString('Scale is Connected')),
        ((1, 3, 6, 1, 4, 1, 82253, 100, 1, 2), v2c.OctetString('Online'))
    ],
    cbFun
)

print(('Notification %s is scheduled to be sent' % sendRequestHandle))

# Run I/O dispatcher which would send pending message and process response
snmpEngine.transportDispatcher.runDispatcher()
