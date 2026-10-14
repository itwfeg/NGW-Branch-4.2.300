import os
import argparse
import sys
import string
import csv
import tempfile

# system base path of images
if os.name == 'posix':
    systemDevicePath = os.path.join( os.sep, "usr", "local", "hobart" )
else:
    systemDevicePath = os.path.join( "C:", "Program Files", "Hobart" )

def convertSize( filename, defaultSize ):
    hints = os.path.join( tempfile.gettempdir(), 'thumbsizes' )

    if os.path.exists( hints ):
        with open( hints ) as csv_file:
            csv_reader = csv.reader( csv_file, delimiter=',' )

            for row in csv_reader:
                if filename == row[0]:
                    if '0' != row[1] and '0' != row[2]:
                        thumbSize = "-resize " + row[1] + "x" + row[2] + "!" #resize with force of aspect ratio
                    elif '0' != row[1]:
                        thumbSize = "-thumbnail " + row[1]
                    elif '0' != row[2]:
                        thumbSize = "-thumbnail x" + row[2]

                    print( 'found size hint ' + thumbSize )
                    return thumbSize

    return "-thumbnail " + defaultSize

def generateThumbnails( htPath = systemDevicePath, defaultSize = 'x64' ):

    try:
        # make list of all folders that we want to convert
        dirTreeItems = []
        dirTreeItems.append( os.path.join( "images", "categories" ) )
        dirTreeItems.append( os.path.join( "images", "products" ) )
        dirTreeItems.append( os.path.join( "images", "selfServe" ) )
        dirTreeItems.append( os.path.join( "images", "softkeys" ) )
        dirTreeItems.append( os.path.join( "images", "specials" ) )

        dirTreeItems.append( os.path.join( "user", "graphics" ) )

        # as per Jim Highley, do not create thumbs for system folders... these will be generated manually and added to the build.
        #dirTreeItems.append( os.path.join( "images", "system", "categories" ) )
        #dirTreeItems.append( os.path.join( "images", "system", "products" ) )
        #dirTreeItems.append( os.path.join( "images", "system", "selfServe" ) )
        #dirTreeItems.append( os.path.join( "images", "system", "softkeys" ) )
        #dirTreeItems.append( os.path.join( "images", "system", "specials" ) )

        #dirTreeItems.append( os.path.join( "system", "flashkeygfx" ) )
        #dirTreeItems.append( os.path.join( "system", "graphics" ) )

        # make list of all file type extensions that we want to convert
        fileTypes = []
        fileTypes.append( ".bmp" )
        fileTypes.append( ".gif" )
        fileTypes.append( ".jpg" )
        fileTypes.append( ".jpeg" )
        fileTypes.append( ".png" )

        if os.name == 'posix':
            fileTypes.append( ".BMP" )
            fileTypes.append( ".GIF" )
            fileTypes.append( ".JPG" )
            fileTypes.append( ".JPEG" )
            fileTypes.append( ".PNG" )

        # iterate over folders
        for curTreeItem in dirTreeItems:
            p = os.path.join( htPath, curTreeItem )

            if os.path.exists( p ):

                # iterate over files in each folder
                for filename in os.listdir( p ):
                    for curFileType in fileTypes:
                        thumbType = ".thumb" + curFileType

                        # check file extension is something we want to convert and it is not a thumbnail already
                        if filename.endswith( curFileType ) and not filename.endswith( thumbType ):

                            img = os.path.join( p, filename )
                            imgThumb = img[:-len(curFileType)] + thumbType

                            # convert
                            if os.name == 'posix':
                                #print( img )
                                #print( imgThumb )

                                # check to see if we have a size hint for this image
                                size = convertSize( os.path.join( curTreeItem, filename ), defaultSize )

                                # generate thumbnail
                                cmd = "convert " + size + " '" + img + "' '" + imgThumb + "'"
                                print( cmd )

                                os.system( cmd )

    except:
        print( 'exception while converting images!' )
        return 1

    return 0

if __name__ == "__main__":
    exitCode = generateThumbnails()
    sys.exit( exitCode )
