# -*- coding: utf-8 -*-

import sys
import subprocess
import os
import tags
import shellCommand
import osUtils
import platform
import getBoardType

CONST_PART_SIZE = 4096
BLANK = b'\xff'

class mfgSection:
    def __init__(self):
        self.sectionMarker = '0'
        self.sectionType = '1'
        self.writeCounter = '0'
        self.sectionCrc = ''
        self.sectionBlank = False
        self.systemStyle = None
        self.scaleStyle = None
        self.boardAssembly = None
        self.boardRev = None
        self.dateOfManufacture = None
        self.serialNumber = None
        self.secTags = tags.mfgTags()
        self.partition = [] # partition (string on python2, bytes on python3)
        self.partition_string = '' # partition as a string (0xff stripped)
        self.w_partition = '' # partition data to write as a string
        #self.partition ='0,1,1,11443<MFGSection><SStyle>0</SStyle><BDAssembly>45-1084-982</BDAssembly><BDRev>1.0</BDRev><MFGDate>11302012</MFGDate><Serial>1234567</Serial></MFGSection>'
        self.read()
        self.init()
        #self.show() --do not show flash as chobartd uses this script

    def init(self):
        #initialize the class
        if len(self.partition) > 0:
            if self.partition[0] != BLANK:
                # the first part is comma-separated metadata
                metadata = self.partition_string.split(self.secTags.getSectionStartTag())[0].split(',')
                #print(metadata)
                self.sectionMarker, self.sectionType, self.writeCounter, self.sectionCrc = metadata

                try:
                    index = self.partition_string.index(self.secTags.getStyleStartTag()) + len(self.secTags.getStyleStartTag())
                    endIndex = self.partition_string.index(self.secTags.getStyleEndTag())
                    self.scaleStyle = self.partition_string[index:endIndex]

                    if len(self.scaleStyle) >= 3:
                        self.scaleStyle = self.scaleStyle.strip()
                        self.scaleStyle = self.scaleStyle.strip('"')
                        if "EPCP" in self.scaleStyle.upper():
                            if len(self.scaleStyle) >= 8:
                                self.scaleStyle = self.scaleStyle[:8].upper() + self.scaleStyle[8:].lower()
                        elif "NGW1" in self.scaleStyle.upper() or "NGW2" in self.scaleStyle.upper() or "NGW3" in self.scaleStyle.upper() or "NGS1" in self.scaleStyle.upper():
                            self.scaleStyle = self.scaleStyle.upper()
                        else:
                            self.scaleStyle = self.scaleStyle[:2].upper() + self.scaleStyle[2].lower() + self.scaleStyle[3:]
                    else:
                        self.scaleStyle = ""
                except:
                    self.scaleStyle = ""
                    
                    
                # get the contents between two tags (x for start, y for end)
                def get_contents(start, end):
                    return self.partition_string[self.partition_string.index(start) + len(start):self.partition_string.index(end)]

                try:
                    self.systemStyle = get_contents(self.secTags.getSystemStyleStartTag(), self.secTags.getSystemStyleEndTag())
                except:
                    self.systemStyle = "HTi"

                try:
                    self.boardAssembly = get_contents(self.secTags.getAssemblyStartTag(), self.secTags.getAssemblyEndTag())
                except:
                    self.boardAssembly = ""

                try:
                    self.boardRev = get_contents(self.secTags.getRevStartTag(), self.secTags.getRevEndTag())
                except:
                    self.boardRev = ""

                try:
                    self.dateOfManufacture = get_contents(self.secTags.getDateStartTag(), self.secTags.getDateEndTag())
                except:
                    self.dateOfManufacture = ""

                try:
                    self.serialNumber = get_contents(self.secTags.getSerialStartTag(), self.secTags.getSerialEndTag())
                except:
                    self.serialNumber = ""

                # check if fields have additional quotes
                self.boardAssembly = self.boardAssembly.strip('"')
                self.boardRev = self.boardRev.strip('"')
                self.dateOfManufacture =  self.dateOfManufacture.strip('"')
                self.serialNumber = self.serialNumber.strip('"')

            else: # self.sectionMarker == BLANK
                #print 'Manufacturing serial flash partition is blank: loading defaults' --do not show flash as chobartd uses this script
                self.sectionBlank = True
                self.loadDefaults()

    def isSectionBlank(self):
        return self.sectionBlank

    def getRawScaleStyle(self):
        return self.scaleStyle

    def getSystemStyle(self):
        return self.systemStyle

    def getScaleStyle(self):
        if self.scaleStyle == '0':
            if getBoardType.isArkBoard():
                return "HTm-7LS"
            else:
                return "HTi-7LS"
        else:
            return self.scaleStyle

    def setBoardAssembly(self,  assembly):
        self.boardAssembly = assembly

    def getBoardAssembly(self):
        return self.boardAssembly

    def setDateOfManufacturing(self,  date):
        self.dateOfManufacture = date

    def getDateOfManufacturing(self):
        return self.dateOfManufacture

    def setBoardRevision(self,  rev):
        self.boardRev = rev

    def getBoardRevision(self):
        return self.boardRev

    def setScaleSerialNumber(self, serial):
        self.serialNumber = serial

    def setScaleSerialNumber(self, serial):
        self.serialNumber = serial

    def getScaleSerialNumber(self):
        return self.serialNumber

    def setScaleStyle(self,  style):
        self.scaleStyle = style

    def getWriteCounter(self):
        return self.writeCounter

    def setWriteCounter(self, value):
        self.writeCounter = value

    def getRawPartition(self):
        return self.partition

    def write(self):
        self.serializeData()
        self.calculateCrc()
        self.serialize()

        #erase the serial flash partition
        if  platform.system() == 'Linux':

            #write the new manufacturing information to file
            outFile = open(os.path.join("/", "tmp", "mfg.dat"),  'w')
            outFile.write(self.w_partition)
            outFile.close()
           
            erase_cmd = []
            erase_cmd.append(os.path.join("/","usr","sbin","flash_eraseall"))
            erase_cmd.append(os.path.join("/", "dev","mtd3"))
            exitCode, stdout, stderr = shellCommand.execute(erase_cmd)

            if exitCode == 0:
                print('Manufacturing partition of the serial flash has been erased.')
            else:
                return ('error', stderr)

            write_cmd = []
            write_cmd.append(os.path.join("/","usr","sbin","flashcp"))
            write_cmd.append("/tmp/mfg.dat")
            write_cmd.append(os.path.join("/", "dev", "mtd3"))
            exitCode, stdout, stderr = shellCommand.execute(write_cmd)

            if exitCode == 0:
                print('Manufacturing data has been saved.')
            else:
                return ('error', stderr)

        elif  platform.system() == 'Windows':

            if os.path.exists(os.path.join("C:\Program Files", "Hobart", "Bin")):

                #write out the partition contents to a file
                outFile = open("C:\\Program Files\\Hobart\\Bin\\mfg.dat",  'w')
                outFile.write(b'\x00' * 4096)
                outFile.seek(0)
                outFile.write(self.w_partition)
                outFile.close()
                #wite the contents of the manufacturing partition file to the serial flash
                args = ['C:\\Program Files\\Hobart\\Bin\\SPIFlashTest.exe',  'u',  '100000',  'C:\\Program Files\\Hobart\\Bin\\mfg.dat']
                osUtils.winRunElevated(args)

    def calculateCrc(self):
        # skip to the actual data
        i = self.w_partition.index(self.secTags.getSectionStartTag())
        
        # convert all characters to ints and add them up, then convert to string
        self.sectionCrc = str(sum([ord(x) for x in self.w_partition[i:]]))

    def serializeData(self):
        #serialize the data into the partition buffer to write

        #write section start tag to the partition buffer
        self.w_partition += self.secTags.getSectionStartTag()
        self.w_partition += self.secTags.getStyleStartTag()
        self.w_partition += self.scaleStyle
        self.w_partition += self.secTags.getStyleEndTag()
        self.w_partition += self.secTags.getAssemblyStartTag()
        self.w_partition += self.boardAssembly
        self.w_partition += self.secTags.getAssemblyEndTag()
        self.w_partition += self.secTags.getRevStartTag()
        self.w_partition += self.boardRev
        self.w_partition += self.secTags.getRevEndTag()
        self.w_partition += self.secTags.getDateStartTag()
        self.w_partition += self.dateOfManufacture
        self.w_partition += self.secTags.getDateEndTag()
        self.w_partition += self.secTags.getSerialStartTag()
        self.w_partition += self.serialNumber
        self.w_partition += self.secTags.getSerialEndTag()
        self.w_partition += self.secTags.getSystemStyleStartTag()
        self.w_partition += self.systemStyle
        self.w_partition += self.secTags.getSystemStyleEndTag()
        self.w_partition += self.secTags.getSectionEndTag()
        #print self.w_partition

    def serialize(self):
        #serialize the data into the partition buffer to write
        dataString = self.w_partition

        #increment the write counter
        count = int(self.writeCounter)
        count += 1
        self.writeCounter = str(count)

        self.w_partition = self.sectionMarker
        self.w_partition += ','
        self.w_partition += self.sectionType
        self.w_partition += ','
        self.w_partition += self.writeCounter
        self.w_partition += ','
        self.w_partition += self.sectionCrc

        #write section start tag to the partition buffer
        self.w_partition += dataString
        #print self.w_partition

    def read(self):
        if platform.system() == 'Linux':
            #read the manufactioning partition of the serial flash
            if os.path.exists(os.path.join("/","dev", "mtdblock3" )):
                try:
                    # open the manufacturing  partition
                    inFile = open(os.path.join("/","dev", "mtd3"), 'rb')

                    # read the entire partition
                    self.partition = inFile.read(CONST_PART_SIZE)
                    inFile.close()
                    #print self.partition --do not show flash as chobartd uses this script

                except IOError as exception_details:
                    result = 'error: Reading MFG partition failed: ' + ', Reason:' + exception_details[1]

        elif platform.system() == 'Windows':

            if os.path.exists(os.path.join("C:\Program Files", "Hobart", "Bin" )):
                args = ['C:\\Program Files\\Hobart\\Bin\\SPIFlashTest.exe',  'd',  '100000',  'C:\\Program Files\\Hobart\\Bin\\mfgDump.bin']
                osUtils.winRunElevated(args)

                try:
                    inFile = open("C:\\Program Files\\Hobart\\Bin\\mfgDump.bin",  'rb')

                    self.partition = inFile.read(CONST_PART_SIZE)
                    #print self.partition --do not show flash as chobartd uses this script
                    inFile.close()

                except IOError as exception_details:
                    result = 'error: Reading manufacturing partition failed: ' + ', Reason:' + exception_details[1]
                    #print result
                    self.partition = ""

        else:
            print('Error: Unsupported Operating system')
        
        # handle python3 a little differently
        if sys.version_info.major == 3:
            self.partition_string = bytes([x for x in self.partition if x != 0xFF]).decode('utf-8')
        else:
            self.partition_string = "".join([x for x in self.partition if x != b'\xFF'])

    def loadDefaults(self):
        #manufacturing test has not been implemented for htx
        if platform.system() == 'Windows':
            #setup default manufacturing information for the htx
            self.writeCounter = '0'
            self.sectionMarker = '1'
            self.sectionType = '1'
            self.sectionCrc  = ''

            self.scaleStyle = 'HTx'
            self.boardAssembly = '00-447159'
            self.boardRev = '0.3'
            self.dateOfManufacture = '03032014'
            self.serialNumber = ''

            #serialize the partition data
            self.serializeData()
            self.calculateCrc()
            self.serialize()

            #print 'w_partition length: ' + str( len(self.w_partition) )
            #append blank characters from the end of the data to the end of the partition
            for i in range(len(self.w_partition),  CONST_PART_SIZE):
                self.w_partition += b'\xff'

            #print self.w_partition

            if os.path.exists(os.path.join("C:\Program Files", "Hobart", "Bin")):

                #write out the partition contents to a file
                outFile = open("C:\\Program Files\\Hobart\\Bin\\mfg.dat",  'w')
                outFile.write(b'\x00' * 4096)
                outFile.seek(0)
                outFile.write(self.w_partition)
                outFile.close()
                #wite the contents of the manufacturing partition file to the serial flash
                args = ['C:\\Program Files\\Hobart\\Bin\\SPIFlashTest.exe',  'u',  '100000',  'C:\\Program Files\\Hobart\\Bin\\mfg.dat']
                osUtils.winRunElevated(args)

        if platform.system() == 'Linux':
            #setup default manufacturing information for the htx
            self.writeCounter = '0'
            self.sectionMarker = '1'
            self.sectionType = '1'
            self.sectionCrc  = ''
            #scale style must be determined and set before loading defaults
            #self.scaleStyle = 'HT'
            self.boardAssembly = '00-447159'
            self.boardRev = '0.3'
            self.dateOfManufacture = '03032014'
            self.serialNumber = ''

    def show(self):
        print('Manufacturing section of serial flash')
        print('*************************************')
        print(('Section marker: ' + str(self.sectionMarker)))
        print(('Section type: ' + str(self.sectionType)))
        print(('Section write counter:' + str(self.writeCounter)))
        print(('Section crc: ' + str(self.sectionCrc)))
        print('*************************************')

        if self.scaleStyle == '0':
            if getBoardType.isArkBoard():
                print(('Scale style: ' + 'HTm-7LS'))
            else:
                print(('Scale style: ' + 'HTi-7LS'))
        else:
            print(('Scale style: ' + self.scaleStyle))

        print(('Board assembly: ' + self.boardAssembly))
        print(('Board revision: ' + self.boardRev))
        print(('Date of manufacture: ' + self.dateOfManufacture))
        print(('Scale serial number: ' + self.serialNumber))

class hardwareSection:
    def __init__(self):
        self.sectionMarker = '0'
        self.sectionType = '3'
        self.writeCounter = '0'
        self.sectionCrc = ''

        self.primaryDisplayType = None
        self.primaryDisplaySize = None
        self.primaryDisplayInterface = None
        self.secondaryDisplayType = None
        self.secondaryDisplaySize = None
        self.secondaryDisplayInterface = None
        self.wifiInstalled = None
        self.sataInstalled = None
        self.bluetoothInstalled = None
        self.power = None
        self.pcieInstalled = None
        self.domInstalled = None
        self.rfidInstalled = None
        self.secTags = tags.hardwareTags()
        self.partition = [] # string in python2, bytes in python3
        self.partition_string = ""

        self.read()
        self.init()
        #self.show()

    def init(self):
        if len(self.partition) > 0 and self.partition[0] != BLANK:
            # the first part is comma-separated metadata
            metadata = self.partition_string.split(self.secTags.getSectionStartTag())[0].split(',')
            self.sectionMarker, self.sectionType, self.writeCounter, self.sectionCrc = metadata
            
            # get the contents between two tags
            def get_contents(start, end):
                return self.partition_string[self.partition_string.index(start) + len(start):self.partition_string.index(end)]

            self.primaryDisplayType = get_contents(self.secTags.getPrimaryDisplayStartTag(), self.secTags.getPrimaryDisplayEndTag())

            self.primaryDisplaySize = get_contents(self.secTags.getPrimaryDisplaySizeStartTag(), self.secTags.getPrimaryDisplaySizeEndTag())

            self.primaryDisplayInterface = get_contents(self.secTags.getPrimaryDisplayInterfaceStartTag(), self.secTags.getPrimaryDisplayInterfaceEndTag())

            self.secondaryDisplayType = get_contents(self.secTags.getSecondaryDisplayStartTag(), self.secTags.getSecondaryDisplayEndTag())

            self.secondaryDisplaySize = get_contents(self.secTags.getSecondaryDisplaySizeStartTag(), self.secTags.getSecondaryDisplaySizeEndTag())

            self.secondaryDisplayInterface = get_contents(self.secTags.getSecondaryInterfaceStartTag(), self.secTags.getSecondaryInterfaceEndTag())

            self.wifiInstalled = get_contents(self.secTags.getWifiStartTag(), self.secTags.getWifiEndTag())

            self.sataInstalled = get_contents(self.secTags.getSataStartTag(), self.secTags.getSataEndTag())
            
            self.bluetoothInstalled = get_contents(self.secTags.getBluetoothStartTag(), self.secTags.getBluetoothEndTag())
            
            self.power = get_contents(self.secTags.getPowerHStartTag(), self.secTags.getPowerHEndTag())

            self.pcieInstalled = get_contents(self.secTags.getPcieStartTag(), self.secTags.getPcieEndTag())

            self.domInstalled = get_contents(self.secTags.getDomStartTag(), self.secTags.getDomEndTag())

            self.rfidInstalled = get_contents(self.secTags.getRfidStartTag(), self.secTags.getRfidEndTag())

    def read(self):
        if platform.system() == 'Linux':
            #read the manufactioning partition of the serial flash
            if os.path.exists(os.path.join("/","dev", "mtdblock4" )):
                try:

                    # open the manufacturing  partition
                    inFile = open(os.path.join("/","dev", "mtd4"), 'rb')

                    # read the entire partition
                    self.partition = inFile.read(CONST_PART_SIZE)

                    inFile.close()
                    #print self.partition

                except IOError as exception_details:
                    result = 'error: Reading hardware partition failed: ' + ', Reason:' + exception_details[1]
        elif platform.system() == 'Windows':
            #TO DO: test on HTx
            if os.path.exists(os.path.join("C:\Program Files", "Hobart",  "Apps" )):
                #change directories
                subprocess.Popen("cd",  "Program Files\Hobart\Apps")
                #read manufacturing partition of the serial flash to a file
                subprocess.Popen("SPIFlashTest",   "d",  "0x",  "hwDump")

                inFile = open("hwDump", 'rb')
                self.partition = inFile.read(CONST_PART_SIZE)
                inFile.close()
        else:
            print('Error: Unsupported Operating system')
            
        # handle python3 a little differently
        if sys.version_info.major == 3:
            self.partition_string = bytes([x for x in self.partition if x != 0xFF]).decode('utf-8')
        else:
            self.partition_string = "".join([x for x in self.partition if x != b'\xFF'])

    def show(self):
        print('Hardware section of serial flash')
        print('*************************************')
        print(('Section marker: ' + str(self.sectionMarker)))
        print(('Section type: ' + str(self.sectionType)))
        print(('Section write counter:' + str(self.writeCounter)))
        print(('Section crc: ' + str(self.sectionCrc)))
        print('*************************************')

        print(('Primary display type: ' + self.primaryDisplayType))
        print(('Primary display size: ' + self.primaryDisplaySize))
        print(('Primary display interface:' + self.primaryDisplayInterface))
        print(('Secondary display type: ' + self.secondaryDisplayType))
        print(('Secondary display size:' + self.secondaryDisplaySize))
        print(('Secondary display interface: ' + self.secondaryDisplayInterface))
        print(('Wifi installed: ' + self.wifiInstalled))
        print(('Sata installed: ' + self.sataInstalled))
        print(('Bluetooth installed: ' + self.bluetoothInstalled))
        print(('Pcie module installed: ' + self.pcieInstalled))
        print(('Dom installed: ' + self.domInstalled))
        print(('Rfid installed: ' + self.rfidInstalled))


