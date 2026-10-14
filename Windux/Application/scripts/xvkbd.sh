#!/bin/bash
# this file should exist in /usr/bin

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

bin=xvkbd
lang=$1
down=$2

#echo "Start xvkbd for $1"
/usr/bin/xrdb /etc/hobart/Xdefaults.$1 

# find the HLX window to focus on

if [ "$down" = "0" ]; then
	win=`export DISPLAY=:0.0;wmctrl -l|grep HLX|cut -c-10`    
	/usr/bin/$bin -window $win -geometry 800x200+0-0 &
elif [ "$down" = "1" ]; then
	win=`export DISPLAY=:0.0;wmctrl -l|grep HLX|cut -c-10`    
	/usr/bin/$bin -window $win -geometry 800x200+0+60 &
elif [ "$down" = "3" ]; then
	win=`export DISPLAY=:0.0;wmctrl -l|grep Intranet|cut -c-10`
	/usr/bin/$bin -window $win -geometry 800x200+0-0 &
elif [ "$down" = "2" ]; then
	win=`export DISPLAY=:0.0;wmctrl -l|grep Intranet|cut -c-10`
	/usr/bin/$bin -window $win -geometry 800x200+0+0 &
else
    /usr/bin/$bin -window $win -geometry 800x200+0+200 &
fi

