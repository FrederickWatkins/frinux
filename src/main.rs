#![no_std]
#![no_main]
#![feature(custom_test_frameworks)]
#![test_runner(qemu_test::test_runner)]
#![reexport_test_harness_main = "test_main"]
#![feature(abi_x86_interrupt)]

use frinux::*;

// The entry point for the OS
#[no_mangle]
pub extern "C" fn _start() -> ! {
    init();
    #[cfg(test)]
    test_main();
    
    halt_loop();
}