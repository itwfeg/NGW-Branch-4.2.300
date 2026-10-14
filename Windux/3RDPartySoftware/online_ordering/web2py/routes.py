# this file does redirects
# in this case, we just redirect an empty path to the OnlineOrdering index page

routes_in=[('/','/OnlineOrdering/default/index')]

# by enabling this, the request.raw_args variable is made available
# this allows us to access parts of the path that contain spaces
# which web2py otherwise converts into underscores
routes_apps_raw=['api']
