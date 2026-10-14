#!/usr/bin/env python3
# -*- coding: utf-8 -*-

""" Implement an extension to smBackend's server with commands to perform scale updates based on checksums. """
import glob
import os
import tarfile
import time

import md5checksum

import database_backup

from functools import wraps
from threading import Thread
from twisted.internet import reactor

from sftpProtocol import SFTPClient, SCPError
from logger import Logger


class RestrictSingleRun(object):
    """ Create a decorator that restricts the wrapped function to only one running instance at a time. """

    def __init__(self, error_message):
        """ Constructor.

            :param error_message: The message to display if the function is called while already running.
        """
        self.running = False
        self.error_message = error_message

    def __call__(self, function):
        """ Create and return the function wrapper. """
        @wraps(function)
        def wrapper(*args, **kwargs):
            if self.running:
                raise TypeError(self.error_message)

            self.running = True

            try:
                function(*args, **kwargs)
            finally:
                self.running = False

        return wrapper


class UpdateNowThread(Thread):
    """ Represent the thread handler that will be called to handle an UpdateNow command. """

    def __init__(self, queue, configDict, debugging=True):
        """ Constructor.

            :param queue: The TaskQueue instance to pull command data from.
            :param configDict: The dictionary of configuration values for this system.
            :param debugging: Flag to turn on debug logging.
        """
        Thread.__init__(self)

        self.queue = queue
        self.configDict = configDict
        self.debugging = True

        log_file_path = os.path.join(self.configDict['LocalFolder'], 'update_now.log')
        self.logger = Logger(log_file_path, printing=self.debugging)

        self.backup_directory = os.path.join(self.configDict['LocalFolder'], 'backup')
        self.local_checksum_filepath = os.path.join(self.configDict['LocalFolder'], 'checksums.txt')

        self.excluded_files = set(('bothChannels.aac',
                                   'leftChannel.aac',
                                   'rightChannel.aac',
                                   'scanbeep.wav',
                                   'Accumulate_Items_Video.mp4',
                                   'FlashKey_Basics_Video.mp4',
                                   'NGW_ChangingLabels.mp4',
                                   'NGW_Cleaning.mp4',
                                   'NGW_CleaningthePrinthead.mp4',
                                   'NGW_LoadingFilm.mp4',
                                   'NGW_Training.mp4',
                                   'Printer_Overview_Video.mp4',
                                   'Scale_Navigation_Video.mp4',
                                   'Search_Capabilities_Video.mp4',
                                   'Super_ConfigOp_Video.mp4',
                                   'Super_Cool_Video.mp4',
                                   'Super_FlashkeyEd_Video.mp4',
                                   'Super_OpScreen_Video.mp4',
                                   'Super_Overview_Video.mp4',
                                   'Tare_Functionality_Video.mp4',
                                   'imgDeliMeat.png',
                                   'imgBread.png',
                                   'imgWrap.png',
                                   'Yellow1.png',
                                   'pork.png',
                                   'imgCheese.png',
                                   '89x60_green.png',
                                   'imgMeal.png',
                                   'beef2.png',
                                   'skyBlueSolid.png',
                                   '182x60_green.png',
                                   'imgSandwich.png',
                                   'DK_Blue.png',
                                   'purpleGlass.png',
                                   'imgSalad.png',
                                   'Dk_Green.png',
                                   'Red1.png',
                                   'flash_blue_trimmed.png',
                                   'TareContainer4.png',
                                   '182x124_green.png',
                                   'orangeGlass.png',
                                   'less.gif',
                                   'paleGreenGlass.png',
                                   'Purple1.png',
                                   'blueSolid.png',
                                   'seafood2.png',
                                   'imgMeat.png',
                                   '182x60_blue.png',
                                   'TareContainer3.png',
                                   'orangeSolid.png',
                                   'TareContainer2.png',
                                   'poultry.png',
                                   'less.png',
                                   '89x60_blue.png',
                                   'paleGreenSolid.png',
                                   'poultry3.png',
                                   'greenSolid.png',
                                   'more.png',
                                   'imgFruit.png',
                                   'purpleSolid.png',
                                   'beef1.png',
                                   'greenGlass.png',
                                   'beef3.png',
                                   'seafood1.png',
                                   'skyBlueGlass.png',
                                   'pinkSolid.png',
                                   'blueGlass.png',
                                   'Orange1.png',
                                   'TareContainer1.png',
                                   'imgCrab.png',
                                   'imgSides.png',
                                   'imgBreakfast.png',
                                   'chartreuseSolid.png',
                                   'imgPizza.png',
                                   'chartreuseGlass.png',
                                   'flash_green_trimmed.png',
                                   'pinkGlass.png',
                                   'more.gif',
                                   'imgPartyTray.png',
                                   '182x124_blue.png',
                                   'template.gif',
                                   'newES.gif',
                                   'new_es.gif',
                                   'new_en.gif',
                                   'sale_fr.gif',
                                   'sale.gif',
                                   'saleES.gif',
                                   'sale_en.gif',
                                   'new.gif',
                                   'new_fr.gif',
                                   'sale_es.gif',
                                   'defaultLayout.xml',
                                   'TLXML11.xml',
                                   'TLXML12.xml',
                                   'TLXML21.xml',
                                   'TLXML22.xml',
                                   'TLXML31.xml',
                                   'TLXML32.xml',
                                   'TLXML41.xml',
                                   'TLXML42.xml',
                                   'TLXML51.xml',
                                   'TLXML52.xml',
                                   'TLXML61.xml',
                                   'TLXML62.xml',
                                   'TLXML71.xml',
                                   'TLXML72.xml',
                                   'adpanel.png',
                                   'annun.png',
                                   'annunw.png',
                                   'background.png',
                                   'backgroundFS.png',
                                   'background_pick5.png',
                                   'bakery.png',
                                   'bakeryFS.png',
                                   'bakeryw.png',
                                   'bakeryw_adpanel.png',
                                   'bakeryw_fspanel.png',
                                   'deli.png',
                                   'deliFS.png',
                                   'deliw.png',
                                   'deliw_adpanel.png',
                                   'deliw_fspanel.png',
                                   'floral.png',
                                   'floralFS.png',
                                   'floralw.png',
                                   'floralw_adpanel.png',
                                   'floralw_fspanel.png',
                                   'grey.png',
                                   'greyFS.png',
                                   'greyw.png',
                                   'greyw_adpanel.png',
                                   'greyw_fspanel.png',
                                   'mainmarket.png',
                                   'meat.png',
                                   'meatFS.png',
                                   'meatw.png',
                                   'meatw_adpanel.png',
                                   'meatw_fspanel.png',
                                   'outOfService.png',
                                   'prePackAnnun.png',
                                   'produce.png',
                                   'produceFS.png',
                                   'producew.png',
                                   'producew_adpanel.png',
                                   'producew_fspanel.png',
                                   'seafood.png',
                                   'seafoodFS.png',
                                   'seafoodw.png',
                                   'seafoodw_adpanel.png',
                                   'seafoodw_fspanel.png',
                                   'zeroAnnun.png',
                                   'Readme.txt'))
        self.excluded_roots = [
            "system",
        ]
        self.multimedia_directories = ('videos',
                                       'audio',
                                       'images',
                                       'user',
                                       'playlists',
                                       'upgrade',
                                       'layouts',
                                       )
        self.database_files = ('!clears.ht',
                               'plu_data.ht',
                               'label_data.ht',
                               'flashkey_data.ht',
                               'operator_data.ht',
                               'configuration_data.ht',
                               'store_data.ht',
                               'synchronization_data.ht',
                               )
        self.database_configuration_flags = ('plu_data',
                                             'label_data',
                                             'flashkey_data',
                                             'operator_data',
                                             'configuration_data',
                                             'store_data',
                                             'synchronization_data',
                                             )
        self.all_configuration_flags = ('videos',
                                        'images',
                                        'audio',
                                        'playlists',
                                        'plu_data',
                                        'label_data',
                                        'flashkey_data',
                                        'operator_data',
                                        'configuration_data',
                                        'store_data',
                                        'synchronization_data',
                                        )

    def run(self):
        """ Run the command handler from its own thread of execution."""
        task = self.queue.get()

        self.dispatch(task)

    def dispatch(self, task):
        """ Find the correct method handler for the given command, and execute it.

            :param task: The Task object with the command to run.
        """
        args = task.command.split('|')
        connection = task.sender

        try:
            method = getattr(self, args[0])
        except AttributeError:
            self.logger.log('warning', 'Invalid command {0} received, ignoring'.format(args[0]))
            reactor.callFromThread(connection.sendLine, self.error_message(args, 'unrecognized command, rejected').encode())
            return

        response_message = None

        try:
            response_message = method(args, connection)
        except Exception as err:
            self.logger.log_exc(err)
            reactor.callFromThread(connection.sendLine, self.error_message(args, str(err)).encode())
            return self.queue.task_done(task, True, respond=False)

        if response_message:
            args.insert(-2, response_message)

        reactor.callFromThread(connection.sendLine, self.success_message(args).encode())

        self.queue.task_done(task, False, respond=False)

    @RestrictSingleRun('A backup is already taking place, command rejected')
    def backupNow(self, args, connection):
        """ Command slot that sends requested backup data to the given network device.

            format: backupNow|ip_address|backup_location|configuration_flags|username|password|id
            ip_address: The network address of the device to backup too.
            backup_location: The root location (relative to server root) to backup our data too.
                (This will add an implicit "./" to the beginning of the path given)
            id: An arbitrary number used to represent this command, means nothing to smBackend itself, but is returned on completion or failure.
            configuration_flags: A comma-separated list of the pieces of data to backup. Leaving this parameter empty will backup all data.
                valid flags:
                    videos,
                    images,
                    audio,
                    playlists,
                    plu_data,
                    label_data,
                    flashkey_data,
                    operator_data,
                    configuration_data,
                    store_data,
                    synchronization_data,

            example command:
                backupNow|10.3.177.26|meat_department|videos,images,product_data,marquees,special_messages,nutrifacts|username|password|1
                backupNow|10.3.177.26|meat_department||username|password|1

            error responses:
                if a backup is already taking place:
                    error|backupNow|a backup is already taking place, command rejected|id
                if the incorrect number of arguments are given:
                    error|backupNow|not enough arguments for backupNow command, expected syntax: "{command_syntax}"|id
                if the password given cannot be decrypted:
                    error|backupNow|invalid encrypted password {password}|id
                if the sftp server could not be connected to:
                    error|backupNow|could not connect to sftp server at {ip_address}|id
                various errors can occur during transfer of backup tarball to sftp server:
                    error|backupNow|{error_message}|id

            success response:
                backupNow|ip_address|remote_backup_location|configuration_flags|user|password|id|done
        """
        self._verify_command_arguments(args, 8, syntax_help='backupNow|ip_address|backup_location|{configuration_flags}|username|password|id')

        ip_address = args[1]
        user = args[4]

        password = args[5]
        remote_backup_location = args[2].rstrip('/') + '/'
        remote_md5s = False
        remote_checksum_filepath = os.path.join(self.configDict['ArchiveFolder'], 'remote_checksums.txt')

        if args[3]:
            configuration_flags = args[3].split(',')
        else:
            configuration_flags = self.all_configuration_flags

        sftp_client = SFTPClient(user, password, ip_address)
        connection_result = sftp_client.connect()

        if not connection_result == 'success' and not connection_result == 'alreadyConnected':
            raise RuntimeError('could not connect to sftp server at {0}'.format(ip_address))

        remote_md5s = download_if_exists('{0}checksums.txt'.format(remote_backup_location), remote_checksum_filepath, sftp_client, logger=self.logger)
        local_checksums = read_checksum_file(self.local_checksum_filepath)

        if remote_md5s:
            remote_checksums = read_checksum_file(remote_checksum_filepath)
            changed_filepaths = find_changed_files(local_checksums, remote_checksums)
        else:
            changed_filepaths = list(local_checksums.keys())

        backup_archive_filepath = os.path.join(self.configDict['ArchiveFolder'], 'backup_now.tgz')
        filtered_filepaths = filter_configuration_flags(changed_filepaths, configuration_flags)

        with tarfile.open(backup_archive_filepath, 'w:gz') as backup_archive:
            for filtered_filepath in filtered_filepaths:
                absolute_filepath = os.path.join(self.configDict['LocalFolder'], filtered_filepath)
                backup_archive.add(absolute_filepath, arcname=filtered_filepath)

            backup_archive.add(self.local_checksum_filepath, arcname='checksums.txt')

        try:
            sftp_client.uploadFile(remote_backup_location, backup_archive_filepath)
        except SCPError as err:
            if 'Failed to open file' in str(err) or 'No such file' in str(err):
                raise SCPError('Invalid remote backup location given, cannot complete backupNow')
            else:
                raise

    @RestrictSingleRun('An update is already taking place, command rejected')
    def updateNow(self, args, connection):
        """ Command slot that performs a smart update using md5 checksum differences with a remote SFTP Location.

            format: updateNow|ip_address|remote_update_location|configuration_flags|user|password|ht_download_directory|install_firmware|id
            ip_address: The IP Address of the remote SFTP Server to update from.
            remote_update_location: The root location (relative to server root) of the update files to use.
                (This will add an implicit "./" to the beginning of the path given)
            id: An arbitrary number used to represent this command, means nothing to smBackend itself, but is returned on completion or failure.
            username: The username to connect to the SFTP server with.
            password: The password to connect to the SFTP server with.
            configuration_flags: A comma-separated list of the pieces of data to backup. Leaving this parameter empty will backup all data.
                valid flags:
                    videos,
                    images,
                    audio,
                    playlists,
                    upgrade,
                    plu_data,
                    label_data,
                    flashkey_data,
                    operator_data,
                    configuration_data,
                    store_data,
                    synchronization_data,
            ht_download_directory: (Optional) The directory to download HTDF backup files too (defaults to ./download).
            install_firmware: (Optional) Flag to instruct updateNow to install upgrades (if any) after downloading.

            example commands:
                updateNow|10.3.177.95|meat_department|videos,images,plu_data,operator_data|1
                updateNow|10.3.177.95|meat_department||1

            error responses:
                if an update is already taking place:
                    error|updateNow|an update is already taking place, command rejected|id
                if the incorrect number of arguments are given:
                    error|updateNow|not enough arguments for updateNow command, expected syntax: "{command_syntax}"|id
                if the password given cannot be decrypted:
                    error|updateNow|invalid encrypted password {password}|id
                if the sftp server could not be connected to:
                    error|updateNow|could not connect to sftp server at {ip_address}|id
                if any number of files were not successfully downloaded after 3 tries:
                    error|updateNow|could not download the following files: filepath1,filepath2,filepath3,...|id

            status responses:
                if an updateNow must revert to using legacy functionality due to no checksum file found on sftp server:
                    performing updateNow using legacy functionality

            success response:
                updateNow|ip_address|remote_update_location|configuration_flags|user|password|id|done
        """
        self._verify_command_arguments(args, 10, syntax_help='updateNow|ip_address|remote_update_location|{configuration_flags}|username|password|download_directory|id')

        ip_address = args[1]
        remote_update_location = args[2].rstrip('/') + '/'
        user = args[4]
        password = args[5]
        ht_download_folder = args[6]

        # Make a backup of the database
        self.backupDatabase(args, connection)

        if not user:
            raise ValueError('no username was provided, command rejected')

        if args[7]:
            install_firmware = True
        else:
            install_firmware = False

        if not ht_download_folder:
            ht_download_folder = 'download'

        remote_checksum_filepath = os.path.join(self.configDict['ArchiveFolder'], 'remote_checksums.txt')

        # Split the different types of data to update
        if args[3]:
            configuration_flags = args[3].split(',')
        else:
            configuration_flags = self.all_configuration_flags

        # Create an SFTP connection
        sftp_client = SFTPClient(user, password, ip_address)
        connection_result = sftp_client.connect()

        if not connection_result == 'success' and not connection_result == 'alreadyConnected':
            raise RuntimeError('could not connect to sftp server at {0}'.format(ip_address))
        
        # Download the checksums.txt file from the primary to compare against checksums on this system
        remote_md5s = download_if_exists('{0}checksums.txt'.format(remote_update_location), remote_checksum_filepath, sftp_client, logger=self.logger)

        if remote_md5s:
            local_checksums = read_checksum_file(self.local_checksum_filepath)
            remote_checksums = read_checksum_file(remote_checksum_filepath)
            changed_filepaths = find_changed_files(local_checksums, remote_checksums)
        else:
            return self.updateNowLegacy(args, connection)

        # Get paths of files that have changed
        filtered_filepaths = filter_configuration_flags(changed_filepaths, configuration_flags)

        def download_database_backup_file(relative_filepath):
            backup_relative_path = relative_filepath.replace(ht_download_folder, 'backup')
            local_filepath = os.path.join(self.configDict['LocalFolder'], relative_filepath)
            server_backup_filepath = remote_update_location + backup_relative_path

            self.logger.log('info', 'Could not download HT backup from download, trying from backup directory.'.format(server_backup_filepath, local_filepath))

            try:
                sftp_client.downloadFile(server_backup_filepath, local_filepath)
                
            except SCPError:
                return False

            self.logger.log('info', "successfully found file in backup directory")
            return True

        def download_attempt(filepaths):
            failed_filepaths = []

            for filepath in filepaths:
                if 'download' in filepath:
                    destination = filepath.replace('download', ht_download_folder)
                else:
                    destination = filepath

                local_filepath = os.path.join(self.configDict['LocalFolder'], destination)

                if not os.path.exists(os.path.dirname(local_filepath)):
                    os.makedirs(os.path.dirname(local_filepath))

                server_filepath = remote_update_location + filepath

                self.logger.log('info', 'downloading {0} to {1}.'.format(server_filepath, local_filepath))

                try:
                    if not download_if_exists(server_filepath, local_filepath, sftp_client, logger=self.logger):
                        if 'download' in filepath:
                            if not download_database_backup_file(destination):
                                self.logger.log('error', 'failed to download from backup: ' + filepath)
                                failed_filepaths.append(filepath)
                except SCPError as err:
                    self.logger.log('error', str(err))
                    failed_filepaths.append(filepath)

            if failed_filepaths:
                self.logger.log('warning', 'The following files failed to download, and will be retried: {0}'.format(failed_filepaths))

            return failed_filepaths

        # Download all changed files
        failed_filepaths = bulk_sftp_download(download_attempt, filtered_filepaths, self.logger, max_retries=3, delay_period=2)

        if install_firmware:
            self.logger.log('info', 'Download completed, Installing upgrade files')
            self.installUpgrades(args, connection)

        if failed_filepaths:
            self.logger.log('could not download the following files: {0}'.format(','.join(failed_filepaths)))
            raise ValueError('could not download the following files: {0}'.format(','.join(failed_filepaths)))

        if not filtered_filepaths:
            self.logger.log('info', 'This scale is already in-sync with the Primary. No Update performed')
        else:
            self.logger.log('info', 'UpdateNow completed')

    @RestrictSingleRun('An update is already taking place, command rejected')
    def updateNowLegacy(self, args, connection):
        """ Perform an update now command using the legacy procedure of downloading entire directories from an sftp server.

            request format: updateNowLegacy|ip_address|remote_update_location|{configuration_flags}|username|password|ht_download_directory|id

            error responses:
                if an update is already taking place:
                    error|updateNow|an update is already taking place, command rejected|id
                if the incorrect number of arguments are given:
                    error|updateNow|not enough arguments for updateNow command, expected syntax: "{command_syntax}"|id
                if the password given cannot be decrypted:
                    error|updateNow|invalid encrypted password {password}|id
                if the sftp server could not be connected to:
                    error|updateNow|could not connect to sftp server at {ip_address}|id

            note: This command will be branched to automatically by the updateNow command if no md5 checksum list file can be found on the remote sftp server.
        """
        ip_address = args[1]
        user = args[4]
        password = args[5]
        remote_backup_location = args[2].rstrip('/') + '/'
        ht_download_folder = args[6]

        if not ht_download_folder:
            ht_download_folder = 'download'

        if args[7]:
            install_firmware = True
        else:
            install_firmware = False

        if args[3]:
            configuration_flags = args[3].split(',')
        else:
            configuration_flags = self.all_configuration_flags

        sftp_client = SFTPClient(user, password, ip_address)
        connection_result = sftp_client.connect()

        if not connection_result == 'success' and not connection_result == 'alreadyConnected':
            raise RuntimeError('could not connect to sftp server at {0}'.format(ip_address))

        download_directory = os.path.join(self.configDict['LocalFolder'], ht_download_folder)

        try:
            filenames = sftp_client.listFiles(remote_backup_location)
            if filenames is not None and len(filenames) > 0:
                for filename1 in filenames:
                    #self.logger.log('info', 'fullpath:' + filename1)
                    loopFilename = os.path.basename(os.path.abspath(filename1))
                    #self.logger.log('info', 'baseLoopFilename:' + loopFilename)
                    if loopFilename.strip('/').lower() == 'hobart':
                        remote_backup_location += loopFilename.rstrip('/') + '/' 
        except (SCPError, IOError):
            self.logger.log('info', 'could not list directories in User-given path')
            pass
			
        def download_sftp_directory(remote_directory, local_root):
            try:
                filenames = sftp_client.listFiles(remote_directory)
            except (SCPError, IOError):
                raise ValueError('Could not list files under {0}'.format(remote_directory))

            for filename in filenames:
                remote_filepath = remote_directory + '/' + filename
                local_filepath = os.path.join(local_root, filename)

                self.logger.log('info', 'downloading {0} to {1}'.format(remote_filepath, local_filepath))
                self.logger.log('info', local_root)

                if not os.path.exists(local_root):
                    self.logger.log('info', 'creating download directory {0}'.format(local_root))
                    os.makedirs(local_root)

                try:
                    if not download_if_exists(remote_filepath, local_filepath, sftp_client, logger=self.logger):
                        download_sftp_directory(remote_filepath, local_filepath)
                        continue
                except SCPError:
                    # TODO: Potentially check if the file actually got downloaded successfully and was just a false positive.
                    continue

        for configuration_flag in configuration_flags:
            if configuration_flag in self.database_configuration_flags:
                backup_filename = configuration_flag + '.ht'
                local_filepath = os.path.join(download_directory, backup_filename)
                self.logger.log('info', 'downloading {0} to {1}'.format(backup_filename, local_filepath))

                if not os.path.exists(download_directory):
                    os.makedirs(download_directory)

                if not download_if_exists(remote_backup_location + 'download/{0}'.format(backup_filename), local_filepath, sftp_client, logger=self.logger):
                    try:
                        sftp_client.downloadFile(remote_backup_location + 'backup/{0}'.format(backup_filename), local_filepath)
                    except SCPError:
                        self.logger.log('error', 'Could not download {0} from backup or download directory on source'.format(backup_filename))
                        continue

            else:
                base_directory = remote_backup_location + configuration_flag

                try:
                    download_sftp_directory(base_directory, os.path.join(self.configDict['LocalFolder'], configuration_flag))
                except ValueError as err:
                    self.logger.log('error', str(err))

        if install_firmware:
            self.logger.log('info', 'Download completed, Installing upgrade files')
            self.installUpgrades(args, connection)

        self.logger.log('info', 'UpdateNow completed')

    @RestrictSingleRun('Firmware is already being installed, command rejected')
    def installUpgrades(self, args, connection):
        """ Install any hobart-qualified features/upgrades under /usr/local/hobart/upgrade.

            request format: installUpgrades|id

            Features should respect the qualification to not reboot after installation based on the existance of the file "/tmp/dontreboot"
            If this file does exists, and a feature needs to reboot, they should flag this by creating the file "/tmp/needsreboot"
            In this way, after all features are installed, the scale will be automatically rebooted by this command if such a file exists.
        """
        upgrade_directory = os.path.join(self.configDict['LocalFolder'], 'upgrade')

        with open('/tmp/dontreboot', 'w') as temp_file:
            temp_file.write('\n')

        core_filepath = None

        if not os.listdir(upgrade_directory):
            raise ValueError('no upgrades to install')

        import installUpgrade

        for root, dirs, files in os.walk(upgrade_directory):
            for filename in files:
                if 'core' in filename:
                    core_filepath = os.path.join(root, filename)
                    continue

                filepath = os.path.join(root, filename)

                try:
                    exitCode = installUpgrade.installUpgradeNonTerminating(upgradeFilename=filepath, removeUpgrade=True)
                except AttributeError:
                    exitCode = installUpgrade.installUpgrade(upgradeFilename=filepath, removeUpgrade=True)

                if not exitCode == 0:
                    reactor.callFromThread(connection.sendLine, ('installation of firmware named {0} failed'.format(filename)).encode())

        if core_filepath:
            try:
                exitCode = installUpgrade.installUpgradeNonTerminating(upgradeFilename=core_filepath, removeUpgrade=True)
            except AttributeError:
                exitCode = installUpgrade.installUpgrade(upgradeFilename=core_filepath, removeUpgrade=True)

            if not exitCode == 0:
                reactor.callFromThread(connection.sendLine, ('installation of firmware named {0} failed'.format(filename)).encode())

        os.remove('/tmp/dontreboot')

        if os.path.exists('/tmp/needsreboot'):
            self.logger.log('info', 'Upgrades installed, rebooting scale to apply.')
            os.remove('/tmp/needsreboot')
            os.system('reboot')
        else:
            self.logger.log('info', 'Upgrades installed, no reboot required.')

    @RestrictSingleRun('A Database backup is already taking place, command rejected')
    def backupDatabase(self, args, connection):
        """ Backup the system database to the different backup files, and compute the filesystem checksums list.

            request format: backupDatabase|id
        """
        database_filepath = glob.glob(os.path.join(self.configDict['LocalFolder'], 'db', 'scale', 'dbfiles', 'dbft?.db'))[-1]
        backup_location = os.path.join(self.configDict['LocalFolder'], 'backup')

        if not os.path.exists(backup_location):
            os.makedirs(backup_location)

        database_backup.create_all_backups(database_filepath, backup_location, self.logger)
        self.makeChecksums(args, connection)

        self.logger.log('info', 'BackupDatabase completed')

    @RestrictSingleRun('Checksums are already being calculated, command rejected')
    def makeChecksums(self, args, connection):
        """ Generate a list of relative filepaths and checksums for all files on the local filesystem.

            These checksums will be written to the file /usr/local/hobart/checksums.txt using the following format:
                relative_filepath=md5_checksum
                relative_filepath=md5_checksum
                relative_filepath=md5_checksum
                ...

            request format: makeChecksums|id
        """
        checksums = []
        self.logger.log('info', 'Calculating checksums for filesystem and writing to /usr/local/hobart/checksums.txt')

        for multimedia_directory in self.multimedia_directories:
            root_filepath = os.path.join(self.configDict['LocalFolder'], multimedia_directory)

            for root, dirs, files in os.walk(root_filepath):
                directory_excluded = False

                for excluded_root in self.excluded_roots:
                    print(("excluded_root = " + excluded_root))
                    print(("root = " + root))
                    print(("excluded_root in root = " + str(excluded_root in root)))

                    if excluded_root in root:
                        directory_excluded = True

                if directory_excluded:
                    continue

                filtered_filenames = set(files) - self.excluded_files
                checksums.extend(generate_md5s(root, filtered_filenames, self.logger))

        for database_file in self.database_files:
            absolute_filepath = os.path.join(self.backup_directory, database_file)

            if not os.path.exists(absolute_filepath):
                self.logger.log('warning', 'Ignoring non-existant ht backup {0}'.format(absolute_filepath))
                continue

            relative_filepath = os.path.join('download', database_file)
            checksum = '{0}={1}'.format(relative_filepath, md5checksum.md5_for_file(absolute_filepath))
            checksums.append(checksum)
            self.logger.log('info', checksum)

        with open(self.local_checksum_filepath, 'w') as checksum_file:
            checksum_file.write('\n'.join(checksums))

    def retrieveChecksums(self, args, connection):
        """ Read the local checksums file at /usr/local/hobart/checksums.txt and return the data inside to the requester.

            request format:
                retrieveChecksums|id

            response format:
                retrieveChecksums|relative_filepath=md5_checksum,relative_filepath=md5_checksum,...|id

            error responses:
                if the checksum file /usr/local/hobart/checksums.txt does not exist:
                    error|retrieveChecksums|cannot read checksums from filesystem|id
        """
        md5s = []

        if not os.path.exists(self.local_checksum_filepath):
            raise OSError('cannot read checksums from filesystem')

        with open(self.local_checksum_filepath, 'r') as checksum_file:
            for md5_line in checksum_file:
                md5s.append(md5_line.strip('\n'))

        md5_string = ','.join(md5s)

        return md5_string

    def pushChecksums(self, args, connection):
        """ Push the local checksums file to a remote server.

            request format: pushChecksums|ip|server_filepath|username|password|id
                ip: The ip address to upload the checksums file too.
                server_filepath: The filepath on the server to upload the file too.
                username: The username to connect to the server with.
                password: The password to connect to the server with.
        """
        ip_address = args[1]
        user = args[3]
        password = args[4]
        remote_checksum_filepath = args[2] + '/checksums.txt'

        self.makeChecksums(args, connection)

        sftp_client = SFTPClient(user, password, ip_address)
        connection_result = sftp_client.connect()

        if not connection_result == 'success' and not connection_result == 'alreadyConnected':
            raise RuntimeError('could not connect to sftp server at {0}'.format(ip_address))

        sftp_client.uploadFile(remote_checksum_filepath, self.local_checksum_filepath)

    def error_message(self, args, error_message):
        """ Create a standard error message response.

            :param args: The arguments for the command that caused the error.
            :param error_message: An error description message to include.
            :return: "error|{command}|{error_message}|{id}"
        """
        command = args.pop(0)

        try:
            id = args.pop(-2)
        except IndexError:
            id = -1

        return 'error|{command}|{error}|{id}'.format(command=command, error=error_message, id=id)

    def success_message(self, args):
        """ Create a standard success message response.

            :param arg: The arugments for the command that succeeded.
            :return: "{command}|{command_arguments}|{id}|done"
        """
        command = args.pop(0)

        try:
            id = args.pop(-2)
        except IndexError:
            id = -1

        if args[:-1]:
            arguments = '|' + '|'.join(args[:-1]) + '|'
        else:
            arguments = '|'

        return '{command}{arguments}{id}|done'.format(command=command, arguments=arguments, id=id)

    def _verify_command_arguments(self, args, required_arg_number, syntax_help):
        if not len(args) == required_arg_number:
            raise TypeError('not enough arguments for {0} command, expected syntax: "{1}"'.format(args[0], syntax_help))


