TEMPLATE = subdirs

SUBDIRS = \
    booda \
    dbusinterfaces \
    eventadapters \
    events \
    hobartcomms \
    hobartsessionman \
    hydramc \
    test_hobartcomms \
    test_hobartsessionman \
    test_hydramc

events.depends = dbusinterfaces
hydramc.depends = dbusinterfaces

eventadapters.depends = events

hobartsessionman.depends = dbusinterfaces events

booda.depends = dbusinterfaces events

hobartcomms.depends = dbusinterfaces eventadapters

test_hydramc.depends = hydramc
test_hobartsessionman.depends = hobartsessionman
test_hobartcomms.depends = hobartcomms
