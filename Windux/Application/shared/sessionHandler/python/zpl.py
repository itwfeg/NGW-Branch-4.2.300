class zpl_commands: 
    def __init__(self):
        self.startFormat =              "^XA"
        self.endFormat =                "^XZ"
        self.dowloadGraphics =      "~DG"
        self.graphicsField =            "^GF"
        self.fieldTypeSet =             "^FT" 
        self.fieldOrigin =                "^FO" 
        self.fieldSeperator =          "^FS"
        self.printMode =                "^MM"
        self.printWidth =                "^PW"
        self.labelLength =              "^LL"
        self.labelShift =                  "^LS"
        self.recallGraphic =            "^XG"
        self.deleteGraphic =           "^ID"
        self.fontEncoding =            "^CI"
        self.tearOffPos=                 "~TA"
        self.backFeed=                   "~JS"
        self.LabelTopPos=               "^LT"
        self.mediaTracking=           "^MN"
        self.mediaType=                 "^MT"
        self.printOrientation=         "^PO" 
        self.printMirror=                 "^PM"
        self.labelHome=                 "^LH"
        self.printDensity=               "^JM"
        self.printRate=                    "^PR"
        self.printDarkness=             "~SD"
        self.configUpdate=              "^JU"
        self.labelReverse=               "^LR"
        self.printQuantity=               "^PQ"
        self.changeCarets=              "~CC"
        self.changeDelimiter=          "~CD"
        self.changeTilde=                 "~CT"
        self.downloadPNG =              "~DY"

    def getStartFormat(self):
        return self.startFormat
    def getEndFormat(self):
        return self.endFormat
    def getDownloadGraphics(self):
        return self.dowloadGraphics
    def getGraphicsField(self):
        return self.graphicsField
    def getFieldTypeSet(self):
        return self.fieldTypeSet
    def getFieldSeperator(self):
        return self.fieldSeperator
    def getFieldOrigin(self):
        return self.fieldOrigin
    def getPrintMode(self):
        return self.printMode
    def getPrinterLabelWidth(self):
        return self.printWidth
    def getPrinterLabelLength(self):
        return self.labelLength
    def getLabelShift(self):
        return self.labelShift
    def getRecallGraphic(self):
        return self.recallGraphic
    def getDeleteGraphic(self):
        return self.deleteGraphic
    def getTareOffPosition(self):
        return self.tearOffPos
    def getBackfeed(self):
        return self.backFeed
    def getLabelTopPosition(self):
        return self.LabelTopPos
    def getMediaTracking(self):
        return self.mediaTracking
    def getMediaType(self):
        return self.mediaType
    def getPrintOrientation(self):
        return self.printOrientation
    def getPrintMirrorImage(self):
        return self.printMirror
    def getLabelHomePosition(self):
        return self.labelHome
    def getPrintDensity(self):
        return self.printDensity
    def getPrintRate(self):
        return self.printRate
    def getPrinterConfig(self):
        return self.configUpdate
    def getLabelReverse(self):
        return self.labelReverse
    def getPrintContrast(self):
        return self.printDarkness
    def getFontEncoding(self):
        return self.fontEncoding
    def getPrintQuantity(self):
        return self.printQuantity
    def getChangeCarets(self):
        return self.changeCarets
    def getChangeDelimiter(self):
        return self.changeDelimiter
    def getChangeTilde(self):
        return self.changeTilde
    def getDownloadGraphic_all(self):
        return self.downloadPNG

    # Setup of Graphics Download command  (sets must be used in order!!!)###########
    def setGraphicsName(self,  name):
        self.dowloadGraphics += name + ".GRF"

    def setGraphicSize(self,  size):
        self.dowloadGraphics += "," + str(size)
    
    def setGraphicWidth(self,  width):
        self.dowloadGraphics += "," + str(width)
    
    def setGraphicData(self,  data):
        self.dowloadGraphics += "," + "," + data
        
    def setGraphicNameAll(self,  name):
        self.downloadPNG +=name +","+ "P" + "," + ".PNG" 
    def setGraphicSizeFile(self,  size):
        self.downloadPNG+= "," + str(size)
    def setGraphicWidthAll(self,  width):
        self.downloadPNG += "," + str(width)
    def setGraphicDataAll(self,  data):
        self.downloadPNG += "," +  "," + data
 

    # Setup of Graphics Field command (sets must be used in order!!!)###########
    def setFieldCompressionAscii(self):
        self.graphicsField += "A"
    def setFieldCompressionBinary(self):
        self.graphicsField += "B"
    
    def setGraphicFieldStreamSize(self,  size):
        self.graphicsField += "," +   str(size)

    def setGraphicFieldSize(self,  size):
        self.graphicsField += "," +   str(size)

    def setGraphicFieldWidth(self,  width): 
        self.graphicsField += "," +   str(width)
        
    
    # Setup of Field Type command###########
    def setFieldTypePosition(self,  x,  y):
        self.fieldTypeSet += str(x)   + "," + str(y)
        
    def setFieldOriginX(self,  x):
        self.fieldOrigin += x
        
    def setFieldOrigin(self,  x,  y):
        self.fieldOrigin += x + "," + y
    
    # Setup of Print Mode command###########        
    def setPrintMode(self):
        self.printMode += "T"
     
    def setPrinterWidthOfLabel(self,  width):
        self.printWidth += str(width * 203)
     
    # Setup of label length command###########   
    def setPrinterLabelLength(self,  y):
        #gk420t has an 8 dot / mm head 
        self.labelLength += str( y )   
        
        
    def setLabelShift(self):
        self.labelShift += "0"
    
    # Setup recall graphic command with default magnification ########### 
    def setRecallGraphic(self,  name,  format=".GRF"):
        self.recallGraphic += name + format + "," + "1,1"
        
        
    def setDeleteGraphic(self,  name):
        self.deleteGraphic += name + ".GRF"
        
    def setFontEncoding(self,  code = 'us'):
        if code == "us":
            self.fontEncoding += "0"

    #command lets you adjust the rest position of the media after a label is printed   
    def setTareOffPosition(self,  pos):
        position = ""
        if pos > -120 and pos <= 120:
            if pos < 100:
                position += "0" 
                if pos < 10:
                    position += "0"
            position += str(pos) 
            self.tearOffPos += str(pos) 
    
    def setBackfeed(self,  percentage):
        if percentage == 100:
            self.backFeed+= 'A'
        elif percentage == 0:
            self.backFeed+= 'B'
        elif percentage == 90:       #default
            self.backFeed+= 'N'
        else:
            self.backFeed += str(percentage)
        
    def setLabelTopPosition(self,  pos_rows):
        if pos_rows <= 120 or pos_rows >= -120:     #range is 120 to -120
            self.LabelTopPos += str(pos_rows)
        
    def setMediaTracking(self,  track = 'W'):
        self.mediaTracking += track  
    
    def setMediaType(self,  type = 'Thermal'):
        if type == 'Thermal':
            self.mediaType  += 'T'
        elif type == 'Direct':
            self.mediaType += 'D'
    
    def setPrintOrientation(self,  invert = False):
        if invert == True:
            self.printOrientation += 'I'
        else:
            self.printOrientation += 'N'
        
    def setPrintMirrorImage(self,  mirror = False):
        if mirror != True:
            self.printMirror += 'N'
        else:
            self.printMirror += 'Y'
        
    def setLabelHomePosition(self,  x, y):
        if x >= 0 and x <= 32000 and y >= 0 and y <= 32000:
            self.labelHome += str(x) +"," +str(y)
        else:
            self.labelHome += "0,0"
            
    def setPrintDensity(self,  density = 'A'):
        self.printDensity += density
        
    def setPrintRate(self,  rate = "5,5"):
        self.printRate += rate
        
    def setPrintContrast(self,  contrast = 15):
        if contrast <= 0 and  contrast <= 30:
            self.printDarkness += str(contrast)
    
    def setPrinterConfiguration(self,  action = "save"):
        if action == "save":
            self.configUpdate += "S"
        if action == "default":
            self.configUpdate += 'F' #reload factory defaults
        if action == "recall":
            self.configUpdate += 'R' #recall last saved values
            
    def setLabelReverse(self, reverse = False):
        if reverse == False:
            self.labelReverse += 'N'
        else:
            self.labelReverse += 'Y'
            
    def setPrintQuantity( self, quantity = "1", pause = "0", replicate = "1", override_pause = "Y" ):
        self.printQuantity +=  quantity + "," + pause + "," + replicate + "," + override_pause
        
    def setChangeCarets(self,  symbol):
        self.changeCarets += symbol
    def setChangeDelimiter(self,  symbol):
        self.changeDelimiter += symbol
    def setChangeTilde(self,  symbol):
        self.changeTilde += symbol
        
        
        
        
