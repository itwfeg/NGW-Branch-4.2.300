import sys
import requests
import json

image_path = "/usr/local/hobart/user/graphics/"

url = "https://api.openweathermap.org/data/2.5/weather?q=Troy&appid=f6d206afe3510fbc0c8774f8fda5dd4c"

res = requests.get(url)

data = res.json()

temp = (data['main']['temp']-273.15)*9/5+32

print("image (50,50) 50 50 fit " + image_path + "cloud.png")
print("text (50,50) 25 #000000 " +  str(int(temp)) + "F")
