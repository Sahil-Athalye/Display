//scene_fragment.rs
//Sahil Athalye: a.sahil@wustl.edu 
//Varad Deouskar: varad@wustl.edu
//Implements SceneFragment which handles each fragment of a play.

use std::sync::{Arc, Mutex};
use std::cmp::Ordering as OtherOrdering;
use std::io::{self, Write};
use std::sync::atomic::Ordering;
use crate::lab3::script_gen::grab_trimmed_file_lines;
use crate::lab3::declarations::SCRIPT_GEN_FAILURE;
use crate::lab3::declarations::SHOULD_COMPLAIN;
use crate::lab3::player::Player;

type PlayConfig = Vec<(String, String)>;

const CONFIG_LINE_NUM:usize = 2;

const CHAR_NAME_INDEX:usize = 0;
const FILE_NAME_INDEX:usize = 1;
const CONFIG_TOKEN_NUM:usize = 2;

#[derive(Debug)]
pub struct SceneFragment{
    pub scene_title:String,
    pub characters: Vec<Arc<Mutex<Player>>>,
}

impl SceneFragment{
    pub fn new(title: &String) -> Self {
        let players: Vec<Arc<Mutex<Player>>> = Vec::new();
        Self { scene_title: title.clone(), characters: players }
    }

    fn print_title_if_nonempty(&self) {
        if !self.scene_title.trim().is_empty() {
            if let Err(e) = writeln!(io::stdout().lock(), "{}", self.scene_title) {
                let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
            }
        }
    }

    pub fn process_config(&mut self, play_config: &PlayConfig) -> Result<(), u8> {
        let mut thread_handles = Vec::new();
    
        for (character_name, file_name) in play_config {
            // Create a new Player instance with the character name.
            let mut player = Player::new(character_name.clone());
            
            // Create the file name for the thread
            let thread_file_name = file_name.clone();
            
            // Spawn a thread to prepare the player
            let player_handle = std::thread::spawn(move || {
                player.prepare(&thread_file_name)?;
                Ok::<Player, u8>(player) // Explicitly specify the Result type
            });
            
            // Store the thread handle
            thread_handles.push((character_name.clone(), player_handle));
        }
        
        // Join all threads and collect the prepared players
        for (_charactername, handle) in thread_handles {
            let prepared_player = handle.join().unwrap()?; // This will panic if the thread panicked
            self.characters.push(Arc::new(Mutex::new(prepared_player)));
        }
        
        Ok(())
    }

    fn add_config(config_line:&String, play_config:&mut PlayConfig){
        let whitespace_tokens:Vec<&str> = (config_line.split_whitespace()).collect(); //split config lines into their two strings

        if whitespace_tokens.len() < CONFIG_TOKEN_NUM {
            if SHOULD_COMPLAIN.load(Ordering::SeqCst){
                if let Err(e) = writeln!(io::stderr().lock(), "Less than two tokens in config line {}",config_line) {
                    let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
                }
            }
        }
    
        if whitespace_tokens.len() > CONFIG_TOKEN_NUM {
            if SHOULD_COMPLAIN.load(Ordering::SeqCst){
                if let Err(e) = writeln!(io::stderr().lock(), "More than two tokens in config line {}",config_line) {
                    let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
                }
            }
        }
    
        if whitespace_tokens.len() >= CONFIG_TOKEN_NUM { 
            play_config.push((whitespace_tokens[CHAR_NAME_INDEX].to_string(),whitespace_tokens[FILE_NAME_INDEX].to_string()));
        }
    }

    fn read_config(config_name:&String, _play_title:&mut String, play_config:&mut PlayConfig) -> Result<(), u8>{
        let mut lines_read:Vec<String> = Vec::new();

        if let Err(_e) =  grab_trimmed_file_lines(&config_name, &mut lines_read){
            return Err(SCRIPT_GEN_FAILURE);
        }

        if lines_read.len() < CONFIG_LINE_NUM {
            return Err(SCRIPT_GEN_FAILURE);
        }
        else{
            for line in lines_read{
                Self::add_config(&line,play_config);

            }
        }
        return Ok(());
    }

    pub fn compare_players(player1: &Arc<Mutex<Player>>, player2: &Arc<Mutex<Player>>) -> OtherOrdering {
        // Try to lock both players.
        match (player1.lock(), player2.lock()) {
            (Ok(ref player1_locked), Ok(ref player2_locked)) => {
                // If both locks are successful, compare the players.
                match player1_locked.partial_cmp(player2_locked) {
                    Some(ordering) => ordering, // Return the partial comparison result.
                    None => OtherOrdering::Equal,    // If partial_cmp is None, consider them equal.
                }
            }
            _ => OtherOrdering::Equal, // If either lock fails, consider them equal.
        }
    }

