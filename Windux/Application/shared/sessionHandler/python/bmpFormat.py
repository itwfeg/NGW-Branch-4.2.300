# -*- coding: utf-8 -*-

import sys
import os
import struct
import binascii
import base64

class bmpHeader:
    def __init__(self,  filename): 
        self.identity= ""
        self.fileSize = 0
        self.offset  = 0
        self.dibHeaderSize = 0
        self.header = ""
        self.bmpWidth = 0
        self.bmpHeight = 0
        self.numColorPlanes = 0
        self.numPixelsPerBit = 0
        self.compressMethod  = ""
        self.horizResolution = 0
        self.vertResolution = 0
        self.imageSize = 0


        compressiontypes = {0: "BI_RGB", 1: "BI_RLE8", 2: "BI_RLE4", 3: "BI_BITFIELDS", 4: "BI_JPEG", 5: "BI_PNG"}
        if os.path.exists(filename):
            # For Python 3, needed to specify opening the file in binary mode,
            # where we didn't need to for Python 2
            f = open(filename, 'rb')
        else:
            print(("File '%s' does not exist." % filename))
            return
        
        #File header
        self.identity= f.read(2)
        
        size = f.read(4)
        self.fileSize = struct.unpack("<i", size)[0]
        
        self.reserved1 = f.read(2)
        self.reserved2=  f.read(2)
        
        offset =  f.read(4) 
        self.offset = struct.unpack("<i", offset)[0]

        #DIB header
        headLen = f.read(4)
        self.dibHeaderSize = struct.unpack("<i", headLen)[0]
        
        if self.dibHeaderSize == 40:
            self.header = "BITMAPINFOHEADER"

            width = f.read(4) 
            self.bmpWidth = abs(struct.unpack("<i", width)[0])

            height = f.read(4) 
            self.bmpHeight = abs(struct.unpack("<i", height)[0])

            color = f.read(2) 
            self.numColorPlanes = struct.unpack("<i", color + b"\x00\x00")[0]

            bpp = f.read(2) 
            self.numPixelsPerBit = struct.unpack("<i", bpp + b"\x00\x00")[0]

            compress = f.read(4) 
            self.compressMethod = compressiontypes[struct.unpack("<i", compress)[0]]
            
            imageSize= f.read(4)
            self.imageSize = abs(struct.unpack("<i", imageSize)[0])   
            if self.imageSize == 0:
                self.imageSize = self.fileSize -  self.offset
            

            if self.compressMethod != "BI_RGB":
                print("Not an uncompressed bitmap.")

            res = f.read(4) 
            self.horizResolution = struct.unpack("<i", res)[0]
            
            res = f.read(4) 
            self.vertResolution = struct.unpack("<i", res)[0]

            numColors = f.read(4) 
            self.numColors  = struct.unpack("<i", numColors)[0]

            numColorsImport = f.read(4) 
            self.numColorsImport = struct.unpack("<i", numColorsImport)[0]
            
            #determine where the bitmap starts
            bytesRead = 54      #58
            delta = self.offset - bytesRead
            junk = f.read(delta)
            
        elif self.dibHeaderSize == 12:
            self.header = "BITMAPCOREHEADER"

            width = f.read(2) 
            self.bmpWidth = abs(struct.unpack("<i", width + b"\x00\x00"))
            
            height = f.read(2) 
            self.bmpHeight= abs(struct.unpack("<i", height + b"\x00\x00"))

            color = f.read(2) 
            self.numColorPlanes = struct.unpack("<i", color + b"\x00\x00")

            bpp = f.read(2) 
            self.numPixelsPerBit = struct.unpack("<i", bpp + b"\x00\x00")

            self.compressMethod = "BI_RGB"
            self.horizResolution = None
            self.vertResolution = None
            self.numColors = None
            self.numColorsImport = None
        
        elif self.dibHeaderSize == 124:
            self.header = "BITMAPV5HEADER"
            #just read what we care about 
            width = f.read(4) 
            self.bmpWidth = abs(struct.unpack("<i", width)[0])

            height = f.read(4) 
            self.bmpHeight = abs(struct.unpack("<i", height)[0])

            color = f.read(2) 
            self.numColorPlanes = struct.unpack("<i", color + b"\x00\x00")[0]

            bpp = f.read(2) 
            self.numPixelsPerBit = struct.unpack("<i", bpp + b"\x00\x00")[0]

            compress = f.read(4) 
            self.compressMethod = compressiontypes[struct.unpack("<i", compress)[0]]

            if self.compressMethod != "BI_RGB":
                print("Not an uncompressed bitmap.")

            imageSize= f.read(4)
            self.imageSize = abs(struct.unpack("<i", imageSize)[0])   
            
            res = f.read(4) 
            self.horizResolution = struct.unpack("<i", res)[0]
            
            res = f.read(4) 
            self.vertResolution = struct.unpack("<i", res)[0]

            numColors = f.read(4) 
            self.numColors  = struct.unpack("<i", numColors)[0]

            numColorsImport = f.read(4) 
            self.numColorsImport = struct.unpack("<i", numColorsImport)[0]
            #we dont care about the rest of the header
            junk = f.read(92)
        else:
            print("Not a supported bitmap format.")

        self.bitMap = f.read()
        f.close()        
        
    def getBitmap(self):
        return self.bitMap
    def getBitMapLength(self):
        return len(self.bitMap)
    def getImageSize(self):
        return self.imageSize
    def getBitsPerPixel(self):
        return self.numPixelsPerBit
    def getBitMapXY(self):
        return self.bmpHeight * self.bmpWidth
    def getBitMapWidth(self):
        return self.bmpWidth
        
        
        
        
