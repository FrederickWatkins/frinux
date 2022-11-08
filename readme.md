# Frinux
## A homemade operating system kernel
The name is a joke, I'm not *that* full of myself.
## Code structure
Subject to change
### [src/gdt](src/gdt/)
Contains the global descriptor table
### [src/interrupts](src/interrupts)
Contains interrupts handlers (will likely be split into multiple submodules in the near future)
### [src/io](src/io)
Contains io (currently just writing to the vga buffer) (probably not going to stay a module for much longer)
### [src/qemu_test](src/qemu_test/)
Macros and panic handler for testing in qemu
### [lib.rs](lib.rs)
Definitions of all modules and functions in the crate root
### [main.rs](main.rs)
Definition of default entry point