def bulk_sftp_download(download_function, filepaths, logger, max_retries=3, delay_period=120):
    """ Download a bulk of files, retrying if necessary any files that failed.

        :param download_function: The callable that will handle downloading the current group of files.
        :param filepaths: The list of filepaths to download.
        :param max_retries: The maximum amount of times to retry downloading failed files.
        :param delay_period: The amount of time to wait between retries (in seconds).
        :return: A list of files that did not download successfully (empty if successful).
    """
    current_retries = 1
    failed_filepaths = download_function(filepaths)

    while current_retries < max_retries and failed_filepaths:
        failed_filepaths = download_function(failed_filepaths)
        current_retries += 1
        time.sleep(delay_period)
        if failed_filepaths:
            logger.log('info', 'failed to download %d files, retrying' % len(failed_filepaths))

    return failed_filepaths


def read_checksum_file(checksum_filepath):
    """ Read a local checksum list, mapping filepaths and checksums in a dictionary.

        :param checksum_filepath: The filepath to the checksum file to read.
        :return: A dictionary[filepath: checksum] mapped from the file.
    """
    checksums = {}

    with open(checksum_filepath, 'r') as checksum_file:
        for checksum_line in checksum_file:
            filepath, checksum = checksum_line.split('=')
            checksums[filepath] = checksum

    return checksums


