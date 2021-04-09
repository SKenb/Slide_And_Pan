import urllib

def getFirmwareEndpoint(firmwareName, firmwareKey):
    return "https://sebastianknoll.net/api/?api=Slider%20Firmware%20Manager&id=3&token=6f40bgi8wsf3h1wcp0p0xc430epaqv561aedizuu4&firmware=" + urllib.parse.quote(str(firmwareName)) + "&fk=" + urllib.parse.quote(str(firmwareKey))

def getFirmwareListEndpoint():
    return "https://sebastianknoll.net/api/?api=Slider%20Firmware%20Manager&id=3&token=6f40bgi8wsf3h1wcp0p0xc430epaqv561aedizuu4&list=1"

def getMockVersionJson():

    return """
        {
            \"Firmwares\": [
                { 
                    \"Name\": \"Basic Slide\",
                    \"Version\": \"1.0.0\",
                    \"ReleaseDate\": \"01.05.2021\",
                    \"Notes\": \"All basic functions included\",
                    \"IsKeyRequired\": 0
                },
                { 
                    \"Name\": \"Astro Slide\",
                    \"Version\": \"1.3.0\",
                    \"ReleaseDate\": \"01.05.2021\",
                    \"Notes\": \"Perfect when Astro is the main focus\",
                    \"IsKeyRequired\": 1
                },
                { 
                    \"Name\": \"Profi Slide\",
                    \"Version\": \"2.0.0\",
                    \"ReleaseDate\": \"01.05.2021\",
                    \"Notes\": \"All available functions included\",
                    \"IsKeyRequired\": 1
                }
            ]
        }
    
    """