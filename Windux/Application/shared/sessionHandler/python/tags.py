class mfgTags:
    def __init__(self):
        self.sectionStart = '<MFGSection>'
        self.styleStart = '<SStyle>'
        self.styleEnd = '</SStyle>'
        self.systemStyleStart = '<SystemStyle>'
        self.systemStyleEnd = '</SystemStyle>'
        self.assemblyStart = '<BDAssembly>'
        self.assemblyEnd ='</BDAssembly>'
        self.revStart = '<BDRev>'
        self.revEnd = '</BDRev>'
        self.dateStart = '<MFGDate>'
        self.dateEnd = '</MFGDate>'
        self.serialStart = '<Serial>'
        self.serialEnd = '</Serial>'
        self.sectionEnd = '</MFGSection>'

    def getSectionStartTag(self):
        return self.sectionStart
    def getSectionEndTag(self):
        return self.sectionEnd
    def getSystemStyleStartTag(self):
        return self.systemStyleStart
    def getSystemStyleEndTag(self):
        return self.systemStyleEnd
    def getStyleStartTag(self):
        return self.styleStart
    def getStyleEndTag(self):
        return self.styleEnd
    def getAssemblyStartTag(self):
        return self.assemblyStart
    def getAssemblyEndTag(self):
        return self.assemblyEnd
    def getRevStartTag(self):
        return self.revStart
    def getRevEndTag(self):
        return self.revEnd
    def getDateStartTag(self):
        return self.dateStart
    def getDateEndTag(self):
        return self.dateEnd
    def getSerialStartTag(self):
        return self.serialStart
    def getSerialEndTag(self):
        return self.serialEnd

