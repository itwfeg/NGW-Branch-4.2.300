#!/bin/bash

#PACKAGE=hostap
PACKAGE=wpa_supplicant-0.7.3
VERSION=0.7.3-9

# remove output folder
if [ -d debian ];
then
	rm -rf debian
fi

###############################################################################
#
# compile wpa_supplicant
#
###############################################################################

# remove existing folder
if [ -d ${PACKAGE} ];
then
	rm -rf ${PACKAGE}
fi

# perform checkout
#git clone git://w1.fi/srv/git/hostap.git
tar -zxvf ${PACKAGE}.tar.gz

if [ ! -d ${PACKAGE} ];
then
	echo "failed to extract ${PACKAGE}!"
	exit -1
fi

# configure and build
cd ${PACKAGE}

# distro specific customization and not suitable for upstream,
# works around busted drivers
patch -p1 < ../wpa_supplicant-assoc-timeout.patch
# ensures that debug output gets flushed immediately to help diagnose driver
# bugs, not suitable for upstream
patch -p1 < ../wpa_supplicant-flush-debug-output.patch
# disto specific customization for log paths, not suitable for upstream
patch -p1 < ../wpa_supplicant-dbus-service-file-args.patch
# quiet an annoying and frequent syslog message
patch -p1 < ../wpa_supplicant-quiet-scan-results-message.patch
# recover from streams of driver disconnect messages (iwl3945)
patch -p1 < ../wpa_supplicant-squelch-driver-disconnect-spam.patch
# allow more private key encryption algorithms
patch -p1 < ../wpa_supplicant-openssl-more-algs.patch
# Send PropertyChanged notificationes when the BSS list changes
patch -p1 < ../wpa_supplicant-bss-changed-prop-notify.patch
# Don't crash trying to pass NULL to dbus
patch -p1 < ../wpa_supplicant-dbus-null-error.patch
# Fix signal quality reporting for some drivers when using nl80211
patch -p1 < ../rh752032-0001-nl80211-Fix-UNSPEC-signal-quality-reporting.patch

# Session Management Entity (SME) patches for nl80211 driver
patch -p1 < ../rh713280-0001-sme-fix-retry-after-auth-assoc-timeout-failure.patch
patch -p1 < ../rh713280-0002-sme-optimize-recovery-from-common-load-balancing-mechanisms.patch
patch -p1 < ../rh713280-0003-sme-blacklist-bss-on-first-failure-if-only-a-single-network-is-enabled.patch
patch -p1 < ../rh713280-0004-sme-extend-load-balancing-optimization-in-bss-blacklisting.patch
patch -p1 < ../rh713280-0005-sme-optimize-recovery-from-association-command-failures.patch
patch -p1 < ../rh713280-0006-sme-add-timers-for-authentication-and-association.patch
patch -p1 < ../rh713280-0007-sme-nl80211-set-cipher-suites.patch

# Fixes for Opportunistic Key Caching (OKC)
patch -p1 < ../rh813579-0001-pmkokc-Set-portValid-TRUE-on-association-for-driver-based-4.patch
patch -p1 < ../rh813579-0002-pmkokc-Clear-WPA-and-EAPOL-state-machine-config-pointer-on-.patch
patch -p1 < ../rh813579-0003-pmkokc-Clear-driver-PMKSA-cache-entry-on-PMKSA-cache-expira.patch
patch -p1 < ../rh813579-0004-pmkokc-Flush-PMKSA-cache-entries-and-invalidate-EAP-state-o.patch
patch -p1 < ../rh813579-0005-pmkokc-Fix-proactive_key_caching-configuration-to-WPA-code.patch
patch -p1 < ../rh813579-0006-pmkokc-RSN-Add-a-debug-message-when-considing-addition-of-O.patch
patch -p1 < ../rh813579-0007-pmkokc-Clear-OKC-based-PMKSA-caching-entries-if-PMK-is-chan.patch
patch -p1 < ../rh813579-0008-pmkokc-Move-wpa_sm_remove_pmkid-call-to-PMKSA-cache-entry-f.patch
patch -p1 < ../rh813579-0009-pmkokc-Use-PMKSA-cache-entries-with-only-a-single-network-c.patch
patch -p1 < ../rh813579-0010-pmkokc-PMKSA-Do-not-evict-active-cache-entry-when-adding-ne.patch
patch -p1 < ../rh813579-0011-pmkokc-PMKSA-Set-cur_pmksa-pointer-during-initial-associati.patch
patch -p1 < ../rh813579-0012-pmkokc-PMKSA-make-deauthentication-due-to-cache-entry-remov.patch
patch -p1 < ../rh813579-0013-pmkokc-PMKSA-update-current-cache-entry-due-to-association-.patch

