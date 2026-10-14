import os
import sys
import shutil

def getImportedModules( moduleList ):

    modules = []

    # import required modules
    import buildFeature

    for m in moduleList:
        modulePaths, names = buildFeature.getImportedModules( m )

        importedModules = []

        # generate list of imported modules
        for mpath in modulePaths:
            importedModules.append( mpath['path'] + os.path.sep + mpath['name'] )

        modules = list( set( modules + importedModules ) )

    return modules

if __name__ == '__main__':

    # update python path to include our sources
    cwd = os.path.dirname( os.path.realpath( __file__ ) )
    cwdList = cwd.split( os.path.sep )

    while ( 'Windux' != cwdList[-1] ):
        del cwdList[-1]

    del cwdList[-1]

    windux = list( cwdList )
    windux.append( 'Windux' )
    windux.append( 'Application' )
    windux.append( 'shared' )
    windux.append( 'sessionHandler' )
    windux.append( 'python' )

    winduxPath = os.path.sep.join( windux )
    sys.path.append( winduxPath )

    scaleManagement = list( cwdList )
    scaleManagement.append( 'ScaleManagement' )
    scaleManagement.append( 'smBackend' )

    scaleManagementPath = os.path.sep.join( scaleManagement )
    sys.path.append( scaleManagementPath )

    # generate list of modules
    moduleList = []
    moduleList.append( scaleManagementPath + os.path.sep + 'autoconfig.py' )
    moduleList.append( scaleManagementPath + os.path.sep + 'gatewayDiscovery.py' )
    moduleList.append( scaleManagementPath + os.path.sep + 'microSmBackend.py' )
    moduleList += getImportedModules( moduleList )

    # generate directory tree
    if os.path.isdir( 'debian' ):
        shutil.rmtree( 'debian' )

    os.makedirs( 'debian' )
    os.makedirs( os.path.sep.join( ['debian', 'DEBIAN'] ) )

    os.makedirs( os.path.sep.join( ['debian', 'etc'] ) )
    os.makedirs( os.path.sep.join( ['debian', 'etc', 'init.d'] ) )

    os.makedirs( os.path.sep.join( ['debian', 'usr'] ) )
    os.makedirs( os.path.sep.join( ['debian', 'usr', 'local'] ) )
    os.makedirs( os.path.sep.join( ['debian', 'usr', 'local', 'hobart'] ) )
    os.makedirs( os.path.sep.join( ['debian', 'usr', 'local', 'hobart', 'audio'] ) )
    os.makedirs( os.path.sep.join( ['debian', 'usr', 'local', 'hobart', 'images'] ) )
    os.makedirs( os.path.sep.join( ['debian', 'usr', 'local', 'hobart', 'python'] ) )
    os.makedirs( os.path.sep.join( ['debian', 'usr', 'local', 'hobart', 'upgrade'] ) )
    os.makedirs( os.path.sep.join( ['debian', 'usr', 'local', 'hobart', 'videos'] ) )

    # copy files
    shutil.copy2( 'control', os.path.sep.join( ['debian', 'DEBIAN'] ) )

    shutil.copy2( 'autoconfig', os.path.sep.join( ['debian', 'etc', 'init.d'] ) )
    shutil.copy2( 'gatewayDiscovery', os.path.sep.join( ['debian', 'etc', 'init.d'] ) )
    shutil.copy2( 'microSmBackend', os.path.sep.join( ['debian', 'etc', 'init.d'] ) )

    for m in moduleList:
        shutil.copy2( m, os.path.sep.join( ['debian', 'usr', 'local', 'hobart', 'python'] ) )

    sys.exit( 0 )

