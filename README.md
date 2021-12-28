# Hand-made bootable floppy code

Assembly code for a bootable floppy. I suggest to test it in a 32-bit virtual machine.

"Why a floppy?"
Because it's the easiest device to boot from (with the help of the BIOS)

The program set up the 32-bit mode with access to the first 4 GB of memory. Supports english PS/2 keyboard and uses the default graphical mode (80x25 characters, with colours).
There is also a very simple mechanism of task switching.

It is written for FASM. main.asm is the main file that calls \*.inc files. The output of FASM is main.img.
To test on real hardware, get an old 32-bit capable PC with 3.5" floppy drive and PS/2 keyboard, and a floppy disk. Write the floppy image onto floppy disk with dd then reboot.
