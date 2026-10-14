import sys
import installUtils
import os

import subprocess

featureName = "digital_signage"
featureFilename = "%s.tgz" % (featureName)
DIGITAL_SIGNAGE_VERSION="2.2.0"

# install pycrypto
subprocess.call(['tar', 'xf', 'pycrypto-2.6.1.tar.gz'])

os.chdir('pycrypto-2.6.1')
subprocess.call(['python', 'setup.py', 'build'])
subprocess.call(['python', 'setup.py', 'install'])
os.chdir('..')

# install feature
exitCode = 0

exitCode |= installUtils.setPythonRoot()

exitCode |= installUtils.extractTarball(featureFilename)

commandLine = 'cd %s; %s/nowServing -tv &' % (os.path.join(os.sep, "usr", "local", "hobart", "nowServing" ),
                                              os.path.join(os.sep, "usr", "local", "hobart", "nowServing" ))

exitCode != installUtils.addDebianApplicationStartup(commandLine, 'debian')

exitCode |= installUtils.addInitdScript('microSmBackend')
exitCode |= installUtils.addInitdScript('autoconfig')

exitCode |= installUtils.addFeatureToInstalledList(featureName, DIGITAL_SIGNAGE_VERSION)

# exitCode |= installUtils.removeInstallerFiles(featureName)

# stop the backend programmatically so socket closed out
exitCode |= installUtils.sendMessage('localhost', 5076, "stop|0")

exitCode |= installUtils.reboot(sync=True)

sys.exit(exitCode)
