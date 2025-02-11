//main.rs
//Sahil Athalye: a.sahil@wustl.edu 
//Varad Deouskar: varad@wustl.edu
//Client Testing main file

use std::env;
use std::process::exit;
use std::io::{BufRead, BufReader, Write};
use std::net::TcpStream;
use std::time::Duration;
use std::thread;
const ERROR_CODE_FAILURE:i32= 1;

const ADDR_INDEX :usize = 1;
const TOKEN_INDEX:usize = 2;

fn main() {
    let args: Vec<String> = env::args().collect();

    // Expect exactly three arguments: [program_name, address, token]
    if args.len() != 3 {
        eprintln!("usage: {} <IP:PORT> <token>", args[0]);
        exit(ERROR_CODE_FAILURE);
    }

    let addr = &args[ADDR_INDEX];
    let token = &args[TOKEN_INDEX];

    if token == "quit" {
        // Connect once, send "quit"
        match TcpStream::connect(addr) {
            Ok(mut stream) => {
                println!("Connected");
                if let Err(e) = writeln!(stream, "quit") {
                    eprintln!("Failed to send quit token to server: {}", e);
                    exit(ERROR_CODE_FAILURE);
                }
            }
            Err(e) => {
                eprintln!("Failed to connect to server {}: {}", addr, e);
                exit(ERROR_CODE_FAILURE);
            }
        }

        // Sleep one second
        let one_second = Duration::from_secs(1);
        thread::sleep(one_second);

        // Connect again to wake up the server from accept
        let _ = TcpStream::connect(addr);

        // Return successfully
        return;
    }

    // Otherwise, connect and send the token
    let mut stream = match TcpStream::connect(addr) {
        Ok(s) => s,
        Err(e) => {
            eprintln!("Failed to connect to server {}: {}", addr, e);
            exit(ERROR_CODE_FAILURE);
        }
    };

    if let Err(e) = writeln!(stream, "{}", token) {
        eprintln!("Failed to send token {} to server: {}", token, e);
        exit(ERROR_CODE_FAILURE);
    }

    // If token was not "quit", read lines from the connection and print them out
    let reader = BufReader::new(stream);
    for line_result in reader.lines() {
        match line_result {
            Ok(line) => println!("{}", line),
            Err(_) => break, // Connection likely closed by server
        }
    }

    // Return successfully
}
