#!/usr/bin/env python
# -*- coding: utf-8 -*-

# adservice app install script
# This feature installs the adservice app
#
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/adservice

import os
import sys
import tarfile
import glob
import shutil


class InstallFeature:

    def __init__(self):
        self.feature_name = "adservice"
        self.feature_version = "1.0.0"
        self.feature_filename = "hti-linux-feature-%s-%s.tgz" % (self.feature_name, self.feature_version)
        self.installed_feature_name = "SCALE_AD_SERVICE"
        self.apps_folder = os.path.join("/", "usr", "local", "hobart", "apps")

        self.install_location = os.path.join(self.apps_folder, self.feature_name)

        self.wildcard_exclusions = []

    def filter_files(self, tarinfo):

        if tarinfo.name in self.wildcard_exclusions:
            return None
        else:
            tarinfo.uid = 0
            tarinfo.gid = 0
            tarinfo.uname = 'root'
            tarinfo.gname = 'root'
            return tarinfo

    def build_installer_tarball(self):

        os.chdir("..")

        self.wildcard_exclusions = glob.glob('./filebox*')
        self.wildcard_exclusions += glob.glob('./*.pyc')
        self.wildcard_exclusions += glob.glob('./feature*')
        self.wildcard_exclusions += ['./.idea', './filebox_store', './pip', './requests', './schedule']

        if os.path.exists(os.path.join(".", "feature", self.feature_filename)):
            os.remove(os.path.join(".", "feature", self.feature_filename))

        with tarfile.open(os.path.join(".", "feature", self.feature_filename), 'w:gz') as feature_tarball:
            feature_tarball.add(".", filter=self.filter_files, recursive=True)

    def install(self):
        # Must add reference to python tree so import such as 'import shellCommand' will work
        sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

        import installUtils

        if not os.path.exists(self.install_location):
            os.mkdir(self.install_location, 0755)

        status_code = installUtils.extractTarballAsIs(featureFilename=self.feature_filename,
                                                      root=self.install_location)

        # put ad_service apps config file in apps folder
        shutil.copy2(os.path.join(self.apps_folder, "adservice", "ad_service.conf"),
                     self.apps_folder)

        # update feature
        installUtils.addFeatureToInstalledList(self.installed_feature_name, self.feature_version)

        # TODO:
        # Need to check all exitCodes and reflect status below
        sys.exit(0)

    def install_license(self):

        # Must add reference to python tree so import such as 'import shellCommand' will work
        sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

        import installUtils

        # update feature
        installUtils.addFeatureToInstalledList(self.installed_feature_name, self.feature_version)


if __name__ == '__main__':
    feature_installer = InstallFeature()
    feature_installer.install()
