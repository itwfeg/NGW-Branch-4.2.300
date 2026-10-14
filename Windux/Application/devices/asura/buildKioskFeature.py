import buildFeature
import os
import osUtils
import tarfile
import sys

def buildKioskNowServing(containerName, relTag):

    scaleManagementRoot = os.path.join("..", "..", "..", "..", "..", "ScaleManagement")
    buildFeature.cleanBuildDirectory()

    featurePath = "extension/apps/hobart"

    archive = tarfile.open(containerName, 'w:gz')
    buildFeature.setApplicationSoftwareVersionInfo(relTag)
    buildFeature.addFileToArchive('SOFTWARE_VERSION_INFO', '/', archive)
    buildFeature.addFileToArchive('applications.conf', featurePath, archive)
    buildFeature.addFileToArchive('icon.png', featurePath, archive)
    buildFeature.addFileToArchive('nowServing', featurePath, archive)
    buildFeature.addFileToArchive('microSmBackend', "etc/init.d", archive)
    buildFeature.addFileToArchive('autoconfig', "etc/init.d", archive)
    buildFeature.addFileToArchive(os.path.join("..", 'nowServing.config'), featurePath, archive)
    buildFeature.addDirectoryToArchive(os.path.join("..","res"), featurePath, archive)
    buildFeature.addEmptyDirectoryToArchive("usr/local/hobart/images", archive)
    buildFeature.addEmptyDirectoryToArchive("usr/local/hobart/upgrade", archive)

    featurePath = "usr/local/hobart/python"

    winduxRoot = os.path.join("..", "..", "..", "..", "..", "Windux")

    status, details = osUtils.assignVariableInFile(os.path.join(scaleManagementRoot, "smBackend", "microSmBackend.py"),
        '^MICRO_SM_BACKEND_VARIANT\s*=\s*\"[a-z]+\"$',
        'MICRO_SM_BACKEND_VARIANT="starasura"')

    status, data = buildFeature.updateFeatureVersion(relTag, os.path.join(winduxRoot, "Application", "customerInterface", "nowServing", "asura", "kiosk_now_serving_install.py"), "kiosk_now_serving")

    status, data = buildFeature.updateFeatureVersion(relTag, os.path.join(scaleManagementRoot, "smBackend", "microSmBackend.py"), "micro_sm_backend")

    moduleLookup = buildFeature.insertModulesIntoArchive(os.path.join(scaleManagementRoot, 'smBackend', 'microSmBackend.py'), archive, featurePath, checkLookup=[])
    moduleLookup = buildFeature.insertModulesIntoArchive(os.path.join(scaleManagementRoot, 'smBackend', 'autoconfig.py'), archive, featurePath, checkLookup=[])

    archive.close()

if __name__ == '__main__':

    featureName = 'kiosk_now_serving'
    featureFilename = '%s.tgz' % (featureName)
    featureInstallerFilename = '%s_install.py' % (featureName)
    device = 'ptrhtque'
    osSupported = 'linux'

    buildFeature.cleanBuildDirectory()
    args = buildFeature.commandLineRelTag()

    buildKioskNowServing(featureFilename, args.reltag)
    
    additionalFiles = []
    additionalFiles.append('pycrypto-2.6.1-asura-armle.tar.gz')
    additionalFiles.append('wifi')
    
    if buildFeature.buildFeature(args.reltag, featureName, featureFilename, featureInstallerFilename, device, osSupported, additionalFiles) == 'success':
        sys.exit(0)
    else:
        sys.exit(1)