patch -p1 < ../rh837402-less-aggressive-roaming.patch

# Fixes for CVE-2015-0210 (wpa_supplicant: broken certificate subject check)
patch -p1 < ../rh1186806-0001-dbus_server_cert_info.patch
patch -p1 < ../rh1186806-0002-move_calls_to_notify.patch
patch -p1 < ../rh1186806-0003-write_server_cert.patch
patch -p1 < ../rh1186806-0004-domain_match.patch
patch -p1 < ../rh1186806-0005-cert_in_cb.patch

# Fix integer underflow in WMM Action frame parser: rh #1221178
patch -p1 < ../0056-rh1221178-fix-int-unferflow-AP-WMM.patch

# Do not quote values for scan_freq and freq_list: rh #1254486
patch -p1 < ../rh1254486-dont-qoute-scan_freq-and-freq_list.patch

# reopen debug log file upon receipt of SIGHUP signal
patch -p1 < ../0057-rh908306-log-rotate.patch

# Deauthenticate on reconfiguration: rh #1359044
patch -p1 < ../0058-rh1359044-Deauthenticate-on-reconfiguration.patch

patch -p1 < ../rh1495530-0001-Clear-TK-part-of-PTK-after-driver-key-configuration.patch
patch -p1 < ../rh1495530-0002-Fix-TK-configuration-to-the-driver-in-EAPOL-Key-3-4-.patch
patch -p1 < ../rh1495530-0003-Reduce-the-amount-of-time-PTK-TPTK-GTK-is-kept-in-me.patch
patch -p1 < ../rh1495530-0004-Prevent-reinstallation-of-an-already-in-use-group-ke.patch
patch -p1 < ../rh1495530-0005-Prevent-installation-of-an-all-zero-TK.patch
patch -p1 < ../rh1495530-0006-Fix-PTK-rekeying-to-generate-a-new-ANonce.patch
patch -p1 < ../rh1495530-0007-FT-Do-not-allow-multiple-Reassociation-Response-fram.patch

# build
cp ../wpa_supplicant_config wpa_supplicant/.config

cd wpa_supplicant
CC=armv7l-timesys-linux-gnueabihf-gcc CXX=armv7l-timesys-linux-gnueabihf-g++ make -j16

cd ../..

###############################################################################
#
# create a debian package.
#
###############################################################################

# build debian package
rm -f *.deb

mkdir -p ./debian/DEBIAN

sed "s/VERSION/${VERSION}/g" ./deb/control > ./debian/DEBIAN/control
cp ./deb/preinst ./debian/DEBIAN
cp ./deb/postinst ./debian/DEBIAN

# install
mkdir -p ./debian/usr/sbin
install -v -m755 ${PACKAGE}/wpa_supplicant/wpa_{cli,passphrase,supplicant} ./debian/usr/sbin/

# make package!
find ./debian -type d | xargs chmod 755
find ./debian -type d | xargs chmod g-s

fakeroot dpkg-deb --build debian

mv debian.deb hobart-wpasupplicant_${VERSION}_hti_armhf.deb


