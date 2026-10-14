import argparse
import compileall
import os
import platform
import shutil
import sys


def main():
    parser = argparse.ArgumentParser(description="Install Scale Backend Application for installToTarget")
    parser.add_argument("svn_branch",
                        action="store",
                        help="The full path to the SVN branch for this build.")
    parser.add_argument("live_path",
                        action="store",
                        help="The full path to the live directory to place files.")
    parser.add_argument("application_path",
                        action="store",
                        help="The full path to the application directory to place files.")
    parser.set_defaults(windows=False)
    args = parser.parse_args()

    sys.path.append(os.path.join(args.svn_branch, 'Windux', 'Application', 'shared', 'sessionHandler', 'python'))
    sys.path.append(os.path.join(args.svn_branch, 'Windux', 'Application', 'shared', 'util', 'dongleKey'))
    sys.path.append(os.path.join(args.svn_branch, 'Windux', 'Application', 'shared', 'util', 'licensing'))
    sys.path.append(os.path.join(args.svn_branch, 'ScaleManagement', 'smBackend'))
    sys.path.append(os.path.join(args.svn_branch, 'ScaleManagement'))

    windows_build = platform.system().lower() == 'windows'

    if not windows_build:
        live_install_path = os.path.join(args.live_path, 'usr', 'local', 'hobart', 'backend')
        application_install_path = os.path.join(args.application_path, 'usr', 'local', 'hobart', 'backend')
    else:
        live_install_path = os.path.join(args.live_path, 'Hobart', 'Backend')
        application_install_path = os.path.join(args.application_path, 'Hobart', 'Backend')

    if not os.path.exists(live_install_path):
        os.makedirs(live_install_path)

    if not os.path.exists(application_install_path) and not windows_build:
        os.makedirs(application_install_path)

    with open('sm_backend_dependencies.txt') as dependency_file:
        for line in dependency_file:
            if not line.strip() or line.strip().startswith('#'):
                continue

            dependency = os.path.join('..', line.strip())

            if not os.path.isfile(dependency):
                continue

            if dependency.endswith('install.py'):
                destination_path = os.path.join(live_install_path, 'install.py')
                application_path = os.path.join(application_install_path, 'install.py')
            else:
                destination_path = os.path.join(live_install_path, os.path.basename(dependency))
                application_path = os.path.join(application_install_path, os.path.basename(dependency))

            shutil.copyfile(dependency, destination_path)

            if not windows_build:
                shutil.copyfile(dependency, application_path)


if __name__ == '__main__':
    main()
