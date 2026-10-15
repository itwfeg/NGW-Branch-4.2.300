#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import getSerialFlashInfo

if os.name == 'posix':
    PATCH_UPGRADE_FILENAME = "hti-linux-upgrade"

    CORE_UPGRADE_OLD_FILENAME = "hte"
    CORE_UPGRADE_FILENAME = "hti-linux-core"

    APPLICATION_UPGRADE_FILENAME = "hti-linux-application"

    BOOTLOADER_UPGRADE_OLD_FILENAME = "hte-uBoot"
    BOOTLOADER_UPGRADE_FILENAME = "hti-linux-uboot"

    RECOVERY_UPGRADE_OLD_FILENAME = "hte-recovery"
    RECOVERY_UPGRADE_FILENAME = "hti-linux-recovery"

    LANGUAGE_UPGRADE_OLD_FILENAME = "hte-lang_[a-z][a-z]"
    LANGUAGE_UPGRADE_FILENAME = "htseries-any-training_videos_[a-z]+"
    FEATURE_UPGRADE_OSAGNOSTIC_FILENAME = "htseries-any-feature-[a-z,_]+"
    MONOLITHIC_UPGRADE_FILENAME = "hti-linux-feature-monolithic_core+"

    FPC_UPGRADE_OLD_FILENAME = "hte-fpc"
    FPC_UPGRADE_FILENAME = "htseries-any-fpc"

    KPC_BOOT_UPGRADE_FILENAME = "htseries-any-kpc-bootloader"
    FSSS_BOOT_UPGRADE_FILENAME = "htseries-any-fsss-bootloader"

    HFPD_UPGRADE_FILENAME = "htseries-any-hfpd-[a-z,_]+"

    FEATURE_UPGRADE_OSAGNOSTIC_FILENAME_REGEX = "htseries-any-feature-[a-z,_]+-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

    LICENSE_INSTALL_FILENAME_REGEX = "[a-z,0-9,A-Z,_]+\.lic"
    FEATURE_UPGRADE_OLD_FILENAME = "hte-feature_[a-z,_]+"
    # Need to support hti, hts, dshtacc, ptrhtque
    FEATURE_UPGRADE_FILENAME = "[a-z,_]+-linux-feature-[a-z,0-9,_]+"

    BACKEND_UPGRADE_FILENAME = "hte-feature_sm_backend"
    BACKEND_UPGRADE_NEW_FILENAME = "htseries-any-feature-sm_backend"

    LICENSE_INSTALL_FILENAME = "[a-z,0-9,A-Z,_]+"

elif os.name == 'nt':
    try:
        modelName = getSerialFlashInfo.getModelName()
    except:
        modelName = ''

    if modelName:
        PATCH_UPGRADE_FILENAME = "hti-win-upgrade"

        CORE_UPGRADE_OLD_FILENAME = "hte"
        CORE_UPGRADE_FILENAME = "htx-win-core"

        BOOTLOADER_UPGRADE_OLD_FILENAME = "hte-uBoot"
        BOOTLOADER_UPGRADE_FILENAME = "htx-win-uboot"

        APPLICATION_UPGRADE_FILENAME = "hti-linux-application"

        RECOVERY_UPGRADE_OLD_FILENAME = "hte-recovery"
        RECOVERY_UPGRADE_FILENAME = "htx-win-recovery"

        LANGUAGE_UPGRADE_OLD_FILENAME = "hte-lang_[a-z][a-z]"
        LANGUAGE_UPGRADE_FILENAME = "htseries-any-training_videos_[a-z]+"

        FPC_UPGRADE_OLD_FILENAME = "hte-fpc"
        FPC_UPGRADE_FILENAME = "htseries-any-fpc"

        KPC_BOOT_UPGRADE_FILENAME = "htseries-any-kpc-bootloader"
        FSSS_BOOT_UPGRADE_FILENAME = "htseries-any-fsss-bootloader"
        HFPD_UPGRADE_FILENAME = "htseries-any-hfpd-[a-z,_]+"

        FEATURE_UPGRADE_OLD_FILENAME = "hte-feature_[a-z,_]+"
        FEATURE_UPGRADE_FILENAME = "htx-win-feature-[a-z,_]+"
        FEATURE_UPGRADE_OSAGNOSTIC_FILENAME = "htseries-any-feature-[a-z,_]+"

        BACKEND_UPGRADE_FILENAME = "hte-feature_sm_backend"
        BACKEND_UPGRADE_NEW_FILENAME = "htseries-any-feature-sm_backend"

        LICENSE_INSTALL_FILENAME = "[a-z,0-9,A-Z,_]+"

        MONOLITHIC_UPGRADE_FILENAME = "hti-linux-feature-monolithic_core+"

    else:
        PATCH_UPGRADE_FILENAME = "hti-linux-upgrade"

        CORE_UPGRADE_OLD_FILENAME = "hte"
        CORE_UPGRADE_FILENAME = "hti-linux-core"

        APPLICATION_UPGRADE_FILENAME = "hti-linux-application"

        BOOTLOADER_UPGRADE_OLD_FILENAME = "hte-uBoot"
        BOOTLOADER_UPGRADE_FILENAME = "hti-linux-uboot"

        RECOVERY_UPGRADE_OLD_FILENAME = "hte-recovery"
        RECOVERY_UPGRADE_FILENAME = "hti-linux-recovery"

        LANGUAGE_UPGRADE_OLD_FILENAME = "hte-lang_[a-z][a-z]"
        LANGUAGE_UPGRADE_FILENAME = "htseries-any-training_videos_[a-z]+"
        FEATURE_UPGRADE_OSAGNOSTIC_FILENAME = "htseries-any-feature-[a-z,_]+"
        MONOLITHIC_UPGRADE_FILENAME = "hti-linux-feature-monolithic_core+"

        FPC_UPGRADE_OLD_FILENAME = "hte-fpc"
        FPC_UPGRADE_FILENAME = "htseries-any-fpc"
        FEATURE_UPGRADE_OSAGNOSTIC_FILENAME_REGEX = "htseries-any-feature-[a-z,_]+-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

        KPC_BOOT_UPGRADE_FILENAME = "htseries-any-kpc-bootloader"
        FSSS_BOOT_UPGRADE_FILENAME = "htseries-any-fsss-bootloader"
        HFPD_UPGRADE_FILENAME = "htseries-any-hfpd-[a-z,_]+"

        LICENSE_INSTALL_FILENAME_REGEX = "[a-z,0-9,A-Z,_]+\.lic"
        FEATURE_UPGRADE_OLD_FILENAME = "hte-feature_[a-z,_]+"
        # Need to support hti, hts, dshtacc, ptrhtque
        FEATURE_UPGRADE_FILENAME = "[a-z,_]+-linux-feature-[a-z,_]+"

        BACKEND_UPGRADE_FILENAME = "hte-feature_sm_backend"
        BACKEND_UPGRADE_NEW_FILENAME = "htseries-any-feature-sm_backend"

        LICENSE_INSTALL_FILENAME = "[a-z,0-9,A-Z,_]+"

