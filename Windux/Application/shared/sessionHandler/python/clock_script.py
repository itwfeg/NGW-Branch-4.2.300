# @file clock_script.py
# @author Tony Shelton-McGaha

import sys
from datetime import datetime

#TODO get insight about what this script entails
if __name__ == "__main__":
	if len(sys.argv) != 1:
		print( "clock_script.py" )
		sys.exit(-1)
		
	time = datetime.now()
	
	#prints current time via text command
	print( "text (50,30) 15 #000000 " + str(time.strftime( "%I:%M %p" )) )
	
	
	
 
