#!/bin/bash
#

#cd /$1 >/dev/null

#
#Checks if dir already exists
#
DoesUserExist() {
	egrep "^$1" /etc/passwd >/dev/null
	if [ $? -eq 0 ]; then
		echo "User $1 already exists!"
		return 1
	else
		return 0
	fi
}


#
#Creates a dir
#
makeDir() {
	if [ ! -d $1 ]; then
		echo "Creating Dir:$1"
		mkdir $1
	fi
	if [ -d $1 ]; then
		echo "Setting perimission for Dir:$1..."
		chmod 774 $1	
		if [ $? -ne 0 ]; then
			echo "Unable to set appropriate permissions for the directory."
			return 1
		fi
	else
		echo "Error:Directory does not exist."
		return 1
	fi
	return 0
}

#
#set specified permission for the user
#
setPerm() {
echo u=$user_perm, g=$grp_perm, o=$others_perm
	umask u=$user_perm,g=$grp_perm,o=$others_perm
	if [ $? -ne 0 ]; then
		echo "Unable to set mask. Existing."
		exit 4
	fi
}

#
#Proceed only if executed as superuser
#
if [ $(id -u) -eq 0 ]; then
#	read -p "Enter username : " username
#	read -s -p "Enter password : " password 
#
#One by one creates user accounts making sure the account
#doesn't already exist.  Also add a user group 'hob'.
#All user would belong to this group in addition to others.
#We'll use various groups to give access to files and dirs
#appropriately.
#
	common_grp=hob
	op_uname=Operator
	addgroup $common_grp
	if [ $? -ne 0 ]; then
		echo "WARNING: Unable to add group.  Setup will continue but will not complete properly."
	fi
	uname=HobartService
	for i in 1 2 3; do
		echo $i
		echo

		DoesUserExist $uname
		if [ $? -eq 0 ]; then
			echo "Adding user $uname..."
			echo "You may hit enter for following prompts to accept defualt values."
			echo
			case $i in
				1)
					adduser --force-badname $uname --ingroup root
					usermod -aG $common_grp $uname 

					if [ ! -d /home/$uname ]; then
						echo "User home directory not created. Exiting."
						exit 5
					fi
					chmod 0750 /home/$uname


					uname=$op_uname
				;;
				2)
					adduser --force-badname $uname 
					if [ ! -d /home/$uname ]; then
						echo "User home directory not created. Exiting."
						exit 5
					fi
					#
					#modify the init file of the limited user account to allow 
					#access to all future files to the group.  Admin account also 
					#belongs to the group of the limited user.
	 				#
					echo "umask u=rwx,g=rwx,o=r">>/home/$uname/.bash_aliases
					usermod -aG $common_grp $uname 
					chmod 0750 /home/$uname
					uname=Admin
				;;
				3)
					#
					#script assumes the group name assigned by default happens 
					#to be the same as the user name.
					#
					adduser --force-badname $uname --ingroup admin
					usermod -aG Operator,$common_grp $uname 
					if [ ! -d /home/$uname ]; then
						echo "User home directory not created. Exiting."
						exit 5
					fi
					chmod 0750 /home/$uname
				;;
			esac
		#
		#else executed if a particular user already exists
		#
		else
			case $i in
				1)
					uname=Operator
				;;
				2)
					uname=Admin
				;;
			esac
		fi
	done

	#
	#Create hobart specific directories and set all
	#permissions appriopriately.  We'll first begin
	#with setting up directories which should be 
	#accessible by Admin account, we'll then procceed
	#to dirs which limited user would have access to
	#as well.
	#
	my_name=$(whoami)
	if [ $? -ne 0 ]; then
		echo "Unable to retrieve user name. Existing..."
		exit 3
	fi
echo "My user name is $my_name."

	cur_grp=$(id -nG|sed 's/$common_grp//'|sed 's/[ ]* /,/g')
	if [ $? -ne 0 ]; then
		echo "Unable to retrieve current group listing. Exiting!"
		exit 4
	fi

echo "My groups $cur_grp."

	usermod -G root,admin $my_name 
	if [ $? -ne 0 ]; then
		echo "Unable to add user to admin group. Existing."
		exit 4
	fi
