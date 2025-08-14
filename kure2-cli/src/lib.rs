use std::{io, ops};

use kure2::lang::LuaState;

use crate::commands::Node;

pub mod commands;
pub mod repl_helper;

const WELCOME_MESSAGE: &str = "Welcome to the kure2-rs REPL! Type '.help' for assistance.";

pub struct Repl {
    pub commands: Node,
    pub state: LuaState,
}

impl Repl {
    pub fn new() -> Self {
        Self {
            commands: Node::root(),
            state: LuaState::new(),
        }
    }

    pub fn welcome(&self, mut out: impl io::Write) -> io::Result<()> {
        writeln!(out, "{WELCOME_MESSAGE}")?;
        Ok(())
    }

    pub fn process_input(
        &mut self,
        line: &str,
        mut out: impl io::Write,
    ) -> io::Result<ops::ControlFlow<()>> {
        let line = line.trim();
        if line.is_empty() || line.starts_with('{') {
            Ok(ops::ControlFlow::Continue(()))
        } else if line.starts_with('.') {
            self.process_command(line, &mut out)
        } else if let Some((var, expr)) = line.split_once('=') {
            let result = self.state.assign(var.trim(), expr.trim());
            match result {
                Ok(()) => {}
                Err(e) => writeln!(out, "Error: {e}")?,
            }

            Ok(ops::ControlFlow::Continue(()))
        } else {
            let result = self.state.exec(line);
            match result {
                Ok(value) => {
                    // Relation::display already adds a newline.
                    write!(out, "{}", value.display("<expr>"))?;
                }
                Err(e) => writeln!(out, "Error: {e}")?,
            }

            Ok(ops::ControlFlow::Continue(()))
        }
    }

    fn process_command(
        &mut self,
        command: &str,
        mut out: impl io::Write,
    ) -> io::Result<ops::ControlFlow<()>> {
        let args = command.split_whitespace().collect::<Vec<_>>();

        let Some(node) = self.commands.traverse(&args) else {
            writeln!(out, "Unknown command or invalid syntax")?;
            return Ok(ops::ControlFlow::Continue(()));
        };
        let Some(func) = &node.func else {
            writeln!(out, "Incomplete command")?;
            return Ok(ops::ControlFlow::Continue(()));
        };

        func(&mut self.state, &mut out, &args)
    }
}
