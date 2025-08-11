use kure2::lang::LuaState;
use rustyline::{Editor, error::ReadlineError, history::DefaultHistory};

fn main() {
    let mut rl = Editor::<(), DefaultHistory>::new().unwrap();

    let mut state = LuaState::new();

    loop {
        let line = match rl.readline("> ") {
            Ok(line) => {
                rl.add_history_entry(line.as_str()).unwrap();
                line
            }
            Err(ReadlineError::Interrupted) | Err(ReadlineError::Eof) => break,
            Err(err) => panic!("Error reading line: {err}"),
        };
        let line = line.trim();

        if line.starts_with('{') || line.is_empty() {
            // Do nothing
        } else if let Some((var, expr)) = line.split_once('=') {
            match state.assign(var.trim(), expr.trim()) {
                Ok(()) => {}
                Err(err) => {
                    eprintln!("Error evaluating `{line}`: {err}");
                }
            }
        } else {
            match state.exec(line) {
                Ok(relation) => {
                    println!("{relation}");
                }
                Err(err) => {
                    eprintln!("Error evaluating `{line}`: {err}");
                }
            }
        }
    }
}
