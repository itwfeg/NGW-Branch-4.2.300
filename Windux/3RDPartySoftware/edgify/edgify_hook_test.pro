TEMPLATE = subdirs

SUBDIRS = \
    edgify_hook \
    edgifytrainer \
    test_app

edgifytrainer.depends = edgify_hook

test_app.depends = edgify_hook
