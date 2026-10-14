#!/bin/bash

#
# This script will run lupdate against the SCALEUI project for the following languages:
# en_CA
# es_MX
# fr_CA
#
# After running this script you then may run linguist (and lrelease eventually).
#

export LUPDATE=/opt/Qt5.6.0/5.6/gcc_64/bin/lupdate

$LUPDATE -target-language en_CA -pro scaleui.pro -ts ../shared/sessionHandler/languages/lang_en_ca.ts
$LUPDATE -target-language es_MX -pro scaleui.pro -ts ../shared/sessionHandler/languages/lang_es_mx.ts
$LUPDATE -target-language fr_CA -pro scaleui.pro -ts ../shared/sessionHandler/languages/lang_fr_ca.ts

