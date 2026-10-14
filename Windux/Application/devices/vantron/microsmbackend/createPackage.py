import os
import shutil
import sys

if __name__ == '__main__':

    # generate directory tree
    if os.path.isdir( 'debian' ):
        shutil.rmtree( 'debian' )

    os.makedirs( 'debian' )
    os.makedirs( os.path.sep.join( ['debian', 'DEBIAN'] ) )

    os.makedirs( os.path.sep.join( ['debian', 'etc'] ) )
    os.makedirs( os.path.sep.join( ['debian', 'etc', 'hobart'] ) )

    os.makedirs( os.path.sep.join( ['debian', 'tmp'] ) )

    # copy files
    shutil.copy2( 'postinst', os.path.sep.join( ['debian', 'DEBIAN'] ) )
    shutil.copy2( 'preinst', os.path.sep.join( ['debian', 'DEBIAN'] ) )

    shutil.copy2( 'vantron', os.path.sep.join( ['debian', 'etc', 'hobart'] ) )

    shutil.copy2( 'python2.7.zip', os.path.sep.join( ['debian', 'tmp'] ) )
    shutil.copy2( 'pycrypto-2.6.1-armle.tar.gz', os.path.sep.join( ['debian', 'tmp'] ) )

    sys.exit( 0 )

