#[cfg(test)]
use core::panic::PanicInfo;

pub mod serial;

#[allow(dead_code)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u32)]
pub enum QemuExitCode {
    Success = 0x10,
    Failed = 0x11,
}

pub fn exit_qemu(exit_code: QemuExitCode) {
    use x86_64::instructions::port::Port;

    unsafe {
        let mut port = Port::new(0xf4);
        port.write(exit_code as u32);
    }
}

#[cfg(test)]
#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    crate::serial_println!("[failed]\n");
    crate::serial_println!("Error: {}", info);
    exit_qemu(QemuExitCode::Failed);
    crate::halt_loop();
}

#[allow(dead_code)]
pub fn test_runner(tests: &[&dyn Testable]) {
    crate::serial_println!("Running {} tests", tests.len());
    for test in tests {
        test.run();
    }

    exit_qemu(QemuExitCode::Success);
}

pub trait Testable {
    fn run(&self);
}

impl<T> Testable for T
where
    T: Fn(),
{
    fn run(&self) {
        crate::serial_print!("{}..\t", core::any::type_name::<T>());
        self();
        crate::serial_println!("[ok]")
    }
}
