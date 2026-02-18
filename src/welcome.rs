use crate::println;

const LOGO: [&str; 5] = [
    "    _______   _______   ________   _______  ________  ________ ",
    "  ╱╱       ╲╱╱       ╲ ╱        ╲╱╱   ╱   ╲╱    ╱   ╲╱    ╱   ╲",
    " ╱╱      __╱╱        ╱_╱       ╱╱╱        ╱         ╱_       _╱",
    "╱        _╱        _╱╱         ╱         ╱        ╱╱         ╱ ",
    "╲_______╱ ╲____╱___╱ ╲╲_______╱╲__╱_____╱╲_______╱╱╲___╱___╱╱  ",
];

pub fn welcome_message() {
    for s in LOGO {
        println!("{s}");
    }
    println!("Welcome to frinux version {}", env!("CARGO_PKG_VERSION"));
}
