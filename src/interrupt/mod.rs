use heapless::format;
use riscv::register::{scause, sepc, stvec};

use core::{arch::naked_asm, fmt};

#[repr(C)]
struct SavedContext {
    pub ra: u64,  // x1
    pub gp: u64,  // x3
    pub tp: u64,  // x4
    pub t0: u64,  // x5
    pub t1: u64,  // x6
    pub t2: u64,  // x7
    pub s0: u64,  // x8
    pub s1: u64,  // x9
    pub a0: u64,  // x10
    pub a1: u64,  // x11
    pub a2: u64,  // x12
    pub a3: u64,  // x13
    pub a4: u64,  // x14
    pub a5: u64,  // x15
    pub a6: u64,  // x16
    pub a7: u64,  // x17
    pub s2: u64,  // x18
    pub s3: u64,  // x19
    pub s4: u64,  // x20
    pub s5: u64,  // x21
    pub s6: u64,  // x22
    pub s7: u64,  // x23
    pub s8: u64,  // x24
    pub s9: u64,  // x25
    pub s10: u64, // x26
    pub s11: u64, // x27
    pub t3: u64,  // x28
    pub t4: u64,  // x29
    pub t5: u64,  // x30
    pub t6: u64,  // x31
    pub sp: u64,  // x2 (stack pointer, after all others)
}

impl fmt::Display for SavedContext {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        writeln!(f, "ra : {:#018x}", self.ra)?;
        writeln!(f, "sp : {:#018x}", self.sp)?;
        writeln!(f, "gp : {:#018x}", self.gp)?;
        writeln!(f, "tp : {:#018x}", self.tp)?;
        writeln!(f, "t0 : {:#018x}", self.t0)?;
        writeln!(f, "t1 : {:#018x}", self.t1)?;
        writeln!(f, "t2 : {:#018x}", self.t2)?;
        writeln!(f, "s0 : {:#018x}", self.s0)?;
        writeln!(f, "s1 : {:#018x}", self.s1)?;
        writeln!(f, "a0 : {:#018x}", self.a0)?;
        writeln!(f, "a1 : {:#018x}", self.a1)?;
        writeln!(f, "a2 : {:#018x}", self.a2)?;
        writeln!(f, "a3 : {:#018x}", self.a3)?;
        writeln!(f, "a4 : {:#018x}", self.a4)?;
        writeln!(f, "a5 : {:#018x}", self.a5)?;
        writeln!(f, "a6 : {:#018x}", self.a6)?;
        writeln!(f, "a7 : {:#018x}", self.a7)?;
        writeln!(f, "s2 : {:#018x}", self.s2)?;
        writeln!(f, "s3 : {:#018x}", self.s3)?;
        writeln!(f, "s4 : {:#018x}", self.s4)?;
        writeln!(f, "s5 : {:#018x}", self.s5)?;
        writeln!(f, "s6 : {:#018x}", self.s6)?;
        writeln!(f, "s7 : {:#018x}", self.s7)?;
        writeln!(f, "s8 : {:#018x}", self.s8)?;
        writeln!(f, "s9 : {:#018x}", self.s9)?;
        writeln!(f, "s10: {:#018x}", self.s10)?;
        writeln!(f, "s11: {:#018x}", self.s11)?;
        writeln!(f, "t3 : {:#018x}", self.t3)?;
        writeln!(f, "t4 : {:#018x}", self.t4)?;
        writeln!(f, "t5 : {:#018x}", self.t5)?;
        writeln!(f, "t6 : {:#018x}", self.t6)?;
        Ok(())
    }
}

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

extern "C" fn handler(regs_ptr: *const SavedContext) {
    let scause = scause::read();
    let sepc = sepc::read();
    let regs = unsafe { &*regs_ptr };
    match scause.cause() {
        scause::Trap::Exception(code) => {
            exception(code, sepc, regs);
        }
        scause::Trap::Interrupt(5) => {
            let now = riscv::register::time::read();
            sbi::timer::set_timer((now + 10000000) as u64).unwrap();
        }
        scause::Trap::Interrupt(_) => todo!(),
    }
}

const EXCEPTION_MESSAGES: [&str; 16] = [
    "instruction address misaligned",
    "instruction access fault",
    "illegal instruction",
    "breakpoint",
    "load address misaligned",
    "load access fault",
    "store/amo address misaligned",
    "store/amo access fault",
    "environment call",
    "unknown exception code",
    "unknown exception code",
    "unknown exception code",
    "instruction page fault",
    "load page fault",
    "unknown exception code",
    "store/amo page fault",
];

fn exception(code: usize, sepc: usize, regs: &SavedContext) -> ! {
    if regs.sp == 0 {
        // Kernel caused exception, panic
        let ptr1 = sepc as *const u16;
        let ptr2 = (sepc + 2) as *const u16;
        let instr = unsafe { (*ptr2 as u32) << 16 | *ptr1 as u32 };
        if let Ok(instr) = rvdc::Inst::decode(instr) {
            panic!(
                "Encountered exception \"{}\" on instruction {} at address 0x{:x}\nRegister state:\n{}",
                EXCEPTION_MESSAGES[code], instr.0, sepc, regs
            );
        } else {
            panic!("Encountered exception \"{}\" at address 0x{:x}", EXCEPTION_MESSAGES[code], sepc);
        }
    } else {
        todo!("Implement userspace")
    }
}
