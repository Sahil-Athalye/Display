//player.rs
//Sahil Athalye: a.sahil@wustl.edu 
//Varad Deouskar: varad@wustl.edu
//declares and implements the player struct which handles the individual speakers

use crate::lab3::script_gen::grab_trimmed_file_lines;
use std::sync::atomic::Ordering;
use std::io::{self, Write};
use crate::lab3::declarations::SHOULD_COMPLAIN;

use std::cmp::Ordering as CmpOrdering;

const MIN_LINE_LEN:usize = 1;

//usize for line number and String for line itself
pub type PlayLines = Vec<(usize, String)>;

#[derive(Debug)]
pub struct Player{
    pub name:String,
    pub lines:PlayLines,
    pub cur_idx:usize,
}

impl Player{
    pub fn new(character_name:String) -> Self {
        let play_lines:PlayLines = Vec::new();
        Self { name:character_name, lines:play_lines, cur_idx:0}
    }

    fn add_script_line(&mut self, unparsed_line: &String) {
        if unparsed_line.len() >= MIN_LINE_LEN {
            if let Some((token, rest)) = unparsed_line.split_once(char::is_whitespace) { //split line
                let trimmed_rest: String = rest.trim().to_string(); //trim whitespace
                if let Ok(number) = token.parse::<usize>() { //parse first string (line #) into usize
                    self.lines.push((number, trimmed_rest));
                } else if SHOULD_COMPLAIN.load(Ordering::SeqCst) { //if first string cannot be parsed into usize
                    if let Err(e) = writeln!(io::stderr().lock(), "Token {} does not represent a valid usize value.", token){
                        let _ = writeln!(io::stderr().lock(), "Failed to write to stderr: {}", e);
                    }
                }
            }
        }
    }

    pub fn prepare(&mut self, file_name: &String) -> Result<(), u8> {
        let mut lines_read: Vec<String> = Vec::new();
    
        // Attempt to read and trim lines from the specified file.
        if let Err(_e) = grab_trimmed_file_lines(file_name, &mut lines_read) {
            panic!(); // Changed to panic instead of returning an error
        }
    
        // Process each line read from the file using add_script_line.
        for line in &lines_read {
            self.add_script_line(line);
        }
    
        // Sort the lines container by line number.
        self.lines.sort_by_key(|(number, _)| *number);
    
        Ok(())
    }


    pub fn speak(&mut self, last_speaker: &mut String) {
        // Check if the current index is within the bounds of the lines.
        if self.cur_idx >= self.lines.len() {
            return; // Simply return if out of bounds.
        }

        // Check if the last speaker was different.
        if *last_speaker != self.name {
            // Update the last speaker to the current player's name.
            *last_speaker = self.name.clone();
            // Print a blank line and then the player's name.
            if let Err(e) = writeln!(io::stdout().lock(), "") {
                let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
            }
            if let Err(e) = writeln!(io::stdout().lock(), "{}", self.name) {
                let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
            }
        }

        // Print the line at the current index.
        if let Err(e) = writeln!(io::stdout().lock(), "{}", self.lines[self.cur_idx].1) {
            let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
        }
        
        // Move to the next line.
        self.cur_idx += 1;
    }

    pub fn next_line(&self) -> Option<usize> {
        // Check if the current index is within the bounds of the lines.
        if self.cur_idx < self.lines.len() {
            // Return the line number at the current index.
            Some(self.lines[self.cur_idx].0)
        } else {
            // Return None if out of bounds.
            None
        }
    }

}

impl PartialEq for Player {
    fn eq(&self, other: &Self) -> bool {
        match (self.lines.get(0), other.lines.get(0)) {
            // Both players have no lines (both are silent)
            (None, None) => true,
            // One has lines while other doesn't, so not equal
            (Some(_), None) | (None, Some(_)) => false,
            // Both have lines, compare their first line numbers
            (Some((self_num, _)), Some((other_num, _))) => self_num == other_num,
        }
    }
}

// Implementing Eq is required for Ord
// Since we have a sound PartialEq implementation that is reflexive, symmetric, and transitive,
// we can safely implement Eq
impl Eq for Player {}

impl PartialOrd for Player {
    fn partial_cmp(&self, other: &Self) -> Option<CmpOrdering> {
        Some(self.cmp(other))
    }
}

impl Ord for Player {
    fn cmp(&self, other: &Self) -> CmpOrdering {
        match (self.lines.get(0), other.lines.get(0)) {
            // Both have no lines - they're equal
            (None, None) => CmpOrdering::Equal,
            // If self has no lines and other has lines, self is less
            (None, Some(_)) => CmpOrdering::Less,
            // If self has lines and other has none, self is greater
            (Some(_), None) => CmpOrdering::Greater,
            // Both have lines, compare their first line numbers
            (Some((self_num, _)), Some((other_num, _))) => self_num.cmp(other_num),
        }
    }
}