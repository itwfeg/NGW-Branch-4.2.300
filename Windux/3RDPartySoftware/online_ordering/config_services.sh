#!/usr/bin/env bash

enable_mysql_file="/etc/hobart/enable_mysql"
enable_lighttpd_file="/etc/hobart/enable_lighttpd"
enable_oo_file="/etc/hobart/online_ordering"
target_folder="/var/www/"
mysql_database="/var/mysql/online_ordering"
system_image_folder="/usr/local/hobart/images/system/"

# stop hydra if it's running already
if [ -f /etc/init.d/hydra-booda ]; then
  /etc/init.d/hydra-booda stop
fi

# stop web2py if it's running already
if [ -f /etc/init.d/web2py-scgihandler ]; then
  /etc/init.d/web2py-scgihandler stop
fi

# create lighttpd, mysql, online ordering flag files
touch $enable_mysql_file $enable_lighttpd_file $enable_oo_file

# preserve any existing images and remove existing web2py
if [ -d $target_folder ]; then
  mv $target_folder/web2py/applications/OnlineOrdering/static/images /oo_images
  rm -rf "$target_folder"
fi

# copy in new web2py
mkdir -p $target_folder
cp -ar web2py $target_folder

# restore old images and copy system images in
if [ -d /images ]; then
  mv /oo_images $target_folder/web2py/applications/OnlineOrdering/static/images
fi

cp -ar $system_image_folder/categories $target_folder/web2py/applications/OnlineOrdering/static/images/
cp -ar $system_image_folder/products $target_folder/web2py/applications/OnlineOrdering/static/images/

# install init.d script(s)
cp hydra-booda /etc/init.d/
chmod +x /etc/init.d/hydra-booda

cp web2py-scgihandler /etc/init.d/
chmod +x /etc/init.d/web2py-scgihandler

# stop everything
/etc/init.d/hydra-booda stop
/etc/init.d/web2py-scgihandler stop
/etc/init.d/lighttpd stop
/etc/init.d/mysql stop > /dev/null 2>&1
sleep 5

# if there's no DB, set it up and configure it
if ! [ -d $mysql_database ]; then
    # start the DB and let it run its init process, then restart it
    # the sleeps are needed because the init scripts return before all things finish
    /etc/init.d/mysql start > /dev/null 2>&1
    sleep 5
    /etc/init.d/mysql stop > /dev/null 2>&1
    sleep 5
    kill $(/usr/bin/ps ax | grep "mysql" | awk '{print $1}') 2>/dev/null
    /etc/init.d/mysql start > /dev/null 2>&1
    sleep 5

    # configure hobart user and create OO database
    mysqladmin -u root password 'hobart'
    mysql --password=hobart -e "create user 'hobart'@'localhost' identified by 'hobart';"
    mysql -u root --password=hobart -e "create database online_ordering;"
    mysql -u root --password=hobart -e "grant all privileges on *.* to 'hobart'@'localhost';"
    mysql -u root --password=hobart -e "flush privileges;"

    # run the DB schema creation SQL file
    mysql -u root --password=hobart online_ordering < db_schema_init.sql
else
    # if there is a DB, migrate it
    /etc/init.d/mysql start > /dev/null 2>&1
    sleep 5
    
    python ./migrate_db.py
fi

# start web server and web2py
/etc/init.d/lighttpd start
/etc/init.d/web2py-scgihandler start
/etc/init.d/hydra-booda start

