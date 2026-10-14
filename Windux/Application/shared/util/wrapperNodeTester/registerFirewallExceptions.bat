netsh advfirewall firewall add rule name="Board Test Server Port" action=allow dir=in protocol=TCP localport=5081 localip=any

netsh advfirewall firewall add rule name="Board Test Server Port" action=allow dir=out protocol=TCP localport=5081 localip=any

echo Board Test Server Firewall Communication Ports Opened
pause