CORE_UPGRADE_OLD_FILENAME_REGEX = CORE_UPGRADE_OLD_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
CORE_UPGRADE_FILENAME_REGEX = CORE_UPGRADE_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

APPLICATION_UPGRADE_FILENAME_REGEX = APPLICATION_UPGRADE_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

BOOTLOADER_UPGRADE_OLD_FILENAME_REGEX = BOOTLOADER_UPGRADE_OLD_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
BOOTLOADER_UPGRADE_FILENAME_REGEX = BOOTLOADER_UPGRADE_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

RECOVERY_UPGRADE_OLD_FILENAME_REGEX = RECOVERY_UPGRADE_OLD_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
RECOVERY_UPGRADE_FILENAME_REGEX = RECOVERY_UPGRADE_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

LANGUAGE_UPGRADE_OLD_FILENAME_REGEX = LANGUAGE_UPGRADE_OLD_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
LANGUAGE_UPGRADE_FILENAME_REGEX = LANGUAGE_UPGRADE_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

FPC_UPGRADE_OLD_FILENAME_REGEX = FPC_UPGRADE_OLD_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
FPC_UPGRADE_FILENAME_REGEX = FPC_UPGRADE_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

KPC_BOOT_UPGRADE_FILENAME_REGEX = KPC_BOOT_UPGRADE_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
FSSS_BOOT_UPGRADE_FILENAME_REGEX = FSSS_BOOT_UPGRADE_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

HFPD_UPGRADE_FILENAME_REGEX = HFPD_UPGRADE_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

FEATURE_UPGRADE_OLD_FILENAME_REGEX = FEATURE_UPGRADE_OLD_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
FEATURE_UPGRADE_FILENAME_REGEX = FEATURE_UPGRADE_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

BACKEND_UPGRADE_FILENAME_REGEX = BACKEND_UPGRADE_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
BACKEND_UPGRADE_NEW_FILENAME_REGEX = BACKEND_UPGRADE_NEW_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

LICENSE_INSTALL_FILENAME_REGEX = LICENSE_INSTALL_FILENAME + "\.lic"

FEATURE_UPGRADE_OSAGNOSTIC_FILENAME_REGEX = FEATURE_UPGRADE_OSAGNOSTIC_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

MONOLITHIC_UPGRADE_FILENAME_REGEX = MONOLITHIC_UPGRADE_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

PATCH_UPGRADE_FILENAME_REGEX = PATCH_UPGRADE_FILENAME + "-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

NGS_UPGRADE_FILENAME_REGEX = "htseries-any-ngs-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

NGW_UPGRADE_FILENAME_REGEX = "htseries-any-ngw-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
NGW2P_UPGRADE_FILENAME_REGEX = "htseries-any-ngw2p-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

AWS3_UPGRADE_OLD_FILENAME_REGEX = "hte-aws3-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
AWS3_UPGRADE_FILENAME_REGEX = "htseries-any-aws3-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

AWS_UPGRADE_OLD_FILENAME_REGEX = "hte-aws-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
AWS_UPGRADE_FILENAME_REGEX = "htseries-any-aws-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

EWS_UPGRADE_OLD_FILENAME_REGEX = "hte-ews-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
EWS_UPGRADE_FILENAME_REGEX = "htseries-any-ews-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

CLA_UPGRADE_OLD_FILENAME_REGEX = "hte-cla-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
CLA_UPGRADE_FILENAME_REGEX = "htseries-any-cla-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

XPE_UPGRADE_OLD_FILENAME_REGEX = "hte-xpe-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
XPE_UPGRADE_FILENAME_REGEX = "htseries-any-xpe-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

EVO_UPGRADE_OLD_FILENAME_REGEX = "hte-evo-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
EVO_UPGRADE_FILENAME_REGEX = "htseries-any-evo-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

WGH_UPGRADE_OLD_FILENAME_REGEX = "hte-wgh-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"
WGH_UPGRADE_FILENAME_REGEX = "htseries-any-wgh-[0-9]+\.[0-9]+\.[0-9]+-.*\.tgz"

HTE_MULTIMEDIA_FILENAME_REGEX = "hte-multimedia-[0-9,a-z,_]+-(0[1-9]|1[0-2])([012][0-9]|3[0-1])20[0-9][0-9]_([01][0-9]|2[0-3])[0-5][0-9][0-5][0-9]\.tgz"
