def getVersionJsonEndpoint():
    return "https://sebastianknoll.net/photo/media/list"

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