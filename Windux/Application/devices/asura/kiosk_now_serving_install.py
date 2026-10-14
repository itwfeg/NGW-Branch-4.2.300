import sys
import installUtils
import os

import subprocess

featureName = "kiosk_now_serving"
featureFilename = "%s.tgz" % (featureName)
KIOSK_NOW_SERVING_VERSION="2.2.0"

# install pycrypto
subprocess.call(['tar', 'xvzf', 'pycrypto-2.6.1-asura-armle.tar.gz', '-C', '/'])

# patch wifi script to support static ip addresses
subprocess.call(['cp', '/etc/init.d/wifi', '/etc/init.d/wifi.orig'])

subprocess.call(['cp', 'wifi', '/etc/init.d'])
subprocess.call(['chmod', 'a+x', '/etc/init.d/wifi'])

# install feature
exitCode = 0

exitCode |= installUtils.setPythonRoot()

nowServingConfig = installUtils.retainFile(os.path.join("/", "extension", "apps", "hobart", "nowServing.config"))
applicationsConfig = installUtils.retainFile(os.path.join("/", "extension", "apps", "hobart", "applications.conf"))
storeLogo = installUtils.retainFile(os.path.join("/", "extension", "apps", "hobart", "res", "storeLogo.png"))
backgroundImage = installUtils.retainFile(os.path.join("/", "extension", "apps", "hobart", "res", "background.png"))
warningIcon = installUtils.retainFile(os.path.join("/", "extension", "apps", "hobart", "res", "warningIcon.png"))

exitCode |= nowServingConfig.backup()
exitCode |= applicationsConfig.backup()
exitCode |= storeLogo.backup()
exitCode |= backgroundImage.backup()
exitCode |= warningIcon.backup()

exitCode |= installUtils.extractTarball(featureFilename)

exitCode |= nowServingConfig.restore()
exitCode |= applicationsConfig.restore()
exitCode |= storeLogo.restore()
exitCode |= backgroundImage.restore()
exitCode |= warningIcon.restore()

exitCode |= installUtils.addInitdScript('microSmBackend')
exitCode |= installUtils.addInitdScript('autoconfig')

exitCode |= installUtils.addFeatureToInstalledList(featureName, KIOSK_NOW_SERVING_VERSION)

# exitCode |= installUtils.removeInstallerFiles(featureName)

# stop the backend programmatically so socket closed out
exitCode |= installUtils.sendMessage('localhost', 5076, "stop|0")

exitCode |= installUtils.reboot(sync=True)

sys.exit(exitCode)
