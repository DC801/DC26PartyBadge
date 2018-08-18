# DC801 DC26 Party Badge
Party badge and BLE hardware board

# Hardware
 - **Rigado BMD-300** Bluetooth low energy based on a Nordic nRF52832
   - 16MHz ARM® Cortex™ M4F CPU, 512kB of flash memory, 64kB RAM
   - [https://www.rigado.com/product/bmd-300/](https://www.rigado.com/product/bmd-300/)
 ***
 - **Lots of I/O**
   - Speaker
   - 6 buttons
   - One hidden button
   - SPI LCD screen
   - Selected pin breakouts
   - JTAG interface
   - Micro USB
   - microSD card
   - 2 SAO connectors
   - 2 minibadges
 ***
 - A MCP73831 LiPo battery charger
 - Kickass silkscreen

Board design is in Kicad

See the [Hardware](/Hardware) directory for more info, schematics and etc

# SD Card

Format your SD card as FAT32 and copy the contents of [SD_Card](SD_Card) into it

# Software
 - Environment is GNU ARM GCC with your choice of IDE (I used CLion)
 - Need a J-Link Segger JTAG programmer
 - Uses the softdevice s132

See the [Software](/Software) directory for more info

# What's it do?

Awesome **things**, that's what!  See the [Software](Software) directory for more information.

# More info

Follow [@rushan_ee](http://twitter.com/rushan_ee) or [@hamster](http://twitter.com/hamster) for updates

