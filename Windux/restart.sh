killall gcd
killall scaleui
killall gdbserver
kill -s TERM $(pgrep chobart_d)

sleep 24s

kill -9 $(pgrep chobart_d)


PATH=/usr/bin:/bin:/usr/sbin:/sbin
#bluetoothd &
DISPLAY=":0.1" gcd > /var/log/gcdStart.txt &
sleep 2s
chobart_d > /var/log/chobart_dStart.txt &
sleep 4s

# -------------------------------------------
# Use one of two sections below by commenting
# either line, depending on if you are remote
# debugging or not.
# -------------------

#Section 1 - No debug
DISPLAY=":0.0" scaleui 127.0.0.1 6000 1 > /var/log/scaleGui.txt

#Section 2 - Remote debugging
#DISPLAY=":0.0" gdbserver localhost:1234 /usr/bin/scaleui -2
