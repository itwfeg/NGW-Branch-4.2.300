# OpenJDK install script
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/java_7_jre

import os

# Must add reference to python tree so import such as 'import shellCommand' will work
import sys
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import shellCommand

featureName = "java_7_jre"

# determine if update-rc.sh file exists and if not
# create file
if not os.path.exists(os.path.join("/","usr","sbin","update-rc.d")):
    os.system("/bin/cp -f update-rc.d %s" % (os.path.join("/","usr", "sbin", "update-rc.d")))
    os.system("/bin/chmod ugo+rx %s" % (os.path.join("/", "usr", "sbin", "update-rc.d")))

if not os.path.exists(os.path.join("/","var","dpkg","status")):
    os.system("/bin/touch %s" % (os.path.join("/","var", "dpkg", "status")))
    os.system("/usr/bin/dpkg --clear-avail")


# openjdk
os.system("/usr/bin/dpkg --unpack java-common_0.47_all.deb")
os.system("/usr/bin/dpkg --unpack ca-certificates_20130119+deb7u1_all.deb")
os.system("/usr/bin/dpkg --unpack ca-certificates-java_20121112+nmu2_all.deb")
os.system("/usr/bin/dpkg --unpack libatk-wrapper-java_0.30.4-3_all.deb")
os.system("/usr/bin/dpkg --unpack --ignore-depends=multiarch-support libatk-wrapper-java-jni_0.30.4-3_armel.deb")
os.system("/usr/bin/dpkg --unpack openjdk-7-jre-lib_7u65-2.5.1-5~deb7u1_all.deb")
os.system("/usr/bin/dpkg --unpack openjdk-7-jre_7u65-2.5.1-5~deb7u1_armel.deb")
os.system("/usr/bin/dpkg --unpack --ignore-depends=multiarch-support openjdk-7-jre-headless_7u65-2.5.1-5~deb7u1_armel.deb")

os.system("/bin/ln -s /usr/lib/jvm/java-7-openjdk-armel/jre/bin/java /usr/bin/java")
os.system("/bin/ln -s /usr/lib/jvm/java-7-openjdk-armel/jre/bin/javaws /usr/bin/javaws")
os.system("/bin/ln -s /usr/lib/jvm/java-7-openjdk-armel/jre/bin/policytool /usr/bin/policytool")


# iced tea plugin
os.system("/usr/bin/dpkg --unpack icedtea-netx-common_1.4-3~deb7u2_all.deb")
os.system("/usr/bin/dpkg --unpack icedtea-netx_1.4-3~deb7u2_armel.deb")
os.system("/usr/bin/dpkg --unpack icedtea-7-plugin_1.4-3~deb7u2_armel.deb")

if not os.path.exists(os.path.join("/","root",".mozilla")):
    os.system("/bin/mkdir %s" % (os.path.join("/","root",".mozilla")))

if not os.path.exists(os.path.join("/","root",".mozilla","plugins")):
    os.system("/bin/mkdir %s" % (os.path.join("/","root",".mozilla","plugins")))

os.system("/bin/ln -s /usr/lib/jvm/java-7-openjdk-armel/jre/lib/arm/IcedTeaPlugin.so /root/.mozilla/plugins/IcedTeaPlugin.so")


# configuration
# I found all these scripts using "find / -name *.dpkg-new"
os.system("/bin/mv /etc/java-7-openjdk/psfontj2d.properties.dpkg-new /etc/java-7-openjdk/psfontj2d.properties")
os.system("/bin/mv /etc/java-7-openjdk/management/management.properties.dpkg-new /etc/java-7-openjdk/management/management.properties")
os.system("/bin/mv /etc/java-7-openjdk/management/jmxremote.access.dpkg-new /etc/java-7-openjdk/management/jmxremote.access")
os.system("/bin/mv /etc/java-7-openjdk/management/snmp.acl.dpkg-new /etc/java-7-openjdk/management/snmp.acl")
os.system("/bin/mv /etc/java-7-openjdk/management/jmxremote.password.dpkg-new /etc/java-7-openjdk/management/jmxremote.password")
os.system("/bin/mv /etc/java-7-openjdk/security/java.policy.dpkg-new /etc/java-7-openjdk/security/java.policy")
os.system("/bin/mv /etc/java-7-openjdk/security/nss.cfg.dpkg-new /etc/java-7-openjdk/security/nss.cfg")
os.system("/bin/mv /etc/java-7-openjdk/security/java.security.dpkg-new /etc/java-7-openjdk/security/java.security")
os.system("/bin/mv /etc/java-7-openjdk/psfont.properties.ja.dpkg-new /etc/java-7-openjdk/psfont.properties")
os.system("/bin/mv /etc/java-7-openjdk/tz.properties.dpkg-new /etc/java-7-openjdk/tz.properties")
os.system("/bin/mv /etc/java-7-openjdk/images/cursors/cursors.properties.dpkg-new /etc/java-7-openjdk/images/cursors/cursors.properties")
os.system("/bin/mv /etc/java-7-openjdk/swing.properties.dpkg-new /etc/java-7-openjdk/swing.properties")
os.system("/bin/mv /etc/java-7-openjdk/accessibility.properties.dpkg-new /etc/java-7-openjdk/accessibility.properties")
os.system("/bin/mv /etc/java-7-openjdk/sound.properties.dpkg-new /etc/java-7-openjdk/sound.properties")
os.system("/bin/mv /etc/java-7-openjdk/calendars.properties.dpkg-new /etc/java-7-openjdk/calendars.properties")
os.system("/bin/mv /etc/java-7-openjdk/content-types.properties.dpkg-new /etc/java-7-openjdk/content-types.properties")
os.system("/bin/mv /etc/java-7-openjdk/fontconfig.properties.dpkg-new /etc/java-7-openjdk/fontconfig.properties")
os.system("/bin/mv /etc/java-7-openjdk/jvm-armel.cfg.dpkg-new /etc/java-7-openjdk/jvm-armel.cfg")
os.system("/bin/mv /etc/java-7-openjdk/logging.properties.dpkg-new /etc/java-7-openjdk/logging.properties")
os.system("/bin/mv /etc/java-7-openjdk/net.properties.dpkg-new /etc/java-7-openjdk/net.properties")
os.system("/bin/mv /etc/java-7-openjdk/flavormap.properties.dpkg-new /etc/java-7-openjdk/flavormap.properties")
os.system("/bin/mv /etc/icedtea-web/javaws.policy.dpkg-new /etc/icedtea-web/javaws.policy")
os.system("/bin/mv /etc/default/cacerts.dpkg-new /etc/default/cacerts")
os.system("/bin/mv /etc/ca-certificates/update.d/jks-keystore.dpkg-new /etc/ca-certificates/update.d/jks-keystore")


# TODO:
# Need to check all exitCodes and reflect status below
sys.exit(0)
