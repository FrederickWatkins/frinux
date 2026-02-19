use core::fmt::{self, Write};
use sbi::{PhysicalAddress, debug_console};
use spin::Mutex;

pub fn read_byte() -> Option<u8> {
    UART.lock().read_byte()
}

struct Uart;

impl Uart {
    pub fn read_byte(&self) -> Option<u8> {
        let mut c = [0 as u8];
        let read_len = unsafe {
            debug_console::read_ptr(
                // We don't mutate s so it's fine, just an openSBI api limitation
                PhysicalAddress::from_ptr(&mut c as *mut [u8]),
            )
            .unwrap()
        };
        if read_len > 0 { Some(c[0]) } else { None }
    }
}

impl Write for Uart {
    fn write_str(&mut self, s: &str) -> core::fmt::Result {
        unsafe {
            debug_console::write_ptr(
                // We don't mutate s so it's fine, just an openSBI api limitation
                PhysicalAddress::from_ptr(s.as_bytes() as *const [u8] as *mut [u8]),
            )
            .map_err(|_| fmt::Error)?;
        }
        Ok(())
    }
}

static UART: Mutex<Uart> = Mutex::new(Uart {});

#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => ($crate::uart::_print(format_args!($($arg)*)));
}

#[macro_export]
macro_rules! println {
    () => ($crate::print!("\n"));
    ($($arg:tt)*) => ($crate::print!("{}\n", format_args!($($arg)*)));
}

#[doc(hidden)]
pub fn _print(args: fmt::Arguments) {
    use core::fmt::Write;
    critical_section::with(|_| UART.lock().write_fmt(args).unwrap());
}

pub unsafe fn force_unlock() {
    unsafe {
        UART.force_unlock();
    }
}
