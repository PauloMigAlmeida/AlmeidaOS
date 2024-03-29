# AlmeidaOS
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/1750dfde498946ba8fb8a113c49a5ac7)](https://www.codacy.com/gh/PauloMigAlmeida/AlmeidaOS/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=PauloMigAlmeida/AlmeidaOS&amp;utm_campaign=Badge_Grade)


Simple, self-contained, clean code, well-documented, hobbyist 64-bit operating system for Intel and AMD processors. It's written in Assembly (Intel syntax) and C (GNU99 Standard) in which I strive for adding comments to the source code as much as possible so it could be used as a reference for future OS developers.

It contains the following components written from scratch:

## BIOS
| Component | Description | Source code |
| --- | ----------- | ----------- |
| MBR | First stage bootloader | [code](src/boot/mbr.asm) |
| Loader | Second stage bootloader | [code](src/boot/loader.asm) |


## Kernel
| Component | Description | Source code |
| --- | ----------- | ----------- |
| PML4 | Paging Structure | [code](src/kernel/mm/page.c) |
| Buddy | Memory allocator System | [code](src/kernel/mm/buddy.c) |
| PrintK | printf-like string format parsing utility | [code](src/kernel/lib/printk.c) |
| Serial Driver | send printk msgs via RS232 to help debugging | [code](src/kernel/device/serial.c) |
| Core Dump | Dump CPU registers for debugging purposes  | [code](src/kernel/debug/coredump.c) |
| Syscall/Sysret | method chosen to jump to Ring 3 and back | [code](src/kernel/syscall) |
| PIT | Programmable Interval Timer | [code](src/kernel/arch/pit.c) |
| PIC | Programmable Interrupt Controller | [code](src/kernel/arch/pic.c) |
| (x)delay | Based on tightloops given that I'm using PIT | [code](src/kernel/time/delay.c) |
| CMOS RTC | Real-time clock | [code](src/kernel/arch/cmos.c) |
| Scheduler | Simple Round-Robin scheduler | [code](src/kernel/task/scheduler.c) |

## libc
functions are being added on-demand:  [code](src/libc)

More to come! Star/Watch this repo to stay tuned!

## Build
In order to build this app, you are expected to have `make` and `docker` installed in your machine. All of the other building dependencies should be taken care of automatically by the docker image created during this step.

To build it you have to run:

```{shell}
make build
```

## Run
In order to run this app, you are expected to have `make` and `qemu-system-x86_64` installed in your machine.

```{shell}
make test
```

## Wishlist
To make sure I won't lose focus on what I want this OS to be able to do, I decided to write a list of features
that I want to implement in the short to medium term.

- [X] Serial port driver
- [X] Early printk (contingent on serial port driver) -> will help debugging in real computers
- [X] limited libc -> (It will always be "Work in progress" as I add functions as I need them)
- [X] Userspace
- [ ] Rudimentary Bash-like terminal

## Stretch goals
Things that would be fantastic to have but I am not sure if I have what it takes to do within my lifetime.

- [ ] networking capabilities (I would be damn happy with UDP already..I wouldn't dare trying to implement TCP)
- [ ] Filesystem
- [X] Scheduler / Time-sharing OS

## References
These are all the references that helped me a lot during the development of AlmeidaOS

Forums:
- https://www.osdev.org/
- https://www.reddit.com/r/osdev/

Books:
- https://www.amazon.com/Understanding-Linux-Kernel-Third-Daniel/dp/0596005652
- https://www.amazon.com/Linux-Device-Drivers-Jonathan-Corbet/dp/0596005903/
- https://www.amazon.com/Hackers-Delight-2nd-Henry-Warren/dp/0321842685 - (thanks [@igorlg](https://github.com/igorlg))
- https://www.amazon.com/Understanding-Using-Pointers-Techniques-Management/dp/1449344186 

Existing OSes:
- https://github.com/beevik/MonkOS
- https://elixir.bootlin.com/linux/latest/source

Courses:
- https://www.udemy.com/course/writing-your-own-operating-system-from-scratch/
