

:: ###########################################
E:\ISO\MyUpdateImage>7z a -ttar -so archive.tar AutoUnattend_Files AutoUnattend.xml | 7z a -si htx-win-core-0.0.6-11172014_171810.tgz
:: ###########################################


7-Zip [64] 9.20  Copyright (c) 1999-2010 Igor Pavlov  2010-11-18
Scanning
7-Zip [64] 9.20  Copyright (c) 1999-2010 Igor Pavlov  2010-11-18
Creating archive htx-win-core-0.0.6-11172014_171810.tgz

Compressing  [Content]    0%

Creating archive stdout

    0%
    0%Everything is Ok


Everything is Ok

E:\ISO\MyUpdateImage>dir
 Volume in drive E is Second Drive
 Volume Serial Number is 9049-C823

 Directory of E:\ISO\MyUpdateImage

11/17/2014  05:18 PM    <DIR>          .
11/17/2014  05:18 PM    <DIR>          ..
11/17/2014  05:05 PM            67,971 AutoUnattend.xml
11/17/2014  05:04 PM    <DIR>          AutoUnattend_Files
11/17/2014  05:21 PM     2,800,184,222 htx-win-core-0.0.6-11172014_171810.tgz
               2 File(s)  2,800,252,193 bytes
               3 Dir(s)  575,505,698,816 bytes free

               
:: ###########################################
E:\ISO\MyUpdateImage>python c:\fasTrack\branch-1-htx-win\Windux\Application\shared\sessionHandler\python\md5checksum.py htx-win-core-0.0.6-11172014_171810.tgz > htx-win-core-0.0.6-11172014_171810.md5
:: ###########################################


E:\ISO\MyUpdateImage>dir
 Volume in drive E is Second Drive
 Volume Serial Number is 9049-C823

 Directory of E:\ISO\MyUpdateImage

11/18/2014  08:23 AM    <DIR>          .
11/18/2014  08:23 AM    <DIR>          ..
11/17/2014  05:05 PM            67,971 AutoUnattend.xml
11/17/2014  05:04 PM    <DIR>          AutoUnattend_Files
11/18/2014  08:24 AM                34 htx-win-core-0.0.6-11172014_171810.md5
11/17/2014  05:21 PM     2,800,184,222 htx-win-core-0.0.6-11172014_171810.tgz

