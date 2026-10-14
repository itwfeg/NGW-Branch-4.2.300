#!/bin/bash
set -x

workingdirectory=`pwd`
export PYTHONPATH="${workingdirectory}/Application/shared/sessionHandler/python"

cd ${workingdirectory}/Application/shared/util/SD_Migration_Feature
python build_sd_migration.py -n 