class configTags:
    def __init__(self):
        self.sectionStart = '<ScaleCFGSection>'
        self.modelStart = '<Model>'
        self.modelEnd = '</Model>'
        self.mainStart = '<PHMain>'
        self.mainEnd = '</PHMain>'
        self.percisionStart = '<Prec>'
        self.percisionEnd = '</Prec>'
        self.unitPriceStart = '<MaxUPrice>'
        self.unitPriceEnd = '</MaxUPrice>'
        self.totalPriceStart = '<MaxTPrice>'
        self.totalPriceEnd =  '</MaxTPrice>'
        self.maxLdaysStart = '<MaxLDays>'
        self.maxLdaysEnd ='</MaxLDays>'
        self.maxLHourStart = '<MaxLHour>'
        self.maxLHourEnd = '</MaxLHour>'
        self.bonusStart = '<BonusP>'
        self.bonusEnd = '</BonusP>'
        self.quarterStart = '<QuarterP>'
        self.quarterEnd = '</QuarterP>'
        self.halfStart = '<HalfP>'
        self.halfEnd = '</HalfP>'
        self.gramsStart = '<GramsFor>'
        self.gramsEnd = '</GramsFor>'
        self.maxLabelStart = '<MaxLabel>'
        self.maxLabelEnd = '</MaxLabel>'
        self.expLabelStart = '<ExpLabelFam>'
        self.expLabelEnd = '</ExpLabelFam>'
        self.expLabelExpStart = '<ExpLabelExp>'
        self.expLabelExpEnd = '</ExpLabelExp>'
        self.sLoginIdStart = '<SLoginId>'
        self.sLoginIdEnd = '</SLoginId>'
        self.sKeyTimeStart = '<SKeyTime>'
        self.sKeyTimeEnd = '</SKeyTime>'
        self.rfidEnStart = '<RFIDEn>'
        self.rfidEnEnd = '</RFIDEn>'
        self.rfidDbStart = '<RFIDdeb>'
        self.rfidDbEnd = '</RFIDdeb>'
        self.rfidComStart = '<RFIDcom>'
        self.rfidComEnd = '</RFIDcom>'
        self.conStatStart = '<ConStatDelay>'
        self.conStatEnd = '</ConStatDelay>'
        self.lostPerStart = '<LostPer>'
        self.lostPerEnd = '</LostPer>'
        self.avoirStart = '<Avoir>'
        self.avoirEnd = '</Avoir>'
        self.metricStart = '<Metric>'
        self.metricEnd = '</Metric>'
        self.lbhoffStart = '<LbHOff>'
        self.lbhoffEnd = '</LbHOff>'
        self.upConfigStart = '<UPConfig>'
        self.upConfigEnd = '</UPConfig>'
        self.heelStart = '<Heel>'
        self.heelEnd = '</Heel>'
        self.testLabStart = '<TestLab>'
        self.testLabEnd = '</TestLab>'
        self.ixferTStart = '<IXferT>'
        self.ixferTEnd = '</IXferT>'
        self.bxferTStart = '<BXferT>'
        self.bxferTEnd = '</BXferT>'
        self.usbStart = '<USBEn>'
        self.usbEnd = '</USBEn>'
        self.syncStart = '<SyncEn>'
        self.syncEnd = '</SyncEn>'
        self.gBeepStart = '<GBeepEn>'
        self.gBeepEnd = '</GBeepEn>'
        self.bBeepStart = '<BBeepEn>'
        self.bBeepEnd = '</BBeepEn>'
        self.pingFreqStart = '<PingFreq>'
        self.pingFreqEnd = '</PingFreq>'
        self.pingAttStart = '<PingAtt>'
        self.pingAttEnd = '</PingAtt>'
        self.metroVersionStart = '<MetroVersion>'
        self.metroVersionEnd = '</MetroVersion>'
        self.poundsForStart = '<PoundsFor>'
        self.poundsForEnd = '</PoundsFor>'
        self.fractionalPriceStart = '<FractionalPrice>'
        self.fractionalPriceEnd = '</FractionalPrice>'
        self.sectionEnd = '</ScaleCFGSection>'

    def getSectionStartTag(self):
        return self.sectionStart
    def getSectionEndTag(self):
        return self.sectionEnd
    def getModelStartTag(self):
        return self.modelStart
    def getMainStartTag(self):
        return self.mainStart
    def getPercisionStart(self):
        return self.percisionStart
    def getUnitPriceStartTag(self):
        return self.unitPriceStart
    def getTotalPriceStartTag(self):
        return self.totalPriceStart
    def getMaxLdaysStartTag(self):
        return self.maxLdaysStart
    def getMaxLHourStartTag(self):
        return self.maxLHourStart
    def getBonusStartTag(self):
        self.bonusStart
    def getQuarterStartTag(self):
        return self.quarterStart
    def getHalfStartTag(self):
        return self.halfStart
    def getGramsStartTag(self):
        return self.gramsStart
    def getMaxLabelStartTag(self):
        return self.maxLabelStart
    def getExpLabelStartTag(self):
        return self.expLabelStart
    def getExpLabelExpStartTag(self):
        return self.expLabelExpStart
    def getSLoginIdStartTag(self):
        return self.sLoginIdStart
    def getSKeyTimeStartTag(self):
        return self.sKeyTimeStart
    def getRfidEnStartTag(self):
        return self.rfidEnStart
    def getRfidDbStartTag(self):
        return self.rfidDbStart
    def getRfidComStartTag(self):
        return self.rfidComStart
    def getConStatStartTag(self):
        return self.conStatStart
    def getLostPerStartTag(self):
        return self.lostPerStart
    def getAvoirStartTag(self):
        return self.avoirStart
    def getMetricStartTag(self):
        return self.metricStart
    def getIbhoffStartTag(self):
        return self.lbhoffStart
    def getupConfigStartTag(self):
        return self.upConfigStart
    def getheelStartTag(self):
        return self.heelStart
    def getTestLabStartTag(self):
        return self.testLabStart
    def getIxferTStartTag(self):
        return self.ixferTStart
    def getBxferTStartTag(self):
        return self.bxferTStart
    def getUsbStartTag(self):
        return self.usbStart
    def getSyncStartTag(self):
        return self.syncStart
    def getGBeepStartTag(self):
        return self.gBeepStart
    def getBBeepStartTag(self):
        return self.bBeepStart
    def getPingFreqStartTag(self):
        return self.pingFreqStart
    def getPingAttStartaTag(self):
        return self.pingAttStart
    def getMetroVersionStartTag(self):
        return self.metroVersionStart
    def getMetroVersionEndTag(self):
        return self.metroVersionEnd
    def getFractionalPriceStartTag(self):
        return self.fractionalPriceStart
    def getFractionalPriceEndTag(self):
        return self.fractionalPriceEnd
    def getPoundsForStartTag(self):
        return self.poundsForStart
    def getPoundsForEndTag(self):
        return self.poundsForEnd

