//return_wrapper.rs
//Sahil Athalye: a.sahil@wustl.edu 
//Varad Deouskar: varad@wustl.edu
//Implemented the return wrapper which is used to handle return codes.

use std::process::{ExitCode,Termination};


pub struct ReturnWrapper
{
    code: u8,
}

impl ReturnWrapper{
    pub fn new(code:u8) -> Self {
        return ReturnWrapper{code};
    }
}

impl Termination for ReturnWrapper {
    fn report(self) -> ExitCode {
        if self.code!=0 {
            eprintln!("Error: {}",self.code);
        }
        return ExitCode::from(self.code);
    }
}