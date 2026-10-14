import tarfile
import sys
import glob
import os


def build_runtime_tarball(tarball_filename):
    whereIAm=os.path.dirname(os.path.realpath(__file__))

    if os.path.exists(tarball_filename):
        os.remove(tarball_filename)

    with tarfile.open(tarball_filename, "w:gz") as tar:
        print 'Inserting files or directories'
        for python_file in glob.glob(os.path.join(whereIAm,'*.py')):
            print "adding the file %s" % python_file
            tar.add(python_file,arcname=os.path.basename(python_file))

        print 'Inserting the DFU library'
        tar.add(os.path.join(whereIAm, 'pc-nrfutil-0.5.3'), recursive=True, arcname='pc-nrfutil-0.5.3')

        print 'Creating empty updates folder'
        # add empty updates folder
        updates_dir = tarfile.TarInfo('updates')
        updates_dir.type = tarfile.DIRTYPE
        tar.addfile(updates_dir)

        print 'Tarfile Created ' + os.path.abspath(tarball_filename)


if __name__ == '__main__':

    if (len(sys.argv) == 2):
        sys.exit(build_runtime_tarball(sys.argv[1]))
    else:
        sys.exit(build_runtime_tarball('hflb_programmer_runtime.tar.gz'))
