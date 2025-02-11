//declarations.rs
//Sahil Athalye: a.sahil@wustl.edu 
//Varad Deouskar: varad@wustl.edu
//declares Play type, program constants, and whinge boolean

use std::sync::atomic::AtomicBool;

pub const SCRIPT_GEN_FAILURE:u8 = 2;

pub const THREAD_PANIC:u8 = 3;

pub static SHOULD_COMPLAIN: AtomicBool = AtomicBool::new(false);