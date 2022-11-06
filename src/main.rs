#![no_std]
#![no_main]
#![feature(custom_test_frameworks)]
#![test_runner(qemu_test::test_runner)]
#![reexport_test_harness_main = "test_main"]

use frinux::*;

// The entry point for the OS
#[no_mangle]
pub extern "C" fn _start() -> ! {
    #[cfg(test)]
    test_main();

    println!("Yabba dabba doo!");
    io::vga_buffer::WRITER
        .lock()
        .set_color(io::vga_buffer::Color::Black, io::vga_buffer::Color::White);
    println!("This should be a different colour now");
    panic!("Test panic");
}