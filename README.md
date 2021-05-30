# esp-idf-mqtt-image-viewer
Image viewer using mqtt and SPI TFT.

![M5Stack-JPEG](https://user-images.githubusercontent.com/6020549/78413968-e0426700-7654-11ea-9040-0fdfd0f2de2e.JPG)

![slide-0001](https://user-images.githubusercontent.com/6020549/118924835-96abfd00-b978-11eb-8224-3331fab505dd.jpg)

![slide-0002](https://user-images.githubusercontent.com/6020549/118925000-e8548780-b978-11eb-9b49-7ae719138bd0.jpg)

![slide-0003](https://user-images.githubusercontent.com/6020549/118927202-649c9a00-b97c-11eb-8a33-f54c230c0997.jpg)

# Hardware requirements
SPI TFT.   
Supported TFTs:   
https://github.com/nopnop2002/esp-idf-ili9340

# Installation for ESP32

```
git clone https://github.com/nopnop2002/esp-idf-mqtt-image-viewer
cd esp-idf-mqtt-image-viewer/
idf.py set-target esp32
idf.py menuconfig
idf.py flash
```

# Installation for ESP32-S2
It don't work because the ROM is small.   


# Configuration   

![config-main](https://user-images.githubusercontent.com/6020549/118922901-c0aff000-b975-11eb-9586-9bce557569ed.jpg)

![config-app](https://user-images.githubusercontent.com/6020549/118922910-c279b380-b975-11eb-94ff-a85ccf06aa19.jpg)

## Network Configure   
You have to set this config value with menuconfig.   
- CONFIG_ESP_WIFI_SSID   
SSID of your wifi.
- CONFIG_ESP_WIFI_PASSWORD   
PASSWORD of your wifi.
- CONFIG_ESP_MAXIMUM_RETRY   
Maximum number of retries when connecting to wifi.   
- CONFIG_BROKER_URL   
MQTT Broker url or IP address to use.   
- CONFIG_MQTT_TOPIC   
MQTT topic to subscribe.   

![config-network](https://user-images.githubusercontent.com/6020549/118922904-c1e11d00-b975-11eb-8013-e98e5ff93430.jpg)


## TFT Configure   
Follow [here](https://github.com/nopnop2002/esp-idf-ili9340).

![config-tft](https://user-images.githubusercontent.com/6020549/118922908-c1e11d00-b975-11eb-89fc-e7c5a57ca88e.jpg)

# How to use
`mosquitto_pub -d -h  broker.emqx.io -t image/example -f esp32.jpeg`

![M5Stack-JPEG](https://user-images.githubusercontent.com/6020549/78413968-e0426700-7654-11ea-9040-0fdfd0f2de2e.JPG)


`mosquitto_pub -d -h  broker.emqx.io -t image/example -f esp_logo.png`

![M5Stack-PNG](https://user-images.githubusercontent.com/6020549/78613610-40c8e280-78a7-11ea-95b0-a89ce14dc196.JPG)

