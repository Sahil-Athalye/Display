//play.rs
//Sahil Athalye: a.sahil@wustl.edu 
//Varad Deouskar: varad@wustl.edu
//declares and implements the Play struct which handles the partial config files

use std::thread;
use std::sync::{Arc, Mutex};
use std::sync::atomic::Ordering;
use std::io::{self, Write};
use crate::lab3::script_gen::grab_trimmed_file_lines;
use crate::lab3::declarations::SCRIPT_GEN_FAILURE;
use crate::lab3::declarations::THREAD_PANIC;
use crate::lab3::declarations::SHOULD_COMPLAIN;

use super::scene_fragment::SceneFragment;

type ScriptConfig = Vec<(bool, String)>;
type Fragments = Vec<Arc<Mutex<SceneFragment>>>;

const ONE_TOKEN:usize = 1;


#[derive(Debug)]
pub struct Play{
    fragments:Fragments,
}

impl Play{
    pub fn new() -> Self {
        let fragments:Fragments = Vec::new();
        return Self { fragments};
    }

    pub fn process_config(&mut self, play_config: &ScriptConfig) -> Result<(), u8> {
        let mut current_title = String::new();
        let mut thread_handles = Vec::new();
        
        for (is_title, text) in play_config {
            if *is_title {
                // Update the current title if this is a scene title
                current_title = text.clone();
            } else {
                // Capture variables for thread
                let title = current_title.clone();
                let config_text = text.clone();
                
                // Spawn a thread to prepare each fragment
                let handle = thread::spawn(move || {
                    let mut local_title = title;
                    let mut fragment = SceneFragment::new(&local_title);
                    
                    // Prepare method now panics on error
                    fragment.prepare(&config_text, &mut local_title)
                        .expect("Failed to prepare scene fragment");
                    
                    fragment
                });
                
                thread_handles.push(handle);
                current_title = String::new(); // Reset title after fragment creation
            }
        }
        
        // Join all threads and collect results
        for handle in thread_handles {
            match handle.join() {
                Ok(fragment) => {
                    // Wrap the fragment in Arc<Mutex> and push it into the vector
                    let wrapped_fragment = Arc::new(Mutex::new(fragment));
                    self.fragments.push(wrapped_fragment);
                },
                Err(_) => {
                    // If any thread panicked, return an error
                    return Err(THREAD_PANIC);
                }
            }
        }
        
        Ok(())
    }

    fn add_config(config_line: &String, play_config: &mut ScriptConfig) {
        // Skip blank lines
        if config_line.trim().is_empty() {
            return;
        }
    
        let tokens: Vec<&str> = config_line.split_whitespace().collect();
        
        // Skip empty token lists (shouldn't happen with trim but being defensive)
        if tokens.is_empty() {
            return;
        }
    
        // Handle [scene] directives
        if tokens[0] == "[scene]" {
            if tokens.len() == ONE_TOKEN {
                // [scene] with no title
                if SHOULD_COMPLAIN.load(Ordering::SeqCst) {
                    if let Err(e) = writeln!(io::stderr().lock(), "Missing scene title after [scene]"){
                        let _ = writeln!(io::stderr().lock(), "Failed to write to stderr: {}", e);
                    }
                }
                return;
            }
            // Concatenate remaining tokens as scene title
            let scene_title = tokens[1..].join(" ");
            play_config.push((true, scene_title));
            return;
        }
    
        // Handle configuration file lines
        let config_file = tokens[0].to_string();
        if tokens.len() > ONE_TOKEN && SHOULD_COMPLAIN.load(Ordering::SeqCst) {
            if let Err(e) = writeln!(io::stderr().lock(), "Additional tokens after configuration file name: {}", config_line){
                let _ = writeln!(io::stderr().lock(), "Failed to write to stderr: {}", e);
            }
        }
        play_config.push((false, config_file));
    }

    fn read_config(script_name: &String, play_config: &mut ScriptConfig) -> Result<(), u8> {
        let mut lines_read: Vec<String> = Vec::new();
        
        // Attempt to read lines from the script file
        if let Err(_e) = grab_trimmed_file_lines(script_name, &mut lines_read) {
            if let Err(e) = writeln!(io::stderr().lock(), "Failed to open or read from script file: {}", script_name){
                let _ = writeln!(io::stderr().lock(), "Failed to write to stderr: {}", e);
            }
            return Err(SCRIPT_GEN_FAILURE);
        }
    
        // Check if any lines were read
        if lines_read.is_empty() {
            if let Err(e) = writeln!(io::stderr().lock(), "No lines read from script file: {}", script_name){
                let _ = writeln!(io::stderr().lock(), "Failed to write to stderr: {}", e);
            }
            return Err(SCRIPT_GEN_FAILURE);
        }
    
        // Process each line using add_config
        for line in lines_read {
            Self::add_config(&line, play_config);
        }
    
        Ok(())
    }

