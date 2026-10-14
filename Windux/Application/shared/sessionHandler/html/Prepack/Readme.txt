
====================
F35557-Rev-A-NGW-2P:
====================


1.  Copied "NGW Operator Manual.htm" to "HT_Operator_Manual.htm".  The scaleui application is hard-code to launch
    online help using "HT_Operator_Manual.htm", so I renamed the main help file to match this.

2.  Within "HT_Operator_Manual.htm", modified references (HREF) to "NGW Operator Manual.htm" to be "HT_Operator_Manual.htm"

3.  I noticed the table of contents pane and main content pane were not filling the whole screen on a 14" display on the
    wrapper.  In "HTi_Operator_Manual.css" I modified "#contentAlignmentContainer" and "#tocDiv" entries to be exactly
    like they were on the previous version of online help (e.g. changed height to "98%" instead of a hard-coded
    height of "465px".  In fact with those two changes, "HTi_Operator_Manual.css" is now identical to the file from
    the previous version of online help.

4.  At the top of the TOC, added links to our training videos - same as always.

5.  Removed links for French and Spanish at the top of the TOC, as they did nothing.  Removed link at the bottom
    of the TOC for "Operator Training Video".


IMPORTANT NOTE:
---------------

The following files are not part of online help, but are needed by the Wrapper Error History screen.
These need to reside in html/Prepack, so do not remove them.

    driverBoardInfoChart.html
    driverBoardInfoChartNGW.html
    driverBoardLimits.html
    driverBoardLimitsNGW.html

