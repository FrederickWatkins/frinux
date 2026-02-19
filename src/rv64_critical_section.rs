use critical_section::RawRestoreState;

struct Rv64CriticalSection;

unsafe impl critical_section::Impl for Rv64CriticalSection {
    unsafe fn acquire() -> RawRestoreState {
        use riscv::register::sstatus;

        let sie = sstatus::read().sie();
        unsafe {
            sstatus::clear_sie();
        }

        sie as RawRestoreState
    }

    unsafe fn release(state: RawRestoreState) {
        use riscv::register::sstatus;
        if state {
            unsafe {
                sstatus::set_sie();
            }
        }
    }
}

critical_section::set_impl!(Rv64CriticalSection);
