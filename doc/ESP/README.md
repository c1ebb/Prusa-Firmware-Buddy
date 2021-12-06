# Original Prusa MINI printer WIFI network connection #

Recently you can find a wireless printer network connection feature code in our Prusa-Firmware-Buddy/Master repository. To be able to connect your MINI to the wireless network and run the PrusaLink feature, you will need an ESP01 board plugged in to the MINI and execute the below tasks. Some of the ESP01 boards are already flashed with a compatible FW therefore you could swap the ESP Flashing and Load network data routines.

Be aware please, that the `Prusa-Firmware-Buddy/master` repository is a **development environment** and features are under development as well. We cannot guarantee, that the feature will be 100% working. Building your own FW from these pages, instead of using official release, is at **your sole risk**.

## Flashing ESP board ##

- Build the last MINI firmware from [https://github.com/prusa3d/Prusa-Firmware-Buddy](https://github.com/prusa3d/Prusa-Firmware-Buddy) and load it to the printer
- Download the appropriate files from [ESP8266 NonOS AT Bin V1.7.4](https://www.espressif.com/en/support/download/at?keys=&field_type_tid%5B%5D=14&field_type_tid%5B%5D=799) or you can use the .bin files stored in attached .zip file
- Move all 4 files to root/ESP folder of the USB flash drive, plug in the ESP board and the USB flash drive to printer
- Make sure the **Settings -> Menu -> Timeout** is Off (otherwise the ESP Update menu will time out and interrupt the update)
- Run **Settings -> ESP Update -> FLASH ESP AT**
- Wait until you will see the "**ESP successfully flashed. WIFI initiation started**"  message
- If you already have loaded your ssid and password into the printer, you should see the WiFi icon ON and in **Settings -> Lan settings** you should find the assigned IP address
    - If no ssid and password is loaded, proceed with next chapter..

## Load network data ##

- In the prusa `printer_settings.ini` file and under `[wifi]` tag edit the ssid and psk fields with the correct WiFi data
```
[wifi]
ssid= enter here the SSID of your wireless network
psk= enter here the valid password
key_mgmt=WPA
```
- Store the prusa `printer_settings.ini` file in the root folder of the USB flash drive and plug in to the printer
- Run the **Setting-> Load Settings from** command from the printer
- Make sure the **Settings -> Lan settings** is set to WIFI
- Check the WIFI icon shines and check in **Settings -> Lan settings** whether the IP address is assigned. If so, your printer is connected to network.

## Connect printer to PRUSA LINK ##
- Make sure your printer is connected to the network
- Open an internet browser and copy the IP address from **Setting -> Lan settings -> WIFI**
- PrusaLink page should open and request you to enter an api-key. You will find it in **Settings -> PRUSA LINK -> X-Api-Key**
- Enter the api key
- Telemetric and graphical information regarding the printer status should be displayed
