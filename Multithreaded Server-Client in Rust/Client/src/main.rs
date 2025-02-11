//main.rs
//Sahil Athalye: a.sahil@wustl.edu
//Varad Deouskar: varad@wustl.edu
//Main Function: parses command line arguments, calls for script preparation, and then recites script
pub mod lab3;

use std::env;
use std::sync::atomic::Ordering;
use std::io::{self, Write};
use lab3::play::Play;

use crate::lab3::declarations::SCRIPT_GEN_FAILURE;
use crate::lab3::declarations::SHOULD_COMPLAIN;
use crate::lab3::return_wrapper::ReturnWrapper;



const MIN_ARGS:usize = 2;
const MAX_ARGS:usize = 3;

const PROGRAM_POSITION:usize = 0;
const CONFIG_POSITION:usize = 1;
const OPTIONALARG_POSITION:usize = 2;

const FIX_COMMAND_LINE:u8 = 1;

const SUCCESS_CODE:u8 = 0;

fn main() ->  ReturnWrapper {
    let mut config_filename:String = String::new();

    if let Err(_e) = parse_args(&mut config_filename){
        return ReturnWrapper::new(FIX_COMMAND_LINE);
    }
    
    let mut play_struct:Play = Play::new();

    //Preparing the play
    if let Err(_e) = play_struct.prepare(&config_filename){
        return ReturnWrapper::new(SCRIPT_GEN_FAILURE);
    }

    //Reciting the play
    play_struct.recite();

    //ReturnWrapper for success
    return ReturnWrapper::new(SUCCESS_CODE);
}

fn usage(program_name: &String){
    if let Err(e) = writeln!(io::stdout().lock(), "usage: {} <config_file_name> [whinge]",program_name) {
        let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
    }
}

fn parse_args(config_filename:&mut String) -> Result<(), u8>{
    let mut cli_args: Vec<String> = Vec::new();
    for arg in env::args() { 
        cli_args.push(arg);
    }

    
    if cli_args.len() > MAX_ARGS { //if too many args
        usage(&cli_args[PROGRAM_POSITION]);
        return Err(FIX_COMMAND_LINE);
    }
    else if cli_args.len() < MIN_ARGS { //if too few args
        usage(&cli_args[PROGRAM_POSITION]);
        return Err(FIX_COMMAND_LINE);
    }
    else if cli_args.len() == MAX_ARGS && cli_args[OPTIONALARG_POSITION] != "whinge"{ //if optional arg is not "whinge"
        usage(&cli_args[PROGRAM_POSITION]);
        return Err(FIX_COMMAND_LINE);
    }
    else if cli_args.len() == MAX_ARGS && cli_args[OPTIONALARG_POSITION] == "whinge"{ //if optional arg is "whinge"
        SHOULD_COMPLAIN.store(true,Ordering::SeqCst);
    }

    *config_filename = cli_args[CONFIG_POSITION].clone();
    Ok(())
}