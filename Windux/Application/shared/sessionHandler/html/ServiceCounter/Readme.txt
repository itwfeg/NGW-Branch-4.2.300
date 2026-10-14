
===============================================================================================================================
I modified/added the following files to add links to our training videos to the on-line help received from O'Neil & Associates:
===============================================================================================================================

Modified F35549_Rev_E\OperatorVideos\Cover.htm (modified to increment rowspan to ‘7’ instead of ‘5’, to account for the 2 new topics for “Operator Training Videos” and “Supervisor Training Videos”)

Added F35549_Rev_E\OperatorVideos\OperatorVideos.htm.  Contains links to our training videos.
Added F35549_Rev_E\SupervisorVideos\SupervisorVideos.htm.  Contains links to our training videos.

Modified whxdata\toc.js (modified to add 2 new topics for “Operator Training Videos” and “Supervisor Training Videos”)
Modified whxdata\toc.new.js (modified to add 2 new topics for “Operator Training Videos” and “Supervisor Training Videos”)

Added whxdata\operatorVideos.js (added for new topic “Operator Training Videos”)
Added whxdata\operatorVideos.new.js (added for new topic “Operator Training Videos”)

Added whxdata\supervisorVideos.js (added for new topic “Supervisor Training Videos”)
Added whxdata\supervisorVideos.new.js (added for new topic “Supervisor Training Videos”)

Modified F35549_Rev_E.ppf (modified to add 2 new topics for “Operator Training Videos” and “Supervisor Training Videos”)

Copied "index.html" to “HT_Operator_Manual.htm”, since the scaleui application launches “HT_Operator_Manual.htm”.
Modified screendata.js to reference “HT_Operator_Manual.htm” instead of "index.html"


====================================================================
Modifications for SOF-4026: HTm Help not showing the training manual
====================================================================

Used Rev-D for release 4.0.0 as the base. Copied the following files
from Rev-E (4.1.0 & 5.1.0) and overlaid them onto Rev-D:

ServiceCounter/HT_Operator_Manual.htm

ServiceCounter/F35549_Rev_E.css  --->  renamed to F35549_Rev_D.css

ServiceCounter/F35549_Rev_E/Chapter_1/Chapter_1.htm  ---> copied to ServiceCounter/F35549_Rev_D
ServiceCounter/F35549_Rev_E/Chapter_2/Chapter_2.htm  ---> copied to ServiceCounter/F35549_Rev_D
ServiceCounter/F35549_Rev_E/Chapter_3/Chapter_3.htm  ---> copied to ServiceCounter/F35549_Rev_D
ServiceCounter/F35549_Rev_E/Chapter_4/Chapter_4.htm  ---> copied to ServiceCounter/F35549_Rev_D
ServiceCounter/F35549_Rev_E/Chapter_5/Chapter_5.htm  ---> copied to ServiceCounter/F35549_Rev_D

ServiceCounter/F35549_Rev_E/Cover/Cover.htm  ---> copied to ServiceCounter/F35549_Rev_D

ServiceCounter/F35549_Rev_E/OperatorVideos/OperatorVideos.htm  ---> copied to ServiceCounter/F35549_Rev_D
ServiceCounter/F35549_Rev_E/SupervisorVideos/OperatorVideos.htm  ---> copied to ServiceCounter/F35549_Rev_D

ServiceCounter/whxdata/toc.js
ServiceCounter/whxdata/toc.new.js
ServiceCounter/whxdata/toc1.js
ServiceCounter/whxdata/toc1.new.js
ServiceCounter/whxdata/toc2.js
ServiceCounter/whxdata/toc2.new..js
ServiceCounter/whxdata/toc3.js
ServiceCounter/whxdata/toc3.new..js
ServiceCounter/whxdata/toc4.js
ServiceCounter/whxdata/toc4.new.js
ServiceCounter/whxdata/toc5.js
ServiceCounter/whxdata/toc5.new.js


NOTE - SUPER MAJOR IMPORTANT: In the text files that were copied from "Rev-E" to "Rev-D",
                              changed any reference of "Rev_E" to "Rev_D"


ServiceCounter\assets - Removed all Rev-D images, and copied all images
                        from Rev-E's assets directory (176 images)
