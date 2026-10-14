def index():
    return HTTP(404)
    
def settings():
    return response.render("default/settings.html")