echo "my new group $(id -nG $my_name)"
	
	user_perm=rwx
	grp_perm=rwx
	others_perm=r
	setPerm $user_perm, $grp_perm, $others_perm

	echo "Creating directory structure..."
	hob_dir=/var/log/hobart
	#for j in 1 2; do
	for (( j=0; j<2; j++ )); do
		makeDir $hob_dir
		if [ $? -ne 0 ]; then
			exit 4
		fi
		case $j in
			0)
				hob_dir=/var/www
			;;
		esac
	done

	usermod -G $cur_grp,$common_grp $my_name 
echo "Group listing now are $(id -nG $my_name)"

	user_perm=rwx
	grp_perm=rwx
	others_perm=r
	setPerm $user_perm, $grp_perm, $others_perm
						
	hob_dir=/etc/hobart
	for (( j=0; j<11; j++ )); do
		makeDir $hob_dir
		if [ $? -ne 0 ]; then
			exit 4
		fi

		case $j in
			0)
				read -p "Enter path to directories which contains files to be copied to $hob_dir directory:" path 
				cp -r $path/* $hob_dir
				if [ $? -ne 0 ]; then
					echo "Warning: Failed to copy files into $hob_dir."
					exit 4
				fi
#
#give full directory access to user and group (for all sub dirs as well)
#and rw access to the files within.Others can only read
#
				chmod -R ug=rwX,o=r $hob_dir
				chgrp -R $common_grp $hob_dir

				hob_dir=/usr/local/hobart
			;;
			1)
				chgrp -R $common_grp $hob_dir
				hob_dir=/usr/local/hobart/system
			;;
			2)
				chgrp -R $common_grp $hob_dir
				hob_dir=/usr/local/hobart/system/fonts
			;;
			3)
				read -p "Enter path to directories which contains files to be copied to $hob_dir directory:" path 

				cp -r $path/* $hob_dir
				if [ $? -ne 0 ]; then
					echo "Warning: Failed to copy files into $hob_dir."
					exit 4
				fi

				chmod -R ug=rwX,o=r $hob_dir
				chgrp -R $common_grp $hob_dir

				hob_dir=/usr/local/hobart/system/flashkeygfx
			;;
			4)
				read -p "Enter path to directories which contains files to be copied to $hob_dir directory:" path 

				cp -r $path/* $hob_dir
				if [ $? -ne 0 ]; then
					echo "Warning: Failed to copy files into $hob_dir."
					exit 4
				fi

				chmod -R ug=rwX,o=r $hob_dir
				chgrp -R $common_grp $hob_dir
				hob_dir=/usr/local/hobart/system/graphics
			;;
			5)
				read -p "Enter path to directories which contains files to be copied to $hob_dir directory:" path 

				cp -r $path/* $hob_dir
				if [ $? -ne 0 ]; then
					echo "Warning: Failed to copy files into $hob_dir."
					exit 4
				fi

				chmod -R ug=rwX,o=r $hob_dir
				chgrp -R $common_grp $hob_dir

				hob_dir=/usr/local/hobart/user
			;;
			6)
				chgrp -R $common_grp $hob_dir
				hob_dir=/usr/local/hobart/user/fonts
			;;
			7)
				read -p "Enter path to directories which contains files to be copied to $hob_dir directory:" path 

				cp -r $path/* $hob_dir
				if [ $? -ne 0 ]; then
					echo "Warning: Failed to copy files into $hob_dir."
					exit 4
				fi

				chmod -R ug=rwX,o=r $hob_dir
				chgrp -R $common_grp $hob_dir

				hob_dir=/usr/local/hobart/user/graphics
			;;
			8)
				read -p "Enter path to directories which contains files to be copied to $hob_dir directory:" path 

				cp -r $path/* $hob_dir
				if [ $? -ne 0 ]; then
					echo "Warning: Failed to copy files into $hob_dir."
					exit 4
				fi

				chmod -R ug=rwX,o=r $hob_dir
				chgrp -R $common_grp $hob_dir

				hob_dir=/lib/firmware/hobart
			;;
			9)
				read -p "Enter path to directories which contains files to be copied to $hob_dir directory:" path 

				cp -r $path/* $hob_dir
				if [ $? -ne 0 ]; then
					echo "Warning: Failed to copy files into $hob_dir."
					exit 4
				fi

				chmod -R ug=rwX,o=r $hob_dir
				chgrp -R $common_grp $hob_dir

				hob_dir=/home/$op_uname/hlx
			;;
		esac
	done
	read -p "Enter path to directories which contains files to be copied to $hob_dir directory:" path 

	cp -r $path/* $hob_dir
	if [ $? -ne 0 ]; then
		echo "Warning: Failed to copy files into $hob_dir."
		exit 4
	fi

	chmod -R ug=rwX,o=r $hob_dir
	chgrp -R $common_grp $hob_dir

	
#
#Daemon
#
	echo "Setting up daemon's permissions"
	daemon_file=chobart_d
	daemon_path=/usr/bin
	if [ -f $daemon_file ]; then
		read -p "Enter path to directories which contains the daemon:" path 
		cp $path/$daemon_file $daemon_path 
		if [ $? -ne 0 ]; then
			echo "Warning: Failed to copy files into $daemon_path."
			exit 4
		fi
	sleep 10
		chmod 4774 $daemon_path/$daemon_file	
		chmod u+s $daemon_path/$daemon_file	
		if [ $? -ne 0 ]; then
			echo "chmod failed"
	else
	echo "$daemon_path/$daemon_file perm set"
		fi

		chgrp -R $common_grp $daemon_path/$daemon_file
	else
		echo "Failed to setup daemon's permissions.  File does not exist."
	fi
#
#Setup the firewall.
#

#Allow established sessions to receive traffic
#
#
#Allow all inbound web traffic
#
iptables -A INPUT -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT
iptables -A INPUT -p tcp --dport 80 -j ACCEPT
#
#Allow HTTP traffic on port 6000 for hlx application
#
iptables -A INPUT -p tcp --dport 6000 -j ACCEPT
#
#Allow ftp traffic on port 20 and 21
#
modprobe ip_conntrack
modprobe ip_conntrack_ftp 
iptables -A INPUT -p tcp -s 0/0 --sport 1024:65535 --dport 21 -m state --state NEW,ESTABLISHED -j ACCEPT
iptables -A OUTPUT -p tcp --sport 21 -d 0/0 --dport 1024:65535 -m state --state ESTABLISHED -j ACCEPT

iptables -A INPUT -p tcp -s 0/0 --sport 1024:65535 --dport 1024:65535 -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A OUTPUT -p tcp --sport 1024:65535 -d 0/0 --dport 1024:65535 -m state --state ESTABLISHED -j ACCEPT

iptables -A OUTPUT -p tcp --sport 20 -d 0/0 --dport 1024:65535 -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A INPUT -p tcp -s 0/0 --sport 1024:65535 --dport 20 -m state --state ESTABLISHED -j ACCEPT

#iptables -A INPUT -p tcp -m state --state NEW -m tcp --dport 21 -j ACCEPT
#iptables -A INPUT -p tcp -m state --state NEW -m tcp --dport 20 -j ACCEPT

#
#Allow SMTP traffic on port 25
#
#XXXXXXXXXXXXXXXXXXXXXX
#Block all other traffic
#
iptables -A INPUT -j DROP


iptables-save

echo "#Load the firewall rules at startup" >> /etc/.bashrc
echo "iptables-restore" >> /etc/.bashrc
else
	echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
	echo "This script may be executed as root only. Terminating!"
	echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
	exit 2
fi

























#		pass=$(perl -e 'print crypt($ARGV[0], "password")' $password)
#		useradd -m -p $pass $username
#		[ $? -eq 0 ] && echo "User has been added to system!" || echo "Failed to add a user!"




#sudo adduser HobartService admin 
#if [ $? -eq 0 ] 
#then
#	echo "Bismillah"
#	exit
#fi




#if  [ $# -eq 0 ]

#if  [ $# -eq 0 ]
#then
#        echo "Please provide the starting ip address and "
#        echo "the count of ip address to be tested"
#        exit 1
#fi

#for ip in $(seq $2 `expr $2 + $3`); do
#        ping -c 1 $1.$ip > /dev/null
##       [ $? -eq 0 ] && echo "$1.$ip UP" || :
#        if [ $? -eq 0 ]; then
#                if ssh $4@$1.$ip
#                then
#                        exit 0
#                fi
#        else
#                echo "-ip address $1.$ip not found!"
#        fi
#done


