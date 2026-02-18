#![no_std]
#![no_main]

global_asm!(include_str!("entry.S"));

mod uart;
mod welcome;

use core::arch::global_asm;
use core::panic::PanicInfo;
use heapless::Deque;

use crate::welcome::welcome_message;

#[unsafe(no_mangle)]
pub extern "C" fn _entry() -> ! {
    let mut input_buffer: Deque<u8, 256> = Deque::new();
    welcome_message();
    loop {
        if let Some(c) = uart::read_byte() {
            print!("{}", char::from(c));
            input_buffer.push_back(c).unwrap_or_default();
            if c == b'\r' {
                let s = str::from_utf8_mut(input_buffer.make_contiguous()).unwrap();
                s.make_ascii_uppercase();
                println!("");
                if s.contains("PANIC") {
                    panic!("Asked to panic")
                } else if s.contains("HELLO COMPUTER") {
                    println!("Hello world!");
                }
            }
        }
    }
}

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    println!("\x1b[31mkernel panic!\x1b[0m");
    print!("{}", info.message());
    if let Some(location) = info.location() {
        println!("@{location}");
    } else {
        println!("");
    }
    loop {}
}
