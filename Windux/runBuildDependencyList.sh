
#-----------------------------------------------------------------------------------------------
#  Executes build_dependency_list.py under Linux.  This python script resides in
#  ScaleManagement/smBackend.
#
#  This requires Python 2.7, which is not installed on ENGDEV1, but can be run on wwbuildserver.
# 
#  Parameters: None
#
#  Must be run from the Windux directory.  Generates "sm_backend_dependencies.txt" which will be
#  placed in this same directory.
#
#  If the sm_backend_dependencies.txt file that is generated is different than the version
#  in Subversion, then check it in BEFORE running the build using installToTarget.sh
#  on wwbuildserver.
#-----------------------------------------------------------------------------------------------

workingdirectory=`pwd`
export PYTHONPATH="$workingdirectory/../ScaleManagement:$workingdirectory/../ScaleManagement/smBackend:$workingdirectory/Application/shared/sessionHandler/python:$workingdirectory/Application/shared/util/dongleKey:$workingdirectory/Application/shared/util/licensing:$workingdirectory/../ScaleManagement/source/payload:$workingdirectory/../ScaleManagement/source:$workingdirectory/../ScaleManagement/SQLLiteParser"

cd ../ScaleManagement/smBackend

python build_dependency_list.py

cd $workingdirectory
