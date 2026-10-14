# OpenJDK install script
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/java_jre

import os
import sys

# Must add reference to python tree so import such as 'import shellCommand' will work
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import installUtils
import shellCommand

featureName = "java_jre"
featureVersion = "7.75"

# multiarch-support
os.system( "/usr/bin/dpkg -i --ignore-depends=libc6 multiarch-support_2.13-38\+deb7u8_armhf.deb" )

# debconf
os.system( "/usr/bin/dpkg -i --ignore-depends=perl-base debconf_1.5.49_all.deb" )

# openjdk w/ iced-tea

ignoreDepends = [
    'libasound2',
    'libatk1.0-0',
    'libc6',
    'libcairo2',
    'libcups2',
    'libfontconfig1',
    'libfreetype6',
    'libgcc1',
    'libgdk-pixbuf2.0-0',
    'libgif4',
    'libgl1',
    'libglib2.0-0',
    'libgtk2.0-0',
    'libjpeg8',
    'libnss3',
    'libpango1.0-0',
    'libpng12-0',
    'libpulse0',
    'libstdc\+\+6',
    'libsqlite3-0',
    'libx11-6',
    'libxext6',
    'libxi6',
    'libxinerama1',
    'libxrandr2',
    'libxrender1',
    'libxtst6',
    'openssl',
    'tzdata',
    'zlib1g'
]

packages = [
    'ca-certificates_20130119\+deb7u1_all.deb',
    'ca-certificates-java_20121112\+nmu2_all.deb',
    'icedtea-7-plugin_1.4-3~deb7u2_armhf.deb',
    'icedtea-netx_1.4-3~deb7u2_armhf.deb',
    'icedtea-netx-common_1.4-3~deb7u2_all.deb',
    'java-common_0.47_all.deb',
    'libatk-wrapper-java_0.30.4-3_all.deb',
    'libatk-wrapper-java-jni_0.30.4-3_armhf.deb',
    'libcomerr2_1.42.5-1.1\+deb7u1_armhf.deb',
    'libk5crypto3_1.10.1\+dfsg-5\+deb7u3_armhf.deb',
    'libkeyutils1_1.5.5-3\+deb7u1_armhf.deb',
    'libkrb5support0_1.10.1\+dfsg-5\+deb7u3_armhf.deb',
    'libkrb5-3_1.10.1\+dfsg-5\+deb7u3_armhf.deb',
    'liblcms2-2_2.6-3\+b3_armhf.deb',
    'libpcsclite1_1.8.4-1\+deb7u1_armhf.deb',
    'openjdk-7-jre_7u75-2.5.4-1~deb7u1_armhf.deb',
    'openjdk-7-jre-headless_7u75-2.5.4-1~deb7u1_armhf.deb',
    'openjdk-7-jre-lib_7u75-2.5.4-1~deb7u1_all.deb',
    'tzdata-java_2014j-0wheezy1_all.deb'
]

os.system( "/usr/bin/dpkg -i --ignore-depends={0} {1}".format( ",".join( ignoreDepends ), " ".join( packages ) ) )

os.system( "/bin/rm /opt/java" )
os.system( "/bin/ln -s /usr/lib/jvm/java-7-openjdk-armhf/jre/ /opt/java" )

# there is an issue when JAVA goes to load NSS it cannot find it (even though
# it sits in /usr/lib) so here we configure it explicitly
os.system( "/bin/cp nss.cfg /etc/java-7-openjdk/security" )

# install plugin
os.system( "/bin/mkdir -p /root/.mozilla/plugins" )

os.system( "/bin/rm /root/.mozilla/plugins/IcedTeaPlugin.so" )
os.system( "/bin/ln -s /opt/java/lib/IcedTeaPlugin.so /root/.mozilla/plugins/IcedTeaPlugin.so" )

# update feature
installUtils.addFeatureToInstalledList(featureName, featureVersion)

# TODO:
# Need to check all exitCodes and reflect status below
sys.exit(0)
