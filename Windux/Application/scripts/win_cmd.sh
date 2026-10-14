#!/bin/bash

win=$2
export CLASSPATH="/usr/lib/jvm/java-6-sun-1.6.0.20/jre/lib/rt.jar:/usr/lib/jvm/java-6-sun-1.6.0.20/jre/lib/resources.jar:/usr/lib/jvm/java-6-sun-1.6.0.20/jre/lib/plugin.jar:/usr/lib/jvm/java-6-sun-1.6.0.20/jre/lib/management-agent.jar:/usr/lib/jvm/java-6-sun-1.6.0.20/jre/lib/jsse.jar:/usr/lib/jvm/java-6-sun-1.6.0.20/jre/lib/jce.jar:/usr/lib/jvm/java-6-sun-1.6.0.20/jre/lib/javaws.jar:/usr/lib/jvm/java-6-sun-1.6.0.20/jre/lib/deploy.jar:/usr/lib/jvm/java-6-sun-1.6.0.20/jre/lib/charsets.jar:/usr/lib/jvm/java-6-sun-1.6.0.20/jre/lib/alt-rt.jar:.:/usr/lib/jvm/java-6-sun-1.6.0.20/jre/lib/"
export HOME="/home/hlx"
export HUSHLOGIN="FALSE"
export JAVA_HOME="/usr/lib/jvm/java-6-sun-1.6.0.20"
export JDK_HOME="/usr/lib/jvm/java-6-sun-1.6.0.20"
export LANG="en_US.UTF-8"
export LESSCLOSE="/usr/bin/lesspipe %s %s"
export LESSOPEN="| /usr/bin/lesspipe %s"
export LOGNAME="hlx"
export MAIL="/var/mail/hlx"
export OLDPWD
export PATH="/usr/lib/jvm/java-6-sun-1.6.0.20/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games"
export PWD="/home/hlx"
export SHELL="/bin/bash"
export SHLVL="1"
export SPEECHD_PORT="7560"
export TERM="linux"
export USER="hlx"
export XDG_SESSION_COOKIE="a77f9d6170a5bc6d09514c6b4c2102ee-1281634318.76046-513277148"
export DISPLAY=":0.0"

if [ -z $2 ]; then
    win="HLX Scale" 
fi

found=`export DISPLAY=:0.0;wmctrl -l|grep $win|cut -c-10`
if [ "$1" = "find" ]; then
    # if we find the window we create a file so that PAL
    # can determine if this window exists.
    if [ -z "$found" ]; then
        if [ -e "/tmp/$win" ]; then
            rm "/tmp/$win"
        fi
    else
        echo "found $win @ $found"
        touch "/tmp/$win"
    fi
elif [ "$1" = "show" ]; then
    # send this window to the other desktop
    if [ -z "$found" ]; then
        echo "Error show but no window $win"
    else
        /usr/bin/wmctrl -i -R "$found"
    fi
elif [ "$1" = "hide" ]; then
    # send this window to the other desktop
    if [ -z "$found" ]; then
        echo "Error hide but no window $win"
    else
        /usr/bin/wmctrl -i -r "$found" -t 1
    fi
elif [ "$1" = "desktop" ]; then
    # show Desktop
    /usr/bin/wmctrl -k on
elif [ "$1" = "close" ]; then
    if [ -z "$found" ]; then
        echo "Error close but no window $win"
    else
        /usr/bin/wmctrl -i -c "$found"
    fi
fi

