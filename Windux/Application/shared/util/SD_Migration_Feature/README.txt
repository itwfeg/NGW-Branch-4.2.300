#-------------------------------------------------
# SD Migration Feature
# Author: Eric Landes
# Date: 1/24/2018
# ------------------------------------------------

The SD migration feature is a feature file which allows the user to migrate
the data from the SD card running the scale (2.x) to a second SD card (3.x). The second 
SD card must have a clean install of the scale software on it.
The feature will migrate the following data:
	1. Licenses
	2. Network Information
	3. Database
	4. Multimedia
	5. Weigher Configuration
	
HOW TO RUN:
	1. Power on scale with 2.x SD card
	2. Once the scale has powered on insert the 3.x into the secondary slot on the scale
	3. Insert the flash drive containing the feature package.tgz file
	4. Using "Supervisor Pages" >> "All Functions" >> "Upgrade vis USB" select the feature file
	5. Press "Upgrade Firmware"
	6. The feature file will copy the required files, giving feedback on the screen
	7. Upon completion of the migration the feature will prompt to update UBoot, respond to the prompt.
		NOTE: the UBoot upgrade is not needed if the new board already has the most recent version of UBoot
	8. After selecting an option either UBoot will be installed or the scale will be shutdown. If UBoot was
	   installed the user may need to manually shutdown the scale.
	9. If needed swap-out/upgrade the scale boards
	10. With the scale powered off Move the 3.x card to the new board
	11. Power on the scale and complete the firs-boot prompts 
	12. During first-boot the user will be prompted to delete the migration data from the 3.x card. This will
		remove the folder /migration 
	13. After first-boot confirm that the data has been transfered.
	NOTE: If first-boot fails to execute some of the data can still be transfered by running the feature on a
	scale running the 3.x card
	
IN DEPTH FUNCTIONALITY:

	Licenses:
	Running the feature file will copy the license data from /dev/mtd6 on the 2.x card to /migration/mtd6.
	In the first-boot section of startupSequence.py the scale will look for /migration/mtd6 and move it to
	/dev/mtdblock6 these licenses will then be applied to the scale. You can check this at "Operator Pages" >> 
	"About Scale" >> "Licenses"
	
	Network Information:
	Running the feature file will copy networking information from the 2.x card to the 3.x card. The following files
	will be copied:
	(2.x)												(3.x)
	'/etc/network/*'									'/etc/network/*'
	'/etc/resolv.conf'									'/etc/resolv.conf'
	'/etc/resolv.conf.head'								'/etc/resolv.conf.head'
	'/etc/resolv.conf.tail'								'/etc/resolv.conf.tail'
	'/etc/wpa_supplicant.conf'							'/etc/wpa_supplicant.conf'
	'/etc/hostname'										'/etc/hostname'
	'/usr/local/hobart/db/settingswizard/*'				'/usr/local/hobart/db/settingswizard/*'
	
	'/usr/local/hobart/db/settingswizard/scalesetting' Is also coppied but it is a special case. We do not want to clobber the 3.x
	version of the file because it would delete and new keys created for the 3.x application. For this file we read in the keys from the
	2.x file and apply their values to the corresponding key in the 3.x file. The scale hostname is also added to the 3.x version of the 
	scaleSettings file.
	
	The wireless and wired MAC Addresses will be written to /migration/MAC_ADDRESSES in plain text. Since most of the time the MAC Addresses
	are not needed,	we do not apply the addresses to the hardware. In the few cases where the MAC Addresses is needed the user can select not
	to delete the /migration folder, and read from the MAC_ADDRESSES file.
	
	Database:
	To backup the database we establish a connection with chobart_d and send the command TNBackupEntireDB. Chobart_d
	will then create a backup file under /etc/hobart/ called verIndData. The feature file will copy this file to 
	/etc/hobart/verIndData and /migration/verIndData on the 3.x card. After the running the 3.x card boots it will 
	detect the verIndData file and restore the database.

	Multimedia:
	Running the feature file will copy multimedia from the 2.x card to the 3.x card. It will skip default files. The following files
	will be copied:
	
	***** 2.x *****							**** items ****		      	***** 3.x *****
	usr/local/hobart/audio					audio for slideshow			usr/local/hobart/audio/
	usr/local/hobart/images				    slideshow graphics			usr/local/hobart/images/
	usr/local/hobart/images/categories		categories					usr/local/hobart/images/categories/
	usr/local/hobart/images/planograms		planograms					usr/local/hobart/images/planograms/
	usr/local/hobart/images/products		products					usr/local/hobart/images/products/
	usr/local/hobart/images/selfServe       self serve graphics     	usr/local/hobart/hobart/images/selfServe/
	usr/local/hobart/images/softkeys		softkeys					usr/local/hobart/images/softkeys/
	usr/local/hobart/images/specials		specials					usr/local/hobart/images/specials/
	usr/local/hobart/layouts				layouts & storelogo.png		usr/local/hobart/layouts/
	usr/local/hobart/playlists				playlist					usr/local/hobart/playlists/
	usr/local/hobart/system/graphics		label graphics				usr/local/hobart/system/graphics/
	usr/local/hobart/user/graphics			label graphics				usr/local/hobart/user/graphics/
	usr/local/hobart/videos					videos						usr/local/hobart/videos/
	
	Weigher Configuration:
	To backup the weigher configuration  we establish a connection with chobart_d and send the command TNreadCat3WeigherConfig.
	chobart_d will read the command and create a file called cat3weigherConfig.ht at /usr/local/hobart/. The feature file will 
	then copy this file from the 2.x card to the /usr/local/hobart/download/ directory on the 3.x card. Once the scale running
	the 3.x card has booted chobart_d will detect the file and change the weigher configuration accordingly. This make take up
	to 5 min to be detected.

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
