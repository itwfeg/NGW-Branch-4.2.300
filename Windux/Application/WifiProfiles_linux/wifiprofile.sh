#!/bin/bash

# Make sure only root can run our script
if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root" 1>&2
    exit 1
fi

wicdconf="/etc/wicd/wireless-settings.conf"
option=$(zenity  --list  \
        --text "Choose to backup or restore /etc/wicd/wireless-settings.conf" \
        --title "Chose option" \
        --radiolist \
        --print-column=2 \
        --column "Selection" \
        --column "Option" TRUE Backup FALSE Restore)

if [ "$option" == "Backup" ];then
    szSavePath=$(zenity --title "Choose file to $option to" --file-selection --save --confirm-overwrite);echo $szSavePath
    echo "Backing up $wicdconf to $szSavePath"
    openssl des3 -salt -in $wicdconf -out $szSavePath -pass pass:hobart
else
    szSavePath=$(zenity --title "Choose file to $option from" --file-selection);echo $szSavePath
    echo "Restoring $wicdconf from $szSavePath"
    openssl des3 -d -salt -in $szSavePath -out $wicdconf -pass pass:hobart
fi
