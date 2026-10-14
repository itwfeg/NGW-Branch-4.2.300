netsh advfirewall firewall delete rule name="Black Box Device SSH Port" protocol=TCP localport=22

rem netsh advfirewall firewall delete rule name="Black Box Device Communication Port" protocol=TCP localport=5076

echo Black Box Device Firewall Communication Ports Closed
pause