import sys
import subprocess
import os
import tags
import shellCommand
import osUtils
import platform

CONST_PART_SIZE = 4096
BLANK = '\xff'

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
        self.partition = []
        self.w_partition = ''
        #self.partition ='0,1,1,11443<MFGSection><SStyle>0</SStyle><BDAssembly>45-1084-982</BDAssembly><BDRev>1.0</BDRev><MFGDate>11302012</MFGDate><Serial>1234567</Serial></MFGSection>'

        self.read()
        self.init()
        #self.show() --do not show flash as chobartd uses this script

    def init(self):
        #initialize the class
        if len(self.partition) > 0:

            self.sectionMarker = self.partition[0]
            self.sectionType = self.partition[2]
            self.writeCounter = self.partition[4]

            if self.sectionMarker != BLANK:

                i = 6
                while self.partition[i] != '<':
                    self.sectionCrc += self.partition[i]
                    i += 1

                #find the index of the start tag for the style field
                try:
                    index = self.partition.index(self.secTags.getStyleStartTag())
                    #offset the length of the tag
                    index += len(self.secTags.getStyleStartTag())
                    self.scaleStyle = str(self.partition[ index ])
                    #if self.scaleStyle != '0' or self.scaleStyle != 'H':
                    #   self.scaleStyle = self.scaleStyle[1 : -1]
                    index += 1

                    while str(self.partition[index]) != '<':
                        self.scaleStyle += str(self.partition[index])
                        index += 1

                    if len(self.scaleStyle) >= 3:
                        self.scaleStyle = self.scaleStyle.strip()
                        self.scaleStyle = self.scaleStyle.strip('"')
                        if "EPCP" in self.scaleStyle.upper():
                            if len(self.scaleStyle) >= 8:
                                self.scaleStyle = self.scaleStyle[:8].upper() + self.scaleStyle[8:].lower()
                        elif "NGW1" in self.scaleStyle.upper() or "NGW2" in self.scaleStyle.upper():
                            self.scaleStyle = self.scaleStyle.upper()
                        else:
                            self.scaleStyle = self.scaleStyle[:2].upper() + self.scaleStyle[2].lower() + self.scaleStyle[3:]
                    else:
                        self.scaleStyle = ""
                except:
                    self.scaleStyle = ""

                try:
                    index = self.partition.index(self.secTags.getSystemStyleStartTag()) + len(self.secTags.getSystemStyleStartTag())
                    endIndex = self.partition.index(self.secTags.getSystemStyleEndTag())
                    self.systemStyle = self.partition[index:endIndex]

                    #find the index of the start tag for the assembly field
                    index = self.partition.index(self.secTags.getAssemblyStartTag())
                    #offset the length of the tag
                    index += len(self.secTags.getAssemblyStartTag())
                    self.boardAssembly = str(self.partition[ index ] )
                    index += 1
                    while str(self.partition[index]) != '<':
                        self.boardAssembly += str(self.partition[index])
                        index += 1
                except:
                    self.boardAssembly = ""
                    self.systemStyle = "HTi-7LS"

                try:
                    #find the index of the start tag for the revision field
                    index = self.partition.index(self.secTags.getRevStartTag())
                    #offset the length of the tag
                    index += len(self.secTags.getRevStartTag())
                    self.boardRev = str(self.partition[index])
                    index += 1
                    while  str(self.partition[index]) != '<':
                        self.boardRev += str(self.partition[index])
                        index += 1
                except:
                    self.boardRev = ""

                try:
                    #find the index of the start tag for the date field
                    index = self.partition.index(self.secTags.getDateStartTag())
                    #offset the length of the tag
                    index += len(self.secTags.getDateStartTag())
                    self.dateOfManufacture = self.partition[index]
                    index += 1
                    while str(self.partition[index]) != '<':
                        self.dateOfManufacture += str(self.partition[index])
                        index += 1
                except:
                    self.dateOfManufacture = ""

                try:
                    #find the index of the start tag for the date field
                    index = self.partition.index(self.secTags.getSerialStartTag())
                    #offset the length of the tag
                    index += len(self.secTags.getSerialStartTag())
                    self.serialNumber = str(self.partition[index])
                    index += 1
                    while str(self.partition[index]) != '<':
                        self.serialNumber += str(self.partition[index])
                        index += 1
                except:
                    self.serialNumber = ""

                #check if fields have additional qoutes
                self.boardAssembly = self.boardAssembly.strip('"')
                self.boardRev = self.boardRev.strip('"')
                self.dateOfManufacture =  self.dateOfManufacture.strip('"')
                self.serialNumber = self.serialNumber.strip('"')

            elif self.sectionMarker == BLANK:
                #print 'Manufacturing serial flash partition is blank: loading defaults' --do not show flash as chobartd uses this script
                self.sectionBlank = True
                self.loadDefaults()
            else:
                #print 'Manufacturting serial flash partition is blank: loading defaults' --do not show flash as chobartd uses this script
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
                print'Manufacturing partition of the serial flash has been erased.'
            else:
                return ('error', stderr)

            write_cmd = []
            write_cmd.append(os.path.join("/","usr","sbin","flashcp"))
            write_cmd.append("/tmp/mfg.dat")
            write_cmd.append(os.path.join("/", "dev", "mtd3"))
            exitCode, stdout, stderr = shellCommand.execute(write_cmd)

            if exitCode == 0:
                print'Manufacturing data has been saved.'
            else:
                return ('error', stderr)

        elif  platform.system() == 'Windows':

            if os.path.exists(os.path.join("C:\Program Files", "Hobart", "Bin")):

                #write out the partition contents to a file
                outFile = open("C:\\Program Files\\Hobart\\Bin\\mfg.dat",  'w')
                outFile.write('\x00' * 4096)
                outFile.seek(0)
                outFile.write(self.w_partition)
                outFile.close()
                #wite the contents of the manufacturing partition file to the serial flash
                args = ['C:\\Program Files\\Hobart\\Bin\\SPIFlashTest.exe',  'u',  '100000',  'C:\\Program Files\\Hobart\\Bin\\mfg.dat']
                osUtils.winRunElevated(args)

    def calculateCrc(self):
        self.sectionCrc = 0

        #determine the size of our section data
        size = len(self.w_partition)
        #size -= self.w_partition.index(self.secTags.getSectionStartTag())
        i = self.w_partition.index(self.secTags.getSectionStartTag())

        #calculate the crc for the partition
        while i != size:
            self.sectionCrc   += ord(self.w_partition[i])
            #print self.sectionCrc
            i += 1
        #print self.sectionCrc

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

                except IOError, exception_details:
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

                except IOError, exception_details:
                    result = 'error: Reading manufacturing partition failed: ' + ', Reason:' + exception_details[1]
                    #print result
                    self.partition = ""

        else:
            print 'Error: Unsupported Operating system'

    def loadDefaults(self):
        #manufacturing test has not been implemented for htx
        if platform.system() == 'Windows':
            #setup default manufacturing information for the htx
            self.writeCounter = '0'
            self.sectionMarker = '1'
            self.sectionType = '1'
            self.sectionCrc  = '0'

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
                self.w_partition += '\xff'

            #print self.w_partition

            if os.path.exists(os.path.join("C:\Program Files", "Hobart", "Bin")):

                #write out the partition contents to a file
                outFile = open("C:\\Program Files\\Hobart\\Bin\\mfg.dat",  'w')
                outFile.write('\x00' * 4096)
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
            self.sectionCrc  = '0'
            #scale style must be determined and set before loading defaults
            #self.scaleStyle = 'HT'
            self.boardAssembly = '00-447159'
            self.boardRev = '0.3'
            self.dateOfManufacture = '03032014'
            self.serialNumber = ''

    def show(self):
        print 'Manufacturing section of serial flash'
        print '*************************************'
        print 'Section marker: ' + str(self.sectionMarker)
        print 'Section type: ' + str(self.sectionType)
        print 'Section write counter:' + str(self.writeCounter)
        print 'Section crc: ' + str(self.sectionCrc)
        print '*************************************'

        if self.scaleStyle == '0':
            print 'Scale style: ' + 'HTi-7LS'
        else:
            print 'Scale style: ' + self.scaleStyle

        print 'Board assembly: ' + self.boardAssembly
        print 'Board revision: ' + self.boardRev
        print 'Date of manufacture: ' + self.dateOfManufacture
        print 'Scale serial number: ' + self.serialNumber


class hadwareSection:
    def __init__(self):
        self.sectionMarker = '0'
        self.sectionType = '3'
        self.writeCounter = '0'
        self.sectionCrc = '0'

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
        self.partition = []

        self.read()
        self.init()
        self.show()

    def init(self):

        #initialize the class
        if len(self.partition) > 0:

            self.sectionMarker = self.partition[0]
            self.sectionType = self.partition[1]
            self.writeCounter = self.partition[2]
            self.sectionCrc = self.partition[3]

            #find the index of the start tag for the  field
            index = self.partition.index(self.secTags.getPrimaryDisplayStartTag())
            #offset the length of the tag
            index += len(self.secTags.getPrimaryDisplayStartTag())
            self.primaryDisplayType = str(self.partition[ index ])
            index += 1
            while str(self.partition[index]) != '<':
                self.primaryDisplayType += str(self.partition[index])
                index += 1

            #find the index of the start tag for the  field
            index = self.partition.index(self.secTags.getPrimaryDisplaySizeStartTag())
            #offset the length of the tag
            index += len(self.secTags.getPrimaryDisplaySizeStartTag())
            self.primaryDisplaySize = str(self.partition[ index ] )
            index += 1
            while str(self.partition[index]) != '<':
                self.primaryDisplaySize += str(self.partition[index])
                index += 1

            #find the index of the start tag for the  field
            index = self.partition.index(self.secTags.getPrimaryDisplayInterfaceStartTag())
            #offset the length of the tag
            index += len(self.secTags.getPrimaryDisplayInterfaceStartTag())
            self.primaryDisplayInterface = str(self.partition[index])
            index += 1
            while  str(self.partition[index]) != '<':
                self.primaryDisplayInterface += str(self.partition[index])
                index += 1

            #find the index of the start tag for the  field
            index = self.partition.index(self.secTags.getSecondaryDisplayStartTag())
            #offset the length of the tag
            index += len(self.secTags.getSecondaryDisplayStartTag())
            self.secondaryDisplayType = self.partition[index]
            index += 1
            while str(self.partition[index]) != '<':
                self.secondaryDisplayType += str(self.partition[index])
                index += 1

            #find the index of the start tag for the  field
            index = self.partition.index(self.secTags.getSecondaryDisplaySizeStartTag())
            #offset the length of the tag
            index += len(self.secTags.getSecondaryDisplaySizeStartTag())
            self.secondaryDisplaySize = str(self.partition[index])
            index += 1
            while str(self.partition[index]) != '<':
                self.secondaryDisplaySize += str(self.partition[index])
                index += 1

            #find the index of the start tag for the  field
            index = self.partition.index(self.secTags.getSecondaryInterfaeStartTag())
            #offset the length of the tag
            index += len(self.secTags.getSecondaryInterfaeStartTag())
            self.secondaryDisplayInterface = str(self.partition[index])
            index += 1
            while str(self.partition[index]) != '<':
                self.secondaryDisplayInterface += str(self.partition[index])
                index += 1

            #find the index of the start tag for the  field
            index = self.partition.index(self.secTags.getWifiStartTag())
            #offset the length of the tag
            index += len(self.secTags.getWifiStartTag())
            self.wifiInstalled = str(self.partition[index])
            index += 1
            while str(self.partition[index]) != '<':
                self.wifiInstalled += str(self.partition[index])
                index += 1

            #find the index of the start tag for the  field
            index = self.partition.index(self.secTags.getSataStartTag())
            #offset the length of the tag
            index += len(self.secTags.getSataStartTag())
            self.sataInstalled = str(self.partition[index])
            index += 1
            while str(self.partition[index]) != '<':
                self.sataInstalled += str(self.partition[index])
                index += 1

            #find the index of the start tag for the  field
            index = self.partition.index(self.secTags.getBluetoothStartTag())
            #offset the length of the tag
            index += len(self.secTags.getBluetoothStartTag())
            self.bluetoothInstalled = str(self.partition[index])
            index += 1
            while str(self.partition[index]) != '<':
                self.bluetoothInstalled += str(self.partition[index])
                index += 1

            #find the index of the start tag for the  field
            index = self.partition.index(self.secTags.getPowerHStartTag())
            #offset the length of the tag
            index += len(self.secTags.getPowerHStartTag())
            self.power = str(self.partition[index])
            index += 1
            while str(self.partition[index]) != '<':
                self.power += str(self.partition[index])
                index += 1

            #find the index of the start tag for the  field
            index = self.partition.index(self.secTags.getPcieStartTag())
            #offset the length of the tag
            index += len(self.secTags.getPcieStartTag())
            self.pcieInstalled = str(self.partition[index])
            index += 1
            while str(self.partition[index]) != '<':
                self.pcieInstalled += str(self.partition[index])
                index += 1

            #find the index of the start tag for the  field
            index = self.partition.index(self.secTags.getDomStartTag())
            #offset the length of the tag
            index += len(self.secTags.getDomStartTag())
            self.domInstalled = str(self.partition[index])
            index += 1
            while str(self.partition[index]) != '<':
                self.domInstalled += str(self.partition[index])
                index += 1

            #find the index of the start tag for the  field
            index = self.partition.index(self.secTags.getRfidStartTag())
            #offset the length of the tag
            index += len(self.secTags.getRfidStartTag())
            self.rfidInstalled = str(self.partition[index])
            index += 1
            while str(self.partition[index]) != '<':
                self.rfidInstalled += str(self.partition[index])
                index += 1

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

                except IOError, exception_details:
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
            print 'Error: Unsupported Operating system'

    def show(self):
        print 'Hardware section of serial flash'
        print '*************************************'
        print 'Section marker: ' + str(self.sectionMarker)
        print 'Section type: ' + str(self.sectionType)
        print 'Section write counter:' + str(self.writeCounter)
        print 'Section crc: ' + str(self.sectionCrc)
        print '*************************************'

        print 'Primary display type: ' + self.primaryDisplayType
        print 'Primary display size: ' + self.primaryDisplaySize
        print 'Primary display interface:' + self.primaryDisplayInterface
        print 'Secondary display type: ' + self.secondaryDisplayType
        print 'Secondary display size:' + self.secondaryDisplaySize
        print 'Secondary display interface: ' + self.secondaryDisplayInterface
        print 'Wifi installed: ' + self.wifiInstalled
        print 'Sata installed: ' + self.sataInstalled
        print 'Bluetooth installed: ' + self.bluetoothInstalled
        print 'Pcie module installed: ' + self.pcieInstalled
        print 'Dom installed: ' + self.domInstalled
        print 'Rfid installed: ' + self.rfidInstalled


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
        self.partition = []
        self.w_partition = ''
        #self.partition ='0,1,1,11443<ScaleCFGSection><MetroVersion>0</MetroVersion><PoundsFor>0</PoundsFor><FractionalPrice>0</FractionalPrice></ScaleCFGSection>'

        # For Linux the partition is split... first half is bitmaps and second half is SW config
        self.partitionSize = 262144
        self.configOffset = 131072

        self.read()
        self.init()
        #self.show() --do not show flash as chobartd uses this script

    def init(self):

        offset = 0
        if platform.system() == 'Linux':
            offset = self.configOffset

        #initialize the class
        if len(self.partition) > offset:

            self.sectionMarker = self.partition[offset+0]
            self.sectionType = self.partition[offset+2]
            self.writeCounter = self.partition[offset+4]

            if self.sectionMarker != BLANK:

                i = offset+6
                while self.partition[i] != '<':
                    self.sectionCrc += self.partition[i]
                    i += 1

                #find the index of the start tag for the  field
                index = self.partition.index(self.secTags.getMetroVersionStartTag())
                #offset the length of the tag
                index += len(self.secTags.getMetroVersionStartTag())
                self.metroVersion = str(self.partition[ index ])
                index += 1
                while str(self.partition[index]) != '<':
                    self.metroVersion += str(self.partition[index])
                    index += 1

                #find the index of the start tag for the  field
                index = self.partition.index(self.secTags.getFractionalPriceStartTag())
                #offset the length of the tag
                index += len(self.secTags.getFractionalPriceStartTag())
                self.fractionalPricingEnabled = str(self.partition[index])
                index += 1
                while  str(self.partition[index]) != '<':
                    self.fractionalPricingEnabled += str(self.partition[index])
                    index += 1

                #find the index of the start tag for the  field
                index = self.partition.index(self.secTags.getPoundsForStartTag())
                #offset the length of the tag
                index += len(self.secTags.getPoundsForStartTag())
                self.poundsForPricingEnabled = str(self.partition[ index ] )
                index += 1
                while str(self.partition[index]) != '<':
                    self.poundsForPricingEnabled += str(self.partition[index])
                    index += 1

            else:
                #print 'Software serial flash partition is blank: loading defaults' --do not show flash as chobartd uses this script
                self.sectionBlank = True
                self.loadDefaults()

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
            outFile.write(self.w_partition)
            outFile.close()

            erase_cmd = []
            erase_cmd.append(os.path.join("/","usr","sbin","flash_eraseall"))
            erase_cmd.append(os.path.join("/", "dev","mtd5"))
            exitCode, stdout, stderr = shellCommand.execute(erase_cmd)

            if exitCode == 0:
                print'Software partition of the serial flash has been erased.'
            else:
                return ('error', stderr)

            write_cmd = []
            write_cmd.append(os.path.join("/","usr","sbin","flashcp"))
            write_cmd.append("/tmp/sw.dat")
            write_cmd.append(os.path.join("/", "dev", "mtd5"))
            exitCode, stdout, stderr = shellCommand.execute(write_cmd)

            if exitCode == 0:
                print'Software data has been saved.'
            else:
                return ('error', stderr)

        elif  platform.system() == 'Windows':

            if os.path.exists(os.path.join("C:\Program Files", "Hobart", "Bin")):

                #write out the partition contents to a file
                outFile = open("C:\\Program Files\\Hobart\\Bin\\sw.dat",  'w')
                outFile.write('\x00' * 4096)
                outFile.seek(0)
                outFile.write(self.w_partition)
                outFile.close()
                #wite the contents of the software partition file to the serial flash
                args = ['C:\\Program Files\\Hobart\\Bin\\SPIFlashTest.exe',  'u',  '102000',  'C:\\Program Files\\Hobart\\Bin\\sw.dat']
                osUtils.winRunElevated(args)


    def calculateCrc(self):
        calcCrc = 0

        #determine the size of our section data
        size = len(self.w_partition)
        #size -= self.w_partition.index(self.secTags.getSectionStartTag())
        i = self.w_partition.index(self.secTags.getSectionStartTag())

        #calculate the crc for the partition
        while i != size:
            calcCrc += ord(self.w_partition[i])
            #print calcCrc
            i += 1
        self.sectionCrc = str(calcCrc)
        #print self.sectionCrc


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
        print self.w_partition


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

                except IOError, exception_details:
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

                except IOError, exception_details:
                    result = 'error: Reading software partition failed: ' + ', Reason:' + exception_details[1]
                    #print result
                    self.partition = ""

        else:
            print 'Error: Unsupported Operating system'

    def loadDefaults(self):
        #setup default software information for the htx
        self.writeCounter = '0'
        self.sectionMarker = '1'
        self.sectionType = '2'
        self.sectionCrc  = 0

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
                self.w_partition += '\xff'

            #print self.w_partition

            if os.path.exists(os.path.join("C:\Program Files", "Hobart", "Bin")):

                #write out the partition contents to a file
                outFile = open("C:\\Program Files\\Hobart\\Bin\\sw.dat",  'w')
                outFile.write('\x00' * 4096)
                outFile.seek(0)
                outFile.write(self.w_partition)
                outFile.close()
                #wite the contents of the software partition file to the serial flash
                args = ['C:\\Program Files\\Hobart\\Bin\\SPIFlashTest.exe',  'u',  '102000',  'C:\\Program Files\\Hobart\\Bin\\sw.dat']
                osUtils.winRunElevated(args)

        elif platform.system() == 'Linux':
            #append blank characters from the end of the data to the end of the partition
            for i in range(len(self.partition),  self.partitionSize):
                self.partition.append( '\xff' )


    def show(self):
        print 'Software section of serial flash'
        print '*************************************'
        print 'Section marker: ' + str(self.sectionMarker)
        print 'Section type: ' + str(self.sectionType)
        print 'Section write counter:' + str(self.writeCounter)
        print 'Section crc: ' + str(self.sectionCrc)
        print '*************************************'

        print 'Metrology version: ' + self.metroVersion
        print 'Fractional pricing enabled: ' + self.fractionalPricingEnabled
        print 'Pounds For pricing enabled: ' + self.poundsForPricingEnabled

