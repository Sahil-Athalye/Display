//return_wrapper.rs
//Sahil Athalye: a.sahil@wustl.edu 
//Varad Deouskar: varad@wustl.edu
//Implemented opening a server


pub mod lab3;
use std::env;
use lab3::server::Server;
use lab3::return_wrapper::ReturnWrapper;

// A custom return type for `main` that implements `Termination` so we can return process exit codes.

fn main() -> ReturnWrapper {
    let args: Vec<String> = env::args().collect();

    // Expect exactly two arguments: [program_name, address]
    if args.len() != 2 {
        eprintln!("usage: {} <IP:PORT>", args[0]);
        return ReturnWrapper::new(1);
    }

    let addr = &args[1];

    let mut server = Server::new();
    if let Err(e) = server.open(addr) {
        eprintln!("Failed to open server on {}: {}", addr, e);
        return ReturnWrapper::new(1);
    }

    server.run();
    return ReturnWrapper::new(0);
}
