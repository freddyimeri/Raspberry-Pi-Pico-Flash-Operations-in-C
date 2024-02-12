# CAP Worksheet 1 Template

## Command lines instructions

### Bulding the code
inside in <B>cap_template/build</B> Run cmake ..
for Bulding the <B>cap_template</B> folder inside in build directory<br>

After the code is executed run <B>make</B> Command<br>

#### run the code in the motherboard
in the <B>cap_template/build</B> will be a file called cap_template.uf2, where <br>
this file needs to be coppied inside in the folder of the motherboard.

### Run the motherboard
When the motherboard is connected we need to open an terminal and type:
sudo dmesg | tail
sudo screen /dev/ttyACM0 115200

<br>
from there you can test the application by running this commands
<br>FLASH_WRITE 0 “Hello World”<br>
<br>FLASH_READ 0 100<br>
<br>FLASH_ERASE 0<br>

#### commands explenation 
FLASH_WRITE 0 “Hello World”<br>
FLASH_WRITE will take 2 parameters, the first parameter is the location where the
data is going to be saved in the flash memory, and the second parameter is what is going to be saved, 
in that instance it will be "hello World”, that is an string format.
<br><br>

FLASH_READ 0 100, this commands it will read anything from the location 0, and the seccond parameter that is 100 it will read the 
100 bites on that location

<br><br>

FLASH_ERASE 0, this is responsible for deleting the memory block of location 0, in that case it will erase the string 
data and fill it up back with 0's 

## code in the 















