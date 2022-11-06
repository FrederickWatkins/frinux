#![no_std]
#![no_main]
#![feature(custom_test_frameworks)]
#![test_runner(qemu_test::test_runner)]
#![reexport_test_harness_main = "test_main"]

#[cfg(not(test))]
use core::panic::PanicInfo;

mod qemu_test;
mod vga_buffer;

// The entry point for the OS
#[no_mangle]
pub extern "C" fn _start() -> ! {
    #[cfg(test)]
    test_main();

    println!("Yabba dabba doo!");
    vga_buffer::WRITER
        .lock()
        .set_color(vga_buffer::Color::Black, vga_buffer::Color::White);
    println!("This should be a different colour now");
    panic!("Test panic");
}

#[cfg(not(test))]
#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    println!("{}", info);
    loop {}
}
