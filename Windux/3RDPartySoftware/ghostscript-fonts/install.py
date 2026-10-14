# PDF Viewer install script
# Assumes that script is running from current directory

import os
import sys
import tarfile

# Must add reference to python tree so import such as 'import shellCommand' will work
if os.name == 'posix':
    sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))
elif os.name == 'nt':
    sys.path.append('c:\\Program Files\\hobart\\python')

import installUtils

featureName = "pdf_viewer"
featureVersion = "8.11"

cwd = os.getcwd()

tar = tarfile.open('ghostscript-fonts-std-8.11.tar.gz', 'r')
tar.extractall(os.getcwd())

# update feature
installUtils.addFeatureToInstalledList(featureName, featureVersion)
