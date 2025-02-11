// script_gen.rs
// Sahil Athalye: a.sahil@wustl.edu
// Varad Deouskar: varad@wustl.edu
// Implements the grab trimmed file lines function and get_buffered_reader

use std::fs::File;
use std::io::{self, BufRead, BufReader, Write};
use std::net::{TcpStream, Shutdown};

use crate::lab3::declarations::SCRIPT_GEN_FAILURE;

const DONE_READING: usize = 0;
const ADDR_INDEX :usize = 1;
const PORT_INDEX:usize = 2;


pub fn get_buffered_reader(name: &String) -> Result<BufReader<Box<dyn std::io::Read>>, u8> {
    // Check if the name matches the pattern: net:<dotted_decimal_addr>:<port>:<filename>
    // Example: net:127.0.0.1:7777:partial_macbeth_act_i_script.txt
    let parts: Vec<&str> = name.split(':').collect();

    if parts.len() >= 4 && parts[0] == "net" {
        // Extract address, port, and filename
        let addr = parts[ADDR_INDEX];
        let port = parts[PORT_INDEX];
        let file_name = parts[3..].join(":"); // join in case filename has colons

        // Construct the socket address string
        let socket_addr = format!("{}:{}", addr, port);

        // Attempt to connect to the server
        let mut stream = match TcpStream::connect(&socket_addr) {
            Ok(s) => s,
            Err(_e) => {
                if let Err(e) = writeln!(io::stdout().lock(), "Failed to connect to {}", socket_addr) {
                    let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
                }
                return Err(SCRIPT_GEN_FAILURE);
            }
        };

        // Send the filename to the server
        if let Err(_e) = stream.write_all(file_name.as_bytes()) {
            if let Err(e) = writeln!(io::stdout().lock(), "Failed to send filename to server") {
                let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
            }
            return Err(SCRIPT_GEN_FAILURE);
        }

        // Close the write side of the connection to signal we're done sending data
        stream.shutdown(Shutdown::Write).expect("could not shutdown");

        // Now use the stream as a reader
        let reader:BufReader<Box<dyn std::io::Read>> = BufReader::new(Box::new(stream));
        return Ok(reader);
    } else {
        // Not a network string, treat as file name
        match File::open(name) {
            Err(_e) => {
                if let Err(e) = writeln!(io::stdout().lock(), "couldn't open file named {}", name) {
                    let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
                }
                Err(SCRIPT_GEN_FAILURE)
            }
            Ok(file) => Ok(BufReader::new(Box::new(file)))
        }
    }
}

pub fn grab_trimmed_file_lines(file_name: &String, lines: &mut Vec<String>) -> Result<(), u8> {
    let mut reader = match get_buffered_reader(file_name) {
        Ok(r) => r,
        Err(e) => return Err(e),
    };

    let mut lines_read = String::new();

    loop {
        match reader.read_line(&mut lines_read) {
            Ok(res) => {
                if res == DONE_READING {
                    // Here the reader (and underlying stream) will go out of scope 
                    // and be closed automatically upon returning
                    return Ok(());
                }

                lines.push(lines_read.trim().to_string());
                lines_read.clear();
            },
            Err(_e) => return Err(SCRIPT_GEN_FAILURE)
        }
    }
}
