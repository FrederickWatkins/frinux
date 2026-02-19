use riscv::register::{scause, sepc, stvec};

use crate::println;

use core::arch::naked_asm;

pub fn init() {
    let stvec = stvec::Stvec::new(trap_entry as *const () as usize, stvec::TrapMode::Direct);
    unsafe { stvec::write(stvec) };
}

#[unsafe(naked)]
pub unsafe extern "C" fn trap_entry() {
    naked_asm!(
        include_str!("context.inc"),
        ".align 2",
        include_str!("save_context.S"),
        "call {rust_handler}",
        include_str!("load_context.S"),
        "sret",
        rust_handler = sym handler,
    );
}

pub fn handler() {
    let scause = scause::read();
    let sepc = sepc::read();
    let ptr = sepc as *mut u16;
    match scause.cause() {
        scause::Trap::Exception(3) => unsafe {
            println!("Break!");
            *ptr = 0x0001;
        },
        scause::Trap::Exception(_) => todo!(),
        scause::Trap::Interrupt(_) => todo!(),
    }
}
