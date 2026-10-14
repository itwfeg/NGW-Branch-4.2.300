netsh advfirewall firewall add rule name="Black Box Device SSH Port" action=allow dir=in protocol=TCP localport=22 localip = any

rem netsh advfirewall firewall add rule name="Black Box Device Communication Port" action=allow dir=in protocol=TCP localport=5076 localip = any

netsh advfirewall firewall add rule name="Black Box Device SSH Port" action=allow dir=out protocol=TCP localport=22 localip = any

rem netsh advfirewall firewall add rule name="Black Box Device Communication Port" action=allow dir=out protocol=TCP localport=5076 localip = any

echo Black Box Device Firewall Communication Ports Opened
pause