class hardwareTags:
    def __init__(self):
        self.sectionStart = '<ScaleHWSection>'
        self.primaryDisplayStart = '<PriDspType>'
        self.primaryDisplayEnd = '</PriDspType>'
        self.primaryDisplaySizeStart = '<PriDspSize>'
        self.primaryDisplaySizeEnd = '</PriDspSize>'
        self.primaryDisplayInterfaceStart = '<PriInter>'
        self.primaryDisplayInterfaceEnd = '</PriInter>'

        self.secondaryDisplayStart = '<SecDspType>'
        self.secondaryDisplayEnd = '</SecDspType>'
        self.secondaryDisplaySizeStart = '<SecDspSize>'
        self.secondaryDisplaySizeEnd = '</SecDspSize>'
        self.secondaryDisplayInterfaceStart = '<SecInter>'
        self.secondaryDisplayInterfaceEnd = '</SecInter>'

        self.wifiStart = '<WifiIns>'
        self.wifiEnd = '</WifiIns>'
        self.sataStart = '<SataIns>'
        self.sataEnd = '</SataIns>'
        self.bluetoothStart = '<BlueIns>'
        self.bluetoothEnd = '</BlueIns>'
        self.powerHStart = '<PowerH>'
        self.powerHEnd = '</PowerH>'
        self.pcieStart = '<Pcie>'
        self.pcieEnd = '</Pcie>'
        self.domStart = '<DomIns>'
        self.domEnd = '</DomIns>'
        self.rfidStart = '<RfidIns>'
        self.rfidEnd = '</RfidIns>'
        self.sectionEnd = '</ScaleHWSection>'
        
    def getSectionStartTag(self):
        return self.sectionStart
    def getPrimaryDisplayStartTag(self):
        return self.primaryDisplayStart
    def getPrimaryDisplaySizeStartTag(self):
        return self.primaryDisplaySizeStart
    def getPrimaryDisplayInterfaceStartTag(self):
        return self.primaryDisplayInterfaceStart
    def getSecondaryDisplayStartTag(self):
        return self.secondaryDisplayStart
    def getSecondaryDisplaySizeStartTag(self):
        return self.secondaryDisplaySizeStart
    def getSecondaryInterfaceStartTag(self):
        return self.secondaryDisplayInterfaceStart
    def getWifiStartTag(self):
        return self.wifiStart
    def getSataStartTag(self):
        return self.sataStart
    def getBluetoothStartTag(self):
        return self.bluetoothStart
    def getPowerHStartTag(self):
        return self.powerHStart
    def getPcieStartTag(self):
        return self.pcieStart
    def getDomStartTag(self):
        return self.domStart
    def getRfidStartTag(self):
        return self.rfidStart
        
    def getSectionEndTag(self):
        return self.sectionEnd
    def getPrimaryDisplayEndTag(self):
        return self.primaryDisplayEnd
    def getPrimaryDisplaySizeEndTag(self):
        return self.primaryDisplaySizeEnd
    def getPrimaryDisplayInterfaceEndTag(self):
        return self.primaryDisplayInterfaceEnd
    def getSecondaryDisplayEndTag(self):
        return self.secondaryDisplayEnd
    def getSecondaryDisplaySizeEndTag(self):
        return self.secondaryDisplaySizeEnd
    def getSecondaryInterfaceEndTag(self):
        return self.secondaryDisplayInterfaceEnd
    def getWifiEndTag(self):
        return self.wifiEnd
    def getSataEndTag(self):
        return self.sataEnd
    def getBluetoothEndTag(self):
        return self.bluetoothEnd
    def getPowerHEndTag(self):
        return self.powerHEnd
    def getPcieEndTag(self):
        return self.pcieEnd
    def getDomEndTag(self):
        return self.domEnd
    def getRfidEndTag(self):
        return self.rfidEnd