    pub fn prepare(&mut self, script_name: &String) -> Result<(), u8> {
        // Create a new empty ScriptConfig
        let mut script_config: ScriptConfig = ScriptConfig::new();
    
        // Read and process the configuration using the new read_config signature
        if let Err(_e) = Self::read_config(script_name, &mut script_config) {
            return Err(SCRIPT_GEN_FAILURE);
        }
    
        // Process the configuration to set up the play
        if let Err(_e) = self.process_config(&script_config) {
            return Err(SCRIPT_GEN_FAILURE);
        }
    
        // Validate that we have fragments and the first fragment has a title
        if self.fragments.is_empty() {
            if let Err(e) = writeln!(io::stderr().lock(), "No scene fragments were created from script file: {}", script_name){
                let _ = writeln!(io::stderr().lock(), "Failed to write to stderr: {}", e);
            }
            return Err(SCRIPT_GEN_FAILURE);
        }

        // Check if the first fragment has a title
        let _ = match self.fragments.first().map(|arc_mutex| arc_mutex.lock()) {
            Some(Ok(ref fragment)) if !fragment.scene_title.is_empty() => Ok::<(), u8>(()),
            Some(Ok(_)) => {
                writeln!(io::stderr().lock(), "First scene fragment is missing a title in script file: {}", script_name)
                    .ok();
                return Err(SCRIPT_GEN_FAILURE);
            }
            Some(Err(_)) => {
                writeln!(io::stderr().lock(), "Failed to lock the first fragment in script file: {}", script_name).ok();
                return Err(SCRIPT_GEN_FAILURE);
            }
            None => {
                writeln!(io::stderr().lock(), "No fragments present in the script file: {}", script_name).ok();
                return Err(SCRIPT_GEN_FAILURE);
            }
        };
    
        Ok(())
    }

    pub fn recite(&mut self) {
        // Do nothing if there are no fragments
        if self.fragments.is_empty() {
            return;
        }

        let last_index = self.fragments.len() - 1;

        for i in 0..=last_index {
            if i == 0 {
                // First fragment: use enter_all
                match self.fragments[0].lock() {
                    Ok(fragment) => fragment.enter_all(),
                    Err(e) => {
                        writeln!(io::stderr().lock(), "Failed to lock first fragment: {}", e).ok();
                        return;
                    }
                }
            } else {
                // Previous and current fragment indices
                let prev_idx = i - 1;
                let curr_idx = i;

                // Match on both previous and current fragments
                match (
                    self.fragments[prev_idx].lock(),
                    self.fragments[curr_idx].lock(),
                ) {
                    (Ok(ref prev_fragment), Ok(current_fragment)) => {
                        current_fragment.enter(&*prev_fragment);
                    }
                    (Err(e), _) | (_, Err(e)) => {
                        writeln!(io::stderr().lock(), "Failed to lock fragment: {}", e).ok();
                        return;
                    }
                }
            }

            // Recite the current fragment
            match self.fragments[i].lock() {
                Ok(mut fragment) => fragment.recite(),
                Err(e) => {
                    writeln!(io::stderr().lock(), "Failed to lock fragment {}: {}", i, e).ok();
                    return;
                }
            }

            if i == last_index {
                // Last fragment: use exit_all
                match self.fragments[i].lock() {
                    Ok(fragment) => fragment.exit_all(),
                    Err(e) => {
                        writeln!(io::stderr().lock(), "Failed to lock last fragment: {}", e).ok();
                        return;
                    }
                }
            } else {
                // Current and next fragment indices
                let curr_idx = i;
                let next_idx = i + 1;

                // Match on both current and next fragments
                match (
                    self.fragments[curr_idx].lock(),
                    self.fragments[next_idx].lock(),
                ) {
                    (Ok(current_fragment), Ok(ref next_fragment)) => {
                        current_fragment.exit(&*next_fragment);
                    }
                    (Err(e), _) | (_, Err(e)) => {
                        writeln!(io::stderr().lock(), "Failed to lock fragment: {}", e).ok();
                        return;
                    }
                }
            }
        }
    }



}