    pub fn prepare(&mut self, config_name: &String, play_name: &mut String) -> Result<(), u8> {
        // Initialize an empty PlayConfig.
        let mut play_config: PlayConfig = PlayConfig::new();

        // Read the configuration file and populate play_name and play_config.
        if let Err(_e) = Self::read_config(config_name, play_name, &mut play_config) {
            // Instead of returning an error, panic
            panic!();
        }

        // Process the configuration to set up the play.
        if let Err(_e) = self.process_config(&play_config) {
            // Instead of returning an error, panic
            panic!();
        }

        // Sort the characters based on their first line numbers using sort_by
        self.characters.sort_by(|player1, player2| {
            SceneFragment::compare_players(player1, player2)
        });

        Ok(())
    }

    pub fn recite(&mut self) {
        let mut current_character = String::new();
        let mut expected_line_number = 0;
    
        loop {
            // Find the next player with the lowest line number to speak.
            let mut next_player: Option<&Arc<Mutex<Player>>> = None;
            let mut next_line_number = usize::MAX;
    
            for player in &self.characters {
                // Lock the player to get a mutable reference to the Player
                if let Ok(player_guard) = player.lock() {
                    if let Some(line_number) = player_guard.next_line() {
                        // Check for the lowest line number in the players.
                        if line_number < next_line_number {
                            next_player = Some(player);  // Store the Arc<Mutex<Player>>
                            next_line_number = line_number;
                        }
                    }
                }
            }
    
            // If no player has a next line, we are done reciting.
            if next_player.is_none() {
                break;
            }
    
            // Handle missing lines if `SHOULD_COMPLAIN` mode is on.
            if next_line_number > expected_line_number {
                if SHOULD_COMPLAIN.load(Ordering::SeqCst) {
                    for missing_line in expected_line_number..next_line_number {
                        if let Err(e) = writeln!(io::stderr().lock(), "Warning: Missing line number {}", missing_line) {
                            let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
                        }
                    }
                }
            } else if next_line_number < expected_line_number && SHOULD_COMPLAIN.load(Ordering::SeqCst) {
                // Complain about duplicate line numbers if they are lower than expected.
                if let Err(e) = writeln!(io::stderr().lock(), "Warning: Duplicate line number {}", next_line_number) {
                    let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
                }
            }
    
            // Have the selected player speak their next line, updating the current character's name.
            if let Some(player) = next_player {
                // Lock the player again to call speak
                if let Ok(mut player_guard) = player.lock() {
                    player_guard.speak(&mut current_character);
                }
            }
    
            // Update the expected line number to the next one.
            expected_line_number = next_line_number + 1;
        }
    }

    pub fn enter(&self, previous: &SceneFragment) {
        self.print_title_if_nonempty();
    
        for current_player in &self.characters {
            // Lock the current player to get a reference.
            if let Ok(current_player_guard) = current_player.lock() {
                // Check if the player was not in the previous scene
                if !previous.characters.iter().any(|prev_player| {
                    if let Ok(prev_player_guard) = prev_player.lock() {
                        prev_player_guard.name == current_player_guard.name
                    } else {
                        false
                    }
                }) {
                    if let Err(e) = writeln!(io::stdout().lock(), "[Enter {}.]", current_player_guard.name) {
                        let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
                    }
                }
            }
        }
    }

    pub fn enter_all(&self) {
        self.print_title_if_nonempty();
    
        for player in &self.characters {
            // Lock the player to get a reference.
            if let Ok(player_guard) = player.lock() {
                if let Err(e) = writeln!(io::stdout().lock(), "[Enter {}.]", player_guard.name) {
                    let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
                }
            }
        }
    }

    pub fn exit(&self, next: &SceneFragment) {
        // Iterate through characters in reverse order
        for current_player in self.characters.iter().rev() {
            // Lock the current player to get a reference.
            if let Ok(current_player_guard) = current_player.lock() {
                // Check if the player is not in the next scene
                if !next.characters.iter().any(|next_player| {
                    if let Ok(next_player_guard) = next_player.lock() {
                        next_player_guard.name == current_player_guard.name
                    } else {
                        false
                    }
                }) {
                    if let Err(e) = writeln!(io::stdout().lock(), "[Exit {}.]", current_player_guard.name) {
                        let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
                    }
                }
            }
        }
    }

    pub fn exit_all(&self) {
        // Iterate through characters in reverse order
        for player in self.characters.iter().rev() {
            // Lock the player to get a reference.
            if let Ok(player_guard) = player.lock() {
                if let Err(e) = writeln!(io::stdout().lock(), "[Exit {}.]", player_guard.name) {
                    let _ = writeln!(io::stderr().lock(), "Failed to write to stdout: {}", e);
                }
            }
        }
    }



}