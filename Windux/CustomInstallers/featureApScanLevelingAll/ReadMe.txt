Feature: 
    AP Scan Leveling
    
Signed Name:
    don't have a signed file at this time

Unsigned Names (unsigned): every sub directory has the same file names. The directory specifies the feature
    hti-linux-feature-apscan_zero_leveling_update-2.1.201-01142015_023308.md5
    hti-linux-feature-apscan_zero_leveling_update-2.1.201-01142015_023308.tgz
    hti-linux-feature-apscan_one_leveling_update-2.1.201-01142015_023308.md5
    hti-linux-feature-apscan_one_leveling_update-2.1.201-01142015_023308.tgz
    hti-linux-feature-apscan_two_leveling_update-2.1.201-01142015_023308.md5
    hti-linux-feature-apscan_two_leveling_update-2.1.201-01142015_023308.tgz
    hti-linux-feature-apscan_none_leveling_update-2.1.201-01142015_023308.md5
    hti-linux-feature-apscan_none_leveling_update-2.1.201-01142015_023308.tgz

What it does:
    Changes the value of ap_scan inside setNetwork.py. The feature contains just different versions of setNetwork.py with this value.

Note: If setNetwork.py or ever changes, this feature may not work anymore if the scale was upgraded.

Validation:
    In usr/local/hobart/python open the file setNetwork.py file. In the 4 different versions, dependent on the directory it
    was installed from, you will get the following values.
    ap_scan=0
    ap_scan=1
    ap_scan=2
    none = ap_scan value does not exist.

Will this work under windows?
    Nope