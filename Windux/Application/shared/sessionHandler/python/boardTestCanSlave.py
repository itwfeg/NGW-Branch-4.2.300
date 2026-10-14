#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import time
import can


class boardTest:
    def __init__(self):
        self.errorLog   = None
        self.bus = None
        self.pingId = 0x100
        self.objectName = 'CanSlave'
        self.logLocation = '/var/log/'
        os.environ["CAN_INTERFACE"] = "socketcan_ctypes"
        os.environ["CAN_CHANNEL"] = "can1"
        self.initCan()
        self.app()
        
    def log(self, s):
        if self.errorLog is not None:
            self.errorLog.write(s.encode('utf-8'))

    def initCan(self):
        #initialize the can interface and set bus speed to 250Khz
        if sys.platform.startswith("linux"):
            #create debug log file
            self.errorLog = open( self.logLocation + self.objectName  + "-log.txt",  "wb")
            self.log("log opened: " + time.asctime( time.localtime(time.time()) ) + "\n")
            
            cmd = []
            cmd.append("ip")
            cmd.append("link")
            cmd.append("set")
            cmd.append("can1")
            cmd.append("up")
            cmd.append("type")
            cmd.append("can")
            cmd.append("bitrate")    
            cmd.append("250000")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            if exitCode != 0:
                self.log("Can bus interface failed to initialize.\n")
            else:
                self.log("Can bus ready for use.\n") 

            cmd = []
            cmd.append("ifconfig")
            cmd.append("can1")
            cmd.append("up")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            if exitCode != 0:
                self.log("Can bus interface driver failed to load.\n") 

        else:
            self.log("OS type not supported: " +  sys.platform)
            self.log("log closed\n")

    def app(self):
        self.bus = can.interface.Bus()        
        if self.bus != None:
            while True:
                msg = self.bus.recv()
                self.parseMsg(msg)
        else:
            self.log("Failed to obtain interface bus! \n")
    
    def parseMsg(self,  msg):
        if msg.arbitration_id == self.pingId:
            if msg.data[0] == 170:
                self.log("Can test message received!\n") 
                self.errorLog.flush()
                self.errorLog.close()
                msg = can.Message(arbitration_id=0x101,
                            data=b'\xff',
                            extended_id=False)
                self.bus.send(msg)
           
def main():
    slave = boardTest()
    
if __name__ == "__main__":
    main()
