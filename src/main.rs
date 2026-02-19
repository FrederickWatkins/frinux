#![no_std]
#![no_main]

global_asm!(include_str!("entry.S"));

mod interrupt;
mod rv64_critical_section;
mod uart;
mod welcome;

use core::arch::{asm, global_asm};
use core::panic::PanicInfo;
use heapless::Deque;
use riscv::register;

use crate::welcome::welcome_message;

#[unsafe(no_mangle)]
pub extern "C" fn _entry() -> ! {
    let mut input_buffer: Deque<u8, 256> = Deque::new();
    interrupt::init();
    welcome_message();
    unsafe {
        register::sie::enable(riscv::interrupt::Interrupt::SupervisorTimer);
        register::sstatus::set_sie();
    }
    let now = riscv::register::time::read();
    sbi::timer::set_timer((now + 10000000) as u64).unwrap();
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
                } else if s.contains("TIME") {
                    println!("Current time: {}", register::time::read64());
                } else if s.contains("KYS") {
                    unsafe { asm!("li t1, 0xDEADBEEF", "amoadd.w zero, t2, (t1)",) }
                } else if s.contains("BREAK") {
                    unsafe { asm!("ebreak") }
                }
                input_buffer.clear();
            }
        }
    }
}

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    unsafe {
        uart::force_unlock();
    }
    println!("\x1b[H\x1b[2J\x1b[31mkernel panic!\x1b[0m");
    print!("{}", info.message());
    if let Some(location) = info.location() {
        println!("@{location}");
    } else {
        println!("");
    }
    loop {}
}
