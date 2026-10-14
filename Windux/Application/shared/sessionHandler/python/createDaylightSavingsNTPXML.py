#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# not tested in python3

import sys, os
import shutil
import shellCommand


def writeXML(dayOfWeek, hour, minute, fileName):
    XMLFileContents = ''
    XMLFileContents += '<?xml version="1.0" encoding="UTF-16"?>\n<Task version="1.3" xmlns="http://schemas.microsoft.com/windows/2004/02/mit/task">\n  <RegistrationInfo>\n    <Date>2014-01-01T12:00:00.6013278</Date>\n    <Author>'
    #add machine name
    cmd = []
    cmd.append("hostname")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    XMLFileContents += stdout.strip()

    
    XMLFileContents += '\HTx</Author>\n  </RegistrationInfo>\n  <Triggers>\n'
    XMLFileContents += '<CalendarTrigger>\n      <StartBoundary>2014-01-01T'
    XMLFileContents += str(hour) + ':' + str(minute)
    XMLFileContents += ':00</StartBoundary>\n      <ExecutionTimeLimit>PT30M</ExecutionTimeLimit>\n      <Enabled>true</Enabled>\n      <ScheduleByMonthDayOfWeek>\n        <Weeks>\n         <Week>2</Week>\n       </Weeks><DaysOfWeek>\n'
    XMLFileContents += '        <Sunday />\n      </DaysOfWeek>\n       <Months>\n      <March />\n      </Months>\n     </ScheduleByMonthDayOfWeek>\n'
    XMLFileContents += '    </CalendarTrigger>\n'
    XMLFileContents += '<CalendarTrigger>\n      <StartBoundary>2014-01-01T'
    XMLFileContents += hour + ':' + minute
    XMLFileContents += ':00</StartBoundary>\n      <ExecutionTimeLimit>PT30M</ExecutionTimeLimit>\n      <Enabled>true</Enabled>\n      <ScheduleByMonthDayOfWeek>\n        <Weeks>\n         <Week>1</Week>\n       </Weeks><DaysOfWeek>\n'
    XMLFileContents += '        <Sunday />\n      </DaysOfWeek>\n       <Months>\n      <November />\n      </Months>\n     </ScheduleByMonthDayOfWeek>\n'
    XMLFileContents += '    </CalendarTrigger>\n'
  
    XMLFileContents += '</Triggers>\n  <Principals>\n    <Principal id="Author">\n      <UserId>S-1-5-18</UserId>\n      <RunLevel>HighestAvailable</RunLevel>\n    </Principal>\n  </Principals>\n  <Settings>\n    <MultipleInstancesPolicy>IgnoreNew</MultipleInstancesPolicy>\n    <DisallowStartIfOnBatteries>true</DisallowStartIfOnBatteries>\n    <StopIfGoingOnBatteries>false</StopIfGoingOnBatteries>\n    <AllowHardTerminate>true</AllowHardTerminate>\n    <StartWhenAvailable>true</StartWhenAvailable>\n    <RunOnlyIfNetworkAvailable>true</RunOnlyIfNetworkAvailable>\n    <IdleSettings>\n      <StopOnIdleEnd>true</StopOnIdleEnd>\n      <RestartOnIdle>false</RestartOnIdle>\n    </IdleSettings>\n'
    XMLFileContents += '      <AllowStartOnDemand>true</AllowStartOnDemand>\n    <Enabled>true</Enabled>\n    <Hidden>false</Hidden>\n    <RunOnlyIfIdle>false</RunOnlyIfIdle>\n    <DisallowStartOnRemoteAppSession>false</DisallowStartOnRemoteAppSession>\n'
    XMLFileContents += '     <UseUnifiedSchedulingEngine>false</UseUnifiedSchedulingEngine>\n    <WakeToRun>false</WakeToRun>\n    <ExecutionTimeLimit>PT1H</ExecutionTimeLimit>\n    <Priority>7</Priority>\n    <RestartOnFailure>\n      <Interval>PT5M</Interval>\n      <Count>3</Count>\n    </RestartOnFailure>\n  </Settings>\n'
    XMLFileContents += '  <Actions Context="Author">\n    <Exec>\n      <Command>w32tm</Command>\n      <Arguments>/resync /force</Arguments>\n    </Exec>\n  </Actions>\n</Task>'

    #print(XMLFileContents)
    import configInfo
    rootFolder = configInfo.getRootFolder()
    ntpXMLFileLocation = rootFolder.replace('\\\\', '\\') + '\\db\\settingswizard\\' + str(fileName)
    #print(ntpXMLFileLocation)

    # DELETE EXISTING PROFILE FILE
    if os.path.isfile(ntpXMLFileLocation):
        try:
            os.remove(ntpXMLFileLocation)
        except:
            pass
    
    # CREATE AND WRITE EXISTING PROFILE FILE
    try:
        f = open(ntpXMLFileLocation,  'w')
        f.write(XMLFileContents)
        f.close()
        return True
    except:
        return False
    
