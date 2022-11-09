use crate::gdt;
use lazy_static::lazy_static;
use x86_64::structures::idt::InterruptDescriptorTable;

mod exceptions;
mod pics;

lazy_static! {
    static ref IDT: InterruptDescriptorTable = {
        let mut idt = InterruptDescriptorTable::new();
        idt.breakpoint
            .set_handler_fn(exceptions::breakpoint_handler);
        idt.double_fault
            .set_handler_fn(exceptions::double_fault_handler);
        unsafe {
            idt.double_fault
                .set_handler_fn(exceptions::double_fault_handler)
                .set_stack_index(gdt::DOUBLE_FAULT_IST_INDEX);
        }
        idt[pics::InterruptIndex::Timer.as_usize()].set_handler_fn(pics::timer_handler);
        idt
    };
}

pub fn init() {
    IDT.load();
    pics::init();
}
