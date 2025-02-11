//scene_fragment.rs
//Sahil Athalye: a.sahil@wustl.edu 
//Varad Deouskar: varad@wustl.edu
//Implements Server which handles binding to a port.

use std::fs::File;
use std::io::{BufRead, BufReader, Read, Write};
use std::net::{TcpListener, TcpStream};
use std::sync::atomic::{AtomicBool, Ordering};
use std::thread;

// Global cancellation flag
static CANCEL_FLAG: AtomicBool = AtomicBool::new(false);
const DONE_READING:usize = 0;

pub struct Server {
    listener: Option<TcpListener>,
    listening_addr: String,
}

impl Server {
    pub fn new() -> Self {
        Server {
            listener: None,
            listening_addr: String::new(),
        }
    }

    pub fn is_open(&self) -> bool {
        self.listener.is_some()
    }

    pub fn open(&mut self, addr: &str) -> std::io::Result<()> {
        let tcp_listener = TcpListener::bind(addr)?;
        self.listener = Some(tcp_listener);
        self.listening_addr = addr.to_string();
        Ok(())
    }

    pub fn run(&self) {
        // We can only run if there's a listener
        let listener = match &self.listener {
            Some(l) => l,
            None => return,
        };

        // Main loop: continue as long as CANCEL_FLAG is false and listener is present
        while !CANCEL_FLAG.load(Ordering::SeqCst) && self.is_open() {
            // Attempt to accept a connection
            let (stream, _addr) = match listener.accept() {
                Ok((s, a)) => (s, a),
                Err(_) => continue, // If accept fails, try again
            };

            // Check CANCEL_FLAG immediately after accept
            if CANCEL_FLAG.load(Ordering::SeqCst) {
                return;
            }

            // Spawn a new thread to handle the connection
            thread::spawn(move || {
                if let Err(e) = handle_connection(stream) {
                    eprintln!("Error handling connection: {:?}", e);
                }
            });
        }
    }
}

// Returns an error if something unexpected happens while handling the connection.
fn handle_connection(mut stream: TcpStream) -> std::io::Result<()> {
    let mut reader = BufReader::new(stream.try_clone()?);

    // Read a single line (token) from the connection
    let mut line = String::new();
    let bytes_read = reader.read_line(&mut line)?;
    if bytes_read == DONE_READING {
        // No token received, just return
        return Ok(());
    }

    let token = line.trim();

    println!("Got token - {}",token);

    // If token is "quit", set CANCEL_FLAG to true and return
    if token == "quit" {
        CANCEL_FLAG.store(true, Ordering::SeqCst);
        return Ok(());
    }


    // Check token for any forbidden path characters: '/', '\\', '..', '$'
    if token.contains('/') || token.contains('\\') || token.contains("..") || token.contains('$') {
        // For security reasons, we don't open files with these patterns.
        // Just return, closing the connection.
        return Ok(());
    }


    // Attempt to open the requested file
    let file = match File::open(token) {
        Ok(f) => f,
        Err(_) => {
            // Unable to open file, just close connection
            println!("Could not open the file");
            return Ok(());
        }
    };

    // Read the entire file content
    let mut contents = Vec::new();
    {
        let mut buf_reader = BufReader::new(file);
        buf_reader.read_to_end(&mut contents)?;
    }

    // Write the file contents out over the connection
    stream.write_all(&contents)?;
    stream.flush()?;

    Ok(())
}
