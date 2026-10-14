#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Filename : ad_service.py
# @Creation Date : 2019-10-16-16-56
# @Project: adservice
# @Organization: ITW Hobart, Inc.
# @AUTHOR : Scott Davis

import argparse
import schedule
import time
from ConfigParser import SafeConfigParser
import requests
import status
import ad_service_alerts
import glob
import sql
import license
import configInfo
import os
import shutil
import md5checksum
import getSerialFlashInfo
import sync
from playlist import ScalePlaylist
from playlist import SlideShowSection
import logging
import logging.handlers
import sys


class AdService(object):
    """
    Advertisement Service which interacts with a specified web service via REST API calls configured in the
    ad_service configuration file
    """

    def __init__(self,
                 configuration_file='ad_service_walmart.cfg',
                 log_filename='ad_service.log'):
        """
        Ad Service Class constructor to process configuration settings and initialize object defaults
        :param configuration_file: advertisement service configuration filename
        :param log_filename: log filename to log all advertisement service execution
        """

        # change to default ad_service installation directory
        os.chdir(os.path.join("/", "usr", "local", "hobart", "apps", "adservice"))

        parser = argparse.ArgumentParser(
            description="Service to periodically retrieve advertisements from a configured "
                        "web service and display advertisements on customer display",
            epilog="""Retrieves advertisements from the specified web service via
                   configuration file and downloads the advertisement files,
                   creates a playlist of the files, and configures the scale
                   to display the advertisement files on the GCD.  For more info            
                   contact Angel Lopez at email:angel.lopez@itwfeg.com""")

        parser.add_argument("-c", "--config-file",
                            dest="config_file",
                            required=False,
                            type=self.extant_file,
                            metavar="FILE",
                            help="configuration filename to use, if not specified %s" % configuration_file)

        parser.add_argument("-l", "--log-file",
                            dest="log_file",
                            required=False,
                            help="log filename to record, if not specified %s" % log_filename)

        args = parser.parse_args()

        if args.config_file is not None:
            configuration_file = args.config_file

        if args.log_file is not None:
            log_filename = args.log_file

        self.configuration = SafeConfigParser()
        self.configuration.read(configuration_file)
        self.megabyte = 1024 * 1024

        self.log_level = self.configuration.get('AD SERVICE', "logging_level")

        self._setup_enhanced_logging(log_filename)

        self.advertisement_server_option_menu_index = 2

        self.ad_service_feature_name = "SCALE_AD_SERVICE"
        # Feature is not licensed
        # if not self.ad_service_is_licensed():
        #     logging.error("Ad service is not licensed to operate on this scale. Service not started.")
        #     sys.exit(1)

        model_name = getSerialFlashInfo.getModelName().strip('"')
        device_id_prefix = None

        if '-7' in model_name:
            device_id_prefix = '07'
        elif '-10' in model_name:
            device_id_prefix = '10'
        elif '-14' in model_name:
            device_id_prefix = '14'
        elif '-SS' in model_name:
            device_id_prefix = '15'

        if device_id_prefix is None:
            logging.error("This scale with serial number: %s does not have a customer display. "
                          "Advertisement server will not be started due to lack of customer display "
                          "or the scale is not a self-service model.  Scale Model name is %s" %
                          (getSerialFlashInfo.getScaleSerialNumber().strip('"'),
                           model_name))
            sys.exit(1)

        self.scale_device_id = device_id_prefix + getSerialFlashInfo.getScaleSerialNumber().strip('"')

        self.images_folder = configInfo.getRootFolder() + os.path.join(os.sep, "images")
        self.videos_folder = configInfo.getRootFolder() + os.path.join(os.sep, "videos")

        self.status_alerts_filename = self.configuration.get('AD SERVICE', 'status_alerts_filename')

        self.maximum_advertising_file_size_before_warning = \
            self.configuration.getint("WEB SERVICE",
                                      'maximum_advertising_file_size_before_warning') * self.megabyte
        self.maximum_number_advertising_files_before_warning = \
            self.configuration.getint("WEB SERVICE",
                                      'maximum_number_advertising_files_before_warning')

        request_custom_header = self.configuration.get("WEB SERVICE", "request_custom_header")
        request_custom_header_tokens = request_custom_header.split(":")
        self.request_custom_header_parameters = \
            {request_custom_header_tokens[0].strip(): request_custom_header_tokens[1].strip()}
        self.connection_type = self.configuration.get("WEB SERVICE", "connection_type")
        self.get_list_of_active_files_request = self.configuration.get("WEB SERVICE",
                                                                       "get_list_of_active_files_request")
        self.get_specified_file_request = self.configuration.get("WEB SERVICE", "get_specified_file_request")

        self.verify_https_connection = self.configuration.getboolean("WEB SERVICE", 'verify_https_connection')

        self.empty_playlist_image_filename = self.configuration.get('AD SERVICE',
                                                                    'empty_playlist_image_filename')

        supported_image_file_types = self.configuration.get('AD SERVICE', "supported_image_file_types").lower()
        supported_image_file_types = supported_image_file_types.split(",")
        self.supported_image_file_types = []
        for i in xrange(0, len(supported_image_file_types)):
            self.supported_image_file_types.append(supported_image_file_types[i].strip())

        supported_video_file_types = self.configuration.get('AD SERVICE', "supported_video_file_types").lower()
        supported_video_file_types = supported_video_file_types.split(",")
        self.supported_video_file_types = []
        for i in xrange(0, len(supported_video_file_types)):
            self.supported_video_file_types.append(supported_video_file_types[i].strip())

        web_service_exclusion_list = self.configuration.get('WEB SERVICE', "ignore_file_list").lower()
        web_service_exclusion_list = web_service_exclusion_list.split(",")
        self.web_service_exclusion_list = []
        for i in xrange(0, len(web_service_exclusion_list)):
            self.web_service_exclusion_list.append(web_service_exclusion_list[i].strip())

        playlist_audio_filename = self.configuration.get("AD SERVICE", "playlist_audio_filename").strip()
        if playlist_audio_filename.upper() == "NONE":
            self.playlist_audio_filename = ''

        self.playlist_filename = "ad_service_playlist.txt"

        self.web_service_connection_timeout = self.configuration.getfloat("WEB SERVICE",
                                                                          "connection_timeout")
        self.web_service_response_timeout = self.configuration.getfloat("WEB SERVICE", "response_timeout")

        try:
            self.sync_check_time_delta = self.configuration.getint('WEB SERVICE', 'sync_check_time_delta')
        except:
            self.sync_check_time_delta = 15

        try:
            self.md5_checksum_block_size = self.configuration.getint("AD SERVICE", "md5_checksum_block_size")
        except:
            self.md5_checksum_block_size = 256 * 128

        try:
            self.playlist_media_delay = self.configuration.getint("AD SERVICE", "playlist_media_delay")
        except:
            self.playlist_media_delay = 20

        try:
            self.maximum_get_file_request_attempts = self.configuration.getint("AD SERVICE",
                                                                               "maximum_get_file_request_attempts")
        except:
            self.maximum_get_file_request_attempts = 1

        try:
            self.maximum_get_list_of_active_files_request_attempts = \
                self.configuration.getint("AD SERVICE", "maximum_get_list_of_active_files_request_attempts")
        except:
            self.maximum_get_list_of_active_files_request_attempts = 2

        try:
            self.playlist_maximum_video_files = self.configuration.getint("AD SERVICE",
                                                                          "playlist_maximum_video_files")
        except:
            self.playlist_maximum_video_files = 99

        try:
            self.playlist_maximum_image_files = self.configuration.getint("AD SERVICE",
                                                                          "playlist_maximum_image_files")
        except:
            self.playlist_maximum_image_files = 99

        self.country_code = None
        self.store_number = None

        status_code = self._get_scale_request_parameters()

        if status_code != status.OK:
            logging.info(
                "Could not retrieve country code or store number from database using configuration file settings.")

        if self.configuration.has_option("AD SERVICE", 'country_code'):
            logging.info("Setting country code to value in configuration file")
            self.country_code = self.configuration.get("AD SERVICE", 'country_code')

        if self.configuration.has_option("AD SERVICE", 'store_number'):
            logging.info("Setting store number to value in configuration file")
            self.store_number = self.configuration.get("AD SERVICE", 'store_number')

        self.ad_service_alerts = ad_service_alerts.Alerts(self.status_alerts_filename)
        self.database_filename = self._get_database_filename()

    def ad_service_is_licensed(self):

        licensed = False

        try:
            license_buffer = license.readFlashKeyBuffer()
            licensed = license.isProductLicensed(license_buffer, self.ad_service_feature_name)
        except:
            pass

        return licensed

    def extant_file(self, x):
        """
        'Type' for argparse - checks that file exists but does not open.
        """
        if not os.path.exists(x):
            # Argparse uses the ArgumentTypeError to give a rejection message like:
            # error: argument input: x does not exist
            raise argparse.ArgumentTypeError("{0} does not exist".format(x))
        return x

    def _get_database_filename(self):
        """
        Gets the name of the sqlite database on the uSD card in the scale
        :return: full path filename of sqlite db
        """
        db_file_pattern = None
        try:
            db_file_pattern = configInfo.getRootFolder() + os.path.join(os.sep, "db", "scale", "dbfiles", "dbft?.db")
            db_filename = glob.glob(db_file_pattern)[0]
        except:
            db_filename = None
            logging.error("Can not find scale database with file pattern: %s" % db_file_pattern)
        return db_filename

    def _get_scale_request_parameters(self):
        """
        Gets the store id and country code from the database
        :return: status of the database select statements to get store id and country code
        """
        status_code = status.ERROR

        outcome, result = self._execute_sql_select_statement(
            "SELECT coValue FROM configs WHERE coName='storeId'")
        if outcome == status.OK:
            self.store_number = result[0]['coValue'].upper()
            outcome, result = self._execute_sql_select_statement(
                "SELECT coValue FROM configs WHERE coName like '%CountryCode'")
            if outcome == status.OK:
                self.country_code = result[0]['coValue'].upper()
                status_code = status.OK
            else:
                logging.info("Country code could not be retrieved from scale database")
        else:
            logging.info("Store number could not be retrieved from scale database")

        return status_code

    def _execute_sql_select_statement(self, select_command):
        """
        Executes a sql select statement on the scale sqlite database
        :param select_command: sql select command to execute
        :return: status code of executing the statement and select query result in a tuple
        """
        status_code = status.ERROR
        result = None
        db_filename = self._get_database_filename()

        if db_filename is None:
            logging.error("Can not find database file")
        else:
            outcome, result = sql.execute(db_filename, select_command)
            if outcome == 'success':
                status_code = status.OK

        return status_code, result

    def _setup_enhanced_logging(self,
                                filename,
                                maximum_file_size_bytes=5 * 1024 * 1024,
                                backup_logs=5):
        """
        Setup the python default logger to include additional elements in logged message
        as well as log backups which rotate when the maximum log size is reached
        :param filename: filename of the log file
        :param maximum_file_size_bytes: maximum size of log file before it is closed and put in rotation
        :param backup_logs: maximum number of backup logs maintained
        :return: None
        """

        log_levels = {'DEBUG': logging.DEBUG,
                      'INFO': logging.INFO,
                      'WARNING': logging.WARNING,
                      'ERROR': logging.ERROR,
                      'CRITICAL': logging.CRITICAL}

        try:
            logging_level = log_levels[self.log_level.upper()]
        except:
            logging_level = logging.DEBUG

        logging.basicConfig(filename=filename,
                            filemode='w',
                            format='[%(asctime)s:%(levelname)s:%(module)s:%(funcName)s:%(lineno)d]-%(message)s',
                            level=logging_level,
                            datefmt='%m/%d/%Y %I:%M:%S %p')

        logger = logging.getLogger('EnhancedLogger')

        # Add the log message handler to the logger
        handler = logging.handlers.RotatingFileHandler(
            filename, maxBytes=maximum_file_size_bytes, backupCount=backup_logs)

        logger.addHandler(handler)

    def scale_idle_time_promotions_set_to_advertisement_server(self):
        """
        Determines if the scale is to produce idle time promotions via the ad service
        :return: True or False on whether the ad_service should produce playlists and ultimately display on GCD
        """

        status_boolean = False

        outcome, result = self._execute_sql_select_statement(
            "SELECT coValue FROM configs WHERE coName='gcdIdleAdvertType'")

        if outcome == status.OK:

            logging.debug("Scale idle time promotions = %s" % str(result[0]['coValue']))

            # check to see if option menu setting for idle time promotions on scale
            # customer promotions tab is Advertisement Server which is index 2
            if result[0]['coValue'].upper() == str(self.advertisement_server_option_menu_index):
                status_boolean = True

        else:
            logging.info("Can not retrieve customer idle time promotions method from database")

        return status_boolean

    def process_advertisements(self):
        """
        Retrieves advertisments from the specified web service via configured REST API calls and
        downloads the advertisment files, creates a playlist of the files, and configures the scale
        to display adverts on the GCD
        :return: None
        """

        logging.debug("Processing advertisements job started")

        # since this was the first time advertisements are to be processed
        # we need to let chobart_d know that this playlist needs processed
        # current_ad_playlist = ScalePlaylist(self.playlist_filename,
        #                                    videos={},
        #                                    slide_shows=[],
        #                                    section_order=[])

        # send tag to chobart_d which will cause gcd to read playlist the first time
        # current_ad_playlist.set_active()

        # process advertisement media from web service if configured to do so on the scale
        if self.scale_idle_time_promotions_set_to_advertisement_server():

            # get the advertisement media from web service
            status_code, advertisements = self.get_advertisements()

            if status_code == status.OK:

                logging.debug("Web service request to get advertisements was successful")

                if advertisements['syncable']:

                    logging.debug("Web service is open to synchronization")

                    total_spaced_used_by_requested_files = 0
                    total_number_of_advertisement_files = 0

                    advertisement_filenames = []

                    playlist_needs_updated = False

                    for advertisement in advertisements['pvFiles']:

                        filename = advertisement['name']

                        if filename not in self.web_service_exclusion_list:

                            status_code, local_directory = self.get_directory_for_file_type(filename)

                            if status_code == status.OK:

                                directory_number = str(advertisement['directory'])

                                checksum = advertisement['hash']

                                advertisement_filenames.append({'name': filename,
                                                                'directory': local_directory,
                                                                'type': self.get_file_media_type(filename),
                                                                'checksum': checksum})

                                file_size = 0

                                # if the advertisement file does not exist on scale
                                if not os.path.exists(os.path.join(local_directory, filename)):

                                    logging.debug(
                                        "Web service file named %s in directory %s does not exist "
                                        "on scale requesting download" %
                                        (filename, directory_number))

                                    # request the file and store it on scale
                                    status_code, file_size = self.get_advertising_file(directory_number,
                                                                                       filename,
                                                                                       checksum)

                                    if status_code == status.OK:
                                        playlist_needs_updated = True
                                        total_number_of_advertisement_files += 1
                                    else:
                                        logging.error("Error retrieving file %s in directory %s "
                                                      "from web service" % (filename, directory_number))

                                # else the file does exist on the scale
                                else:

                                    # compare the checksum of the file with the file that is named the same
                                    # on the scale
                                    if md5checksum.md5_for_file(
                                            os.path.join(local_directory, filename)) != checksum:

                                        # checksums are different so download new version of the file that
                                        # has the same name on the scale
                                        logging.debug(
                                            "Web service file named %s in directory %s exists on scale but "
                                            "checksum is different, requesting download" %
                                            (filename, directory_number))

                                        # request the file
                                        status_code, file_size = self.get_advertising_file(directory_number,
                                                                                           filename,
                                                                                           checksum)

                                        if status_code == status.OK:
                                            playlist_needs_updated = True
                                            total_number_of_advertisement_files += 1
                                        else:
                                            logging.error("Error retrieving file %s in directory %s "
                                                          "from web service" % (filename, directory_number))

                                    # file just requested has the same checksum as the file on the scale
                                    # therefore no need to download
                                    else:
                                        # compute file size
                                        file_size = os.path.getsize(os.path.join(local_directory, filename))

                                if status_code == status.OK:

                                    # compute the total space used by all files downloaded to scale
                                    total_spaced_used_by_requested_files += file_size

                                    if file_size > self.maximum_advertising_file_size_before_warning:
                                        logging.warning("Downloaded advertisement file in directory %s named: %s "
                                                        "with size %d is greater than warning threshold of %d in "
                                                        "configuration file." %
                                                        (directory_number, filename, file_size,
                                                         self.maximum_advertising_file_size_before_warning))

                                else:
                                    logging.error("Could not retrieve web service file in directory %s, named %s" %
                                                  (directory_number, filename))

                            else:
                                logging.warning(
                                    "Web service responded with advertisement that does not have supported "
                                    "file extension: %s, file not added to playlist" % filename)

                        else:
                            logging.info("Web service responded with a file named %s that is in the configured "
                                         "exclusion list" % filename)

                    if total_number_of_advertisement_files > self.maximum_number_advertising_files_before_warning:
                        logging.warning("Total number of advertisement files downloaded %d is greater than "
                                        "maximum number before warning threshold of %d in configuration file." %
                                        (total_number_of_advertisement_files,
                                         self.maximum_number_advertising_files_before_warning))

                    logging.info("Total amount of storage space used by web service playlist files is %f MB" %
                                 (float(total_spaced_used_by_requested_files) / self.megabyte))

                    # load the last playlist file that was last written to get the names of the
                    # files in current playlist before we write the new playlist with current list
                    # of files from web service request we just got
                    current_ad_playlist = ScalePlaylist(self.playlist_filename,
                                                        videos={},
                                                        slide_shows=[],
                                                        section_order=[])

                    # load the last saved playlist file
                    current_ad_playlist.load_file()

                    # create a new playlist instance to create a playlist for current advertisements to process
                    new_ad_playlist = ScalePlaylist(self.playlist_filename,
                                                    videos={},
                                                    slide_shows=[],
                                                    section_order=[])

                    video_section_index = 1
                    slide_show_section_index = 1
                    playlist_section_order = []
                    maximum_number_images_exceeded = False
                    maximum_number_videos_exceeded = False

                    for i in xrange(0, len(advertisement_filenames)):

                        section_name = None
                        if advertisement_filenames[i]['type'] == 'video':
                            if video_section_index <= self.playlist_maximum_video_files:
                                section_name = advertisement_filenames[i]['name']
                                new_ad_playlist.add_section(section_name=section_name)
                                video_section_index += 1
                            else:
                                maximum_number_videos_exceeded = True

                        elif advertisement_filenames[i]['type'] == 'image':
                            if slide_show_section_index <= self.playlist_maximum_image_files:
                                section_name = 'slideshow%d' % slide_show_section_index
                                slide_show_section = SlideShowSection(section_name=section_name, media=[])
                                slide_show_section.add_media(advertisement_filenames[i]['name'])
                                slide_show_section.set_audio(self.playlist_audio_filename)
                                slide_show_section.set_delay(self.playlist_media_delay)
                                new_ad_playlist.add_section(section_name=section_name,
                                                            slide_show=slide_show_section)
                                slide_show_section_index += 1
                            else:
                                maximum_number_images_exceeded = True

                        else:
                            logging.error("Unknown file type in web service file list")

                        if section_name is not None:
                            playlist_section_order.append(section_name)
                        else:
                            logging.error("Section name not assigned due to unsupport file type from web service.")

                    if maximum_number_videos_exceeded:
                        logging.warning("Maximum number of videos in web service request exceeded. "
                                        "Playlist truncated.")

                    if maximum_number_images_exceeded:
                        logging.warning("Maximum number of images in web service request exceeded. "
                                        "Playlist truncated.")

                    # get all the video and image filenames in the current playlist
                    current_ad_playlist_files = current_ad_playlist.get_media_files()

                    # get all the video and image filenames in the new playlist about to be written
                    new_ad_playlist_files = new_ad_playlist.get_media_files()

                    # check the last written playlist files and if any are not in the new playlist to be
                    # written then remove the file from the local storage device
                    # for each image or video file in the current playlist
                    for ad_playlist_file in current_ad_playlist_files:

                        # if the current playlist media file is not in the new playlist
                        if ad_playlist_file not in new_ad_playlist_files:

                            status_code, directory = self.get_directory_for_file_type(ad_playlist_file)

                            # if the media file currently exists on uSD card
                            if os.path.exists(os.path.join(directory,
                                                           ad_playlist_file)):

                                logging.debug(
                                    "Removing file named %s in previous playlist that "
                                    "is not found in new playlist" % os.path.join(
                                        directory,
                                        ad_playlist_file))

                                # remove the media file
                                os.remove(os.path.join(directory,
                                                       ad_playlist_file))

                                sync.sync()

                                playlist_needs_updated = True

                            else:
                                logging.warning(
                                    "File named %s in current playlist but not found in scale storage." %
                                    os.path.join(
                                        directory,
                                        ad_playlist_file))

                    # if there are no playlist files in what was just received from the web service
                    if not new_ad_playlist_files:
                        # generate a default playlist with the empty playlist image
                        section_name = 'slideshow1'
                        slide_show_section = SlideShowSection(section_name=section_name, media=[])
                        slide_show_section.add_media(self.empty_playlist_image_filename)
                        slide_show_section.set_audio(self.playlist_audio_filename)
                        slide_show_section.set_delay(self.playlist_media_delay)
                        new_ad_playlist.add_section(section_name=section_name, slide_show=slide_show_section)
                        playlist_section_order.append(section_name)

                        status_code, directory = self.get_directory_for_file_type(self.empty_playlist_image_filename)

                        if status_code == status.OK:
                            if os.path.exists(os.path.join(directory, self.empty_playlist_image_filename)):
                                os.remove(os.path.join(directory, self.empty_playlist_image_filename))
                            shutil.copyfile(self.empty_playlist_image_filename,
                                            os.path.join(directory, self.empty_playlist_image_filename))

                        playlist_needs_updated = True

                    # if currently there is no ad service playlist file on the scale but a new playlist is being
                    # processed that has files then create a new playlist
                    if not current_ad_playlist_files and new_ad_playlist_files:
                        playlist_needs_updated = True

                    # if any playlist changes have occurred from above
                    if playlist_needs_updated:

                        logging.debug("Playlist named %s needs to be updated." % self.playlist_filename)

                        # set the section order of the playlist to match order from web service
                        new_ad_playlist.set_section_order(playlist_section_order)

                        # save the new playlist file
                        new_ad_playlist.save_file()

                        logging.debug("New playlist named %s created" % self.playlist_filename)

                        # send via socket or write tag file to chobart_d so gcd re-reads playlist
                        # and starts playing new playlist files
                        new_ad_playlist.set_active()

                        logging.debug("Playlist named %s activated" % self.playlist_filename)

                    else:
                        logging.debug("No playlist created since no files have changed via web service")

                else:
                    logging.debug("Web Service not available for synchronization")

            else:
                logging.info("Web Service responded with error")

        else:
            logging.debug("Scale idle time promotions on customer promotions tab "
                          "is not set to advertisement server")

    def get_advertisements(self):
        """
        Request web service to provide list of all advertisement media filenames with checksum info to download
        :return: status code of the request and the advertisement filenames info tuple
        """
        status_code = status.ERROR
        advertisements = None
        file_request_url = self.get_list_of_active_files_request.replace("${country_code}", self.country_code)
        file_request_url = file_request_url.replace("${store_number}", self.store_number)
        file_request_url = file_request_url.replace("${device_id}", self.scale_device_id)

        logging.debug("Requesting advertisement listing from web service")

        get_advertising_file_attempts = 0
        request_succeeded = False

        while (get_advertising_file_attempts < self.maximum_get_list_of_active_files_request_attempts) \
                and (not request_succeeded):

            try:
                response = requests.get(url=file_request_url,
                                        headers=self.request_custom_header_parameters,
                                        timeout=(self.web_service_connection_timeout,
                                                 self.web_service_response_timeout),
                                        verify=self.verify_https_connection)

                if response.status_code == self.ad_service_alerts.WEB_SERVICE_REQUEST_SUCCESS:
                    advertisements = response.json()
                    logging.debug("Web service request for advertisement listing was successful")
                    status_code = status.OK
                    request_succeeded = True
                else:
                    logging.error("Web service request for advertisement listing returned status code: " +
                                  self.ad_service_alerts.lookup(response.status_code).message)

            except requests.Timeout:
                logging.error("Web service response timeout exceeded.")

            except requests.ConnectionError:
                logging.error("Web service connection timeout exceeded.")

            get_advertising_file_attempts += 1

        return status_code, advertisements

    def get_file_media_type(self, filename):
        """
        Determines the media type of the filename based on file extension
        :param filename: filename to analyze
        :return: media type of the file (image or video)
        """
        file_type = None
        if os.path.splitext(filename)[1][1:].lower() in self.supported_image_file_types:
            file_type = 'image'
        elif os.path.splitext(filename)[1][1:].lower() in self.supported_video_file_types:
            file_type = 'video'
        else:
            logging.error("Unrecognized file type for filename %s" % filename)
        return file_type

    def get_directory_for_file_type(self, filename):
        """
        Determines the correct download directory on scale to place file based on the filename extension
        :param filename: filename to analyze
        :return: the local scale directory to store the file when downloaded
        """
        local_directory = None
        status_code = status.ERROR
        file_type = self.get_file_media_type(filename)
        if file_type is not None:
            if file_type == 'image':
                local_directory = self.images_folder
                status_code = status.OK
            elif file_type == 'video':
                local_directory = self.videos_folder
                status_code = status.OK
        else:
            logging.error("Can not detect storage folder for file %s due to unsupported file type" % filename)
        return status_code, local_directory

    def get_advertising_file(self, directory_number, filename, checksum):
        """
        Retrieves the specified advertisement file from the web service based on directory number and filename
        with additional check of the specified checksum once downloaded to scale to make sure the checksum
        matches that computed by the web service
        :param directory_number: directory number of the path to the file on the web service to retrieve
        :param filename: filename on the web service to retrieve
        :param checksum: checksum of the file as computed by the web service and used to compare against resultant
        file downloaded to the scale
        :return: status code of the web service request and the size of the file downloaded in a tuple
        """
        status_code = status.ERROR
        file_size = 0
        file_request_url = self.get_specified_file_request.replace("${directory_number}", directory_number)
        file_request_url = file_request_url.replace("${filename}", filename)

        logging.debug("Requesting advertising file %s in directory %s" % (filename, directory_number))

        get_advertising_file_attempts = 0
        file_downloaded = False

        while (get_advertising_file_attempts < self.maximum_get_file_request_attempts) and (not file_downloaded):

            try:

                response = requests.get(url=file_request_url,
                                        stream=True,
                                        headers=self.request_custom_header_parameters,
                                        timeout=(self.web_service_connection_timeout,
                                                 self.web_service_response_timeout),
                                        verify=self.verify_https_connection)

                if response.status_code == self.ad_service_alerts.WEB_SERVICE_REQUEST_SUCCESS:

                    logging.debug("Advertising file request for file %s in directory %s succeeded." %
                                  (filename, directory_number))

                    status_code, local_directory = self.get_directory_for_file_type(filename)

                    if status_code == status.OK:

                        local_filename = os.path.join(local_directory, filename)

                        # always remove an existing file on scale to write new version of the file if exists
                        if os.path.exists(local_filename):
                            os.remove(local_filename)

                        try:

                            # pull the advertisement binary data from the raw request response stream
                            # and save it to a file on the scale uSD card
                            with open(local_filename, 'wb') as content:
                                shutil.copyfileobj(response.raw, content)

                            # sync the RAM filesystem to the uSD card
                            sync.sync()

                            file_size = os.path.getsize(local_filename)

                            if md5checksum.md5_for_file(local_filename,
                                                        block_size=self.md5_checksum_block_size) == checksum:
                                status_code = status.OK
                                file_downloaded = True

                            else:
                                logging.error("File downloaded from webservice to scale does not have the "
                                              "correct checksum.  File transmission error.")

                        except:

                            logging.error("File %s could not be downloaded." % local_filename)

                            if os.path.exists(local_filename):
                                # remove any corrupt file if it exists
                                os.remove(local_filename)

                    else:
                        logging.error("Web service file is not a supported file type.")

                else:
                    logging.error("Web service request for file returned status code: " +
                                  self.ad_service_alerts.lookup(response.status_code).message)

            except requests.Timeout:
                logging.error("Web service response timeout exceeded.")

            except requests.ConnectionError:
                logging.error("Web service connection timeout exceeded.")

            get_advertising_file_attempts += 1

        return status_code, file_size

    def schedule_recurring_job(self):
        """
        Schedules a recurring job to process advertisements based on the sync_check_delta configuration setting
        :return: None
        """
        schedule.every(self.sync_check_time_delta).minutes.do(self.process_advertisements)

    def run(self):
        """
        Scheduler that run a process advertisement job every N minutes based on sync_check_delta setting in
        ad_service configuration file
        :return: None
        """
        # run initial job to process advertisements
        self.process_advertisements()
        # schedule the same job to run every N minutes
        self.schedule_recurring_job()
        while True:
            schedule.run_pending()
            time.sleep(1)


# Instantiate the AD Server
if __name__ == "__main__":
    ad_service = AdService()
    ad_service.run()