class softwareSection:
    def __init__(self):
        self.sectionMarker = '0'
        self.sectionType = '2'
        self.writeCounter = '0'
        self.sectionCrc = ''
        self.sectionBlank = False
        self.metroVersion = None
        self.fractionalPricingEnabled = None
        self.poundsForPricingEnabled = None
        self.secTags = tags.configTags()
        self.partition = [] # string on python2, bytes on python3
        self.partition_string = ""
        self.w_partition = ''
        #self.partition ='0,1,1,11443<ScaleCFGSection><MetroVersion>0</MetroVersion><PoundsFor>0</PoundsFor><FractionalPrice>0</FractionalPrice></ScaleCFGSection>'

        # For Linux the partition is split... first half is bitmaps and second half is SW config
        self.partitionSize = 262144
        self.configOffset = 131072

        # ARK
        # uversion bmp in its own partion
        if os.path.exists(os.path.join("/","dev", "mtd9" )):
            self.partitionSize = 131072
            self.configOffset = 0

        self.read()
        self.init()
        #self.show() --do not show flash as chobartd uses this script

    def init(self):
        offset = self.configOffset

        #initialize the class
        if len(self.partition) > 0 and self.partition[offset] != BLANK:
            # the first part is comma-separated metadata
            metadata = self.partition_string.split(self.secTags.getSectionStartTag())[0].split(',')
            self.sectionMarker, self.sectionType, self.writeCounter, self.sectionCrc = metadata
            
            # get the contents between two tags
            def get_contents(start, end):
                return self.partition_string[self.partition_string.index(start) + len(start):self.partition_string.index(end)]

            self.metroVersion = get_contents(self.secTags.getMetroVersionStartTag(), self.secTags.getMetroVersionEndTag())
            self.fractionalPricingEnabled = get_contents(self.secTags.getFractionalPriceStartTag(), self.secTags.getFractionalPriceEndTag())
            self.poundsForPricingEnabled = get_contents(self.secTags.getPoundsForStartTag(), self.secTags.getPoundsForEndTag())

        else:
            #print 'Software serial flash partition is blank: loading defaults' --do not show flash as chobartd uses this script
            self.sectionBlank = True
            self.loadDefaults()


    def isSectionBlank(self):
        return self.sectionBlank

    def setMetroVersion(self, ver):
        self.metroVersion = ver

    def getMetroVersion(self):
        return self.metroVersion

    def setFractionalPricingEnabled(self, fpe):
        self.fractionalPricingEnabled = fpe

    def getFractionalPricingEnabled(self):
        return self.fractionalPricingEnabled

    def setPoundsForPricingEnabled(self, pfe):
        self.poundsForPricingEnabled = pfe

    def getPoundsForPricingEnabled(self):
        return self.poundsForPricingEnabled

    def getWriteCounter(self):
        return self.writeCounter

    def setWriteCounter(self, value):
        self.writeCounter = value

    def getRawPartition(self):
        return self.partition

    def write(self):
        self.serializeData()
        self.calculateCrc()
        self.serialize()

        #erase the serial flash partition
        if  platform.system() == 'Linux':
            #write the new manufacturing information to file
            outFile = open(os.path.join("/", "tmp", "sw.dat"),  'wb')
            outFile.write(self.partition)
            outFile.seek(self.configOffset)
            outFile.write(self.w_partition.encode('utf-8'))
            outFile.close()

            erase_cmd = []
            erase_cmd.append(os.path.join("/","usr","sbin","flash_eraseall"))
            erase_cmd.append(os.path.join("/", "dev","mtd5"))
            exitCode, stdout, stderr = shellCommand.execute(erase_cmd)

            if exitCode == 0:
                print('Software partition of the serial flash has been erased.')
            else:
                return ('error', stderr)

            write_cmd = []
            write_cmd.append(os.path.join("/","usr","sbin","flashcp"))
            write_cmd.append("/tmp/sw.dat")
            write_cmd.append(os.path.join("/", "dev", "mtd5"))
            exitCode, stdout, stderr = shellCommand.execute(write_cmd)

            if exitCode == 0:
                print('Software data has been saved.')
            else:
                return ('error', stderr)

        elif  platform.system() == 'Windows':

            if os.path.exists(os.path.join("C:\Program Files", "Hobart", "Bin")):

                #write out the partition contents to a file
                outFile = open("C:\\Program Files\\Hobart\\Bin\\sw.dat",  'w')
                outFile.write(b'\x00' * 4096)
                outFile.seek(0)
                outFile.write(self.w_partition)
                outFile.close()
                #wite the contents of the software partition file to the serial flash
                args = ['C:\\Program Files\\Hobart\\Bin\\SPIFlashTest.exe',  'u',  '102000',  'C:\\Program Files\\Hobart\\Bin\\sw.dat']
                osUtils.winRunElevated(args)


    def calculateCrc(self):
        # skip to the actual data
        i = self.w_partition.index(self.secTags.getSectionStartTag())
        
        # convert all characters to ints and add them up, then convert to string
        self.sectionCrc = str(sum([ord(x) for x in self.w_partition[i:]]))


    def serializeData(self):
        #serialize the data into the partition buffer to write

        #write section start tag to the partition buffer
        self.w_partition += self.secTags.getSectionStartTag()
        self.w_partition += self.secTags.getMetroVersionStartTag()
        self.w_partition += self.metroVersion
        self.w_partition += self.secTags.getMetroVersionEndTag()
        self.w_partition += self.secTags.getFractionalPriceStartTag()
        self.w_partition += self.fractionalPricingEnabled
        self.w_partition += self.secTags.getFractionalPriceEndTag()
        self.w_partition += self.secTags.getPoundsForStartTag()
        self.w_partition += self.poundsForPricingEnabled
        self.w_partition += self.secTags.getPoundsForEndTag()
        self.w_partition += self.secTags.getSectionEndTag()
        #print self.w_partition

    def serialize(self):
        #serialize the data into the partition buffer to write
        dataString = self.w_partition

        #increment the write counter
        count = int(self.writeCounter)
        count += 1
        self.writeCounter = str(count)

        self.w_partition = self.sectionMarker
        self.w_partition += ','
        self.w_partition += self.sectionType
        self.w_partition += ','
        self.w_partition += self.writeCounter
        self.w_partition += ','
        self.w_partition += self.sectionCrc

        #write section start tag to the partition buffer
        self.w_partition += dataString
        #print((self.w_partition))


    def read(self):

        if platform.system() == 'Linux':
            #read the software partition of the serial flash
            if os.path.exists(os.path.join("/","dev", "mtdblock5" )):
                try:

                    # open the manufacturing  partition
                    inFile = open(os.path.join("/","dev", "mtd5"), 'rb')

                    # read the entire partition
                    self.partition = inFile.read(self.partitionSize)

                    inFile.close()
                    #print self.partition

                except IOError as exception_details:
                    result = 'error: Reading software partition failed: ' + ', Reason:' + exception_details[1]

        elif platform.system() == 'Windows':

            if os.path.exists(os.path.join("C:\Program Files", "Hobart", "Bin" )):
                args = ['C:\\Program Files\\Hobart\\Bin\\SPIFlashTest.exe',  'd',  '102000',  'C:\\Program Files\\Hobart\\Bin\\swDump.bin']
                osUtils.winRunElevated(args)

                try:
                    inFile = open("C:\\Program Files\\Hobart\\Bin\\swDump.bin",  'rb')

                    self.partition = inFile.read(CONST_PART_SIZE)
                    #print self.partition --do not show flash as chobartd uses this script
                    inFile.close()

                except IOError as exception_details:
                    result = 'error: Reading software partition failed: ' + ', Reason:' + exception_details[1]
                    #print result
                    self.partition = ""

        else:
            print('Error: Unsupported Operating system')
        
        # skip over the offset data
        # handle python3 a little differently
        if sys.version_info.major == 3:
            self.partition_string = bytes([x for x in self.partition[self.configOffset:] if x != 0xFF]).decode('utf-8')
        else:
            self.partition_string = "".join([x for x in self.partition[self.configOffset:] if x != b'\xFF'])

    def loadDefaults(self):
        #setup default software information for the htx
        self.writeCounter = '0'
        self.sectionMarker = '1'
        self.sectionType = '2'
        self.sectionCrc = '0'

        self.metroVersion = '0'
        self.fractionalPricingEnabled = '0'
        self.poundsForPricingEnabled = '0'

        #print 'Entering loadDefaultss'
        if platform.system() == 'Windows':
            #serialize the partition data
            self.serializeData()
            self.calculateCrc()
            self.serialize()

            #print 'w_partition length: ' + str( len(self.w_partition) )
            #append blank characters from the end of the data to the end of the partition
            for i in range(len(self.w_partition),  CONST_PART_SIZE):
                self.w_partition += b'\xff'

            #print self.w_partition

            if os.path.exists(os.path.join("C:\Program Files", "Hobart", "Bin")):

                #write out the partition contents to a file
                outFile = open("C:\\Program Files\\Hobart\\Bin\\sw.dat",  'w')
                outFile.write(b'\x00' * 4096)
                outFile.seek(0)
                outFile.write(self.w_partition)
                outFile.close()
                #wite the contents of the software partition file to the serial flash
                args = ['C:\\Program Files\\Hobart\\Bin\\SPIFlashTest.exe',  'u',  '102000',  'C:\\Program Files\\Hobart\\Bin\\sw.dat']
                osUtils.winRunElevated(args)

        elif platform.system() == 'Linux':
            #append blank characters from the end of the data to the end of the partition
            for i in range(len(self.partition),  self.partitionSize):
                self.partition.append( b'\xff' )


    def show(self):
        print('Software section of serial flash')
        print('*************************************')
        print(('Section marker: ' + str(self.sectionMarker)))
        print(('Section type: ' + str(self.sectionType)))
        print(('Section write counter:' + str(self.writeCounter)))
        print(('Section crc: ' + str(self.sectionCrc)))
        print('*************************************')

        print(('Metrology version: ' + self.metroVersion))
        print(('Fractional pricing enabled: ' + self.fractionalPricingEnabled))
        print(('Pounds For pricing enabled: ' + self.poundsForPricingEnabled))