def find_changed_files(local_checksums, remote_checksums):
    """ Find remote files that have changed.

        :param local_checksums: The dictionary of local file checksums.
        :param remote_checksums: The dictionary of remote file checksums.
        :return: A list of filepaths that have changed.
    """
    if not local_checksums:
        return list(remote_checksums.keys())
    elif not remote_checksums:
        return list(local_checksums.keys())

    changed_files = []

    for filepath, local_checksum in list(local_checksums.items()):
        try:
            remote_checksum = remote_checksums[filepath]
        except KeyError:
            changed_files.append(filepath)
            continue

        if not local_checksum == remote_checksum:
            changed_files.append(filepath)

    for filepath in list(remote_checksums.keys()):
        try:
            local_checksum = local_checksums[filepath]
        except KeyError:
            changed_files.append(filepath)

    return changed_files


def filter_configuration_flags(filepaths, configuration_flags):
    """ Filter a list of filepaths based on given configuration flags.

        :param filepaths: The list of filepaths to filter.
        :param configuration_flags: A list of configuration flags which denote accepted filetypes.
        :return: The list of filepaths filtered by configuration flags.
    """
    filtered_filepaths = []

    for filepath in filepaths:
        for configuration_flag in configuration_flags:
            if configuration_flag == 'images' and 'system' in filepath:
                print("skipping system image")
                continue
            elif configuration_flag in filepath:
                filtered_filepaths.append(filepath)
            elif configuration_flag == 'images' and 'user' in filepath:
                filtered_filepaths.append(filepath)
            elif configuration_flag == 'images' and 'layouts' in filepath:
                filtered_filepaths.append(filepath)

    return filtered_filepaths


def generate_md5s(root, filename_list, logger=None):
    """ Generate a list of MD5 checksum strings for a list of files.

        :param root: The root of the filesystem to calculate from.
        :param filename_list: The list of filenames under the root to calculate.
        :param logger: A logger instance to write log messages to (or None).
        :return: A list of MD5 strings: "{filepath}={md5}"
    """
    md5s = []

    for filename in filename_list:
        filepath = os.path.join(root, filename)

        if 'hobart' in root:
            strip_index = root.index('hobart')
        elif 'Hobart' in root:
            strip_index = root.index('Hobart')
        else:
            strip_index = 0

        strip_index += len('hobart') + 1
        relative_filepath = root[strip_index:].rstrip('/') + '/' + filename

        if not os.path.isfile(filepath):
            if logger:
                logger.log('error', 'GenerateMD5s: Filepath {0} does not exist or is a directory, and was ignored'.format(filepath))

            continue

        md5s.append('{0}={1}'.format(relative_filepath, md5checksum.md5_for_file(filepath)))

    return md5s


def download_if_exists(remote_filepath, local_filepath, sftp_client, logger=None):
    """ Attempt to download a file, ignoring if it doesn't exist.

        :param remote_filepath: The path to the file on the server.
        :param local_filepath: The path to download the file too.
        :param sftp_client: The SFTPClient instance to use to download.
        :param logger: A Logger instance to write log messages too (or None).
        :return: False if the file does not exist, True if it downloaded successfully.
        :raises SCPError: If the file existed on the server, but did not download successfully.
    """
    try:
        sftp_client.downloadFile(remote_filepath, local_filepath)
    except SCPError as err:
        if 'The specified file does not exist' in str(err) or 'No such file or directory' in str(err):

            if logger:
                logger.log('error', "File does not exist: " + remote_filepath)

            return False
        else:
            if logger:
                logger.log('error', 'Could not download {0} to {1}'.format(remote_filepath, local_filepath))
                logger.log_exc(err)
                logger.log('error', err.data)

            raise

    return True
