# AlmeidaOS


Simple, self-contained, clean code, well-documented, hobbyist 64-bit operating system for Intel and AMD processors. It's written in Assembly (Intel syntax) and C (GNU99 Standard) in which I strived for adding comments to the source code as much as possible so it could be used as a reference for future OS developers.

It contains the following components written from scratch:

- BIOS Boot loader: [code](src/boot)
- Buddy Memory Allocation System
- libc (functions are being added on-demand): 
- printk -> simplified printf-like string format parsing

More to come.

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
- [ ] limited libc -> (Work in progress)
- [X] Userspace
- [ ] Rudimentary Bash-like terminal

## Stretch goals
Things that would be fantastic to have but I am not sure if I have what it takes to do within my lifetime.

- [ ] networking capabilities (I would be damn happy with UDP already..I wouldn't dare trying to implement TCP)
- [ ] Filesystem
- [ ] Scheduler / Time-sharing OS
