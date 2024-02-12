# Worksheet 1 
## Program Architecture
The program contains the following files

| file name    | Porpuse                                      |
|--------------|----------------------------------------------|
| flash_ops.c  | main functions for running terminal commands |

## Bulding the program

By following the following commands lines, can complile the program.
```bash
cd cap_template/build
cmake ..
make
```
 
## Execution of the program
In the `cap_template/build` will contain a file called `cap_template.uf2` where 
this file needs to be movied inside in the folder of the motherboard.

## Run the motherboard
When the motherboard is connected we need to open an terminal and type:

``` bash
sudo dmesg | tail
sudo screen /dev/ttyACM0 115200
```

## Task  1
For Task 1, have been implement 3 functions called 
- flash_write_safe
- flash_read_safe
- flash_erase_safe

They are responsible for data manipulation on the physical motherboard. Those 3 function can write, read and delete data from the motherboard. 

### flash_write_safe function
The function:
```c
void flash_write_safe(uint32_t offset, const uint8_t *data, size_t data_len)
```
Command line:
```bash
FLASH_WRITE 0 “Hello World"
```

`FLASH_WRITE` will take 2 parameters, the first parameter is the location where the
data is going to be saved in the flash memory, and the second parameter is te data that is going to be saved, 
in that instance it will be `"hello World”` on the location block `0`


### Function: flash_read_safe 
The function:
```c
void flash_read_safe(uint32_t offset, uint8_t *buffer, size_t buffer_len)
```
Command line:

```bash
FLASH_READ 0 100
```
The command it will read anything from the location 0, and the seccond parameter that is 100, will read the 
100 bites on that location

### Function: flash_erase_safe 
The function:
```c
void flash_erase_safe(uint32_t offset) 
```
Command line:
```bash
FLASH_ERASE 0
```
This is responsible for deleting the memory block of location 0, in that case it will erase the string 
data and fill it up back with 0's










### Testing

We can test the program by typing on the
terminal the following code, after you completed this step "link to run the motherboard"

```c
FLASH_WRITE 0 “Hello World”
FLASH_READ 0 100
FLASH_ERASE 0
results......
```






<br><br>
 

## APPENDIX

### Full code explenation for FLASH_WRITE 

The `flash_write` function contains various components for writing on the flash memory.
<br> The following code, contains the FLASH_WRITE function:

```c
void flash_write_safe(uint32_t offset, const uint8_t *data, size_t data_len) {
uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
uint32_t ints = save_and_disable_interrupts();
flash_range_erase(flash_offset, FLASH_SECTOR_SIZE);
flash_range_program(flash_offset, data, data_len);
restore_interrupts(ints);
}
```
Now will break down what each line does<br>
By looking on the first line:

```c
void flash_write_safe(uint32_t offset, const uint8_t *data, size_t data_len)
```
This file has 3 parameters and are:

| Name            | Porpuse |
|-----------------|---------|
| uint32_t offset | blabla  |

<br>
The following line of code is 

```c

uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;

```
The code does: blabla

<br>
"mising code for task for checking"
<br>

After the checking, the next step is to blabl this wil be achieved by this:

```c
uint32_t ints = save_and_disable_interrupts();
```

<br>

Interupts are stopped, giving apportiounity for :

``` c
flash_range_erase(flash_offset, FLASH_SECTOR_SIZE);
``` 
where this and this will happen 

<br>
after that one happens this willl happen 

```c
flash_range_program(flash_offset, data, data_len);

```

blablabla 

<br>

Final line of code is responsible for reseting the interupts function,
```c
restore_interrupts(ints);

```






