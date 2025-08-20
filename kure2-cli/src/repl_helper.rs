use std::{cell::RefCell, rc::Rc};

use kure2::lang;
use rustyline::{
    Helper, Result,
    completion::{Completer, FilenameCompleter, Pair},
    highlight::Highlighter,
    hint::Hinter,
    validate::Validator,
};

use crate::{Repl, commands::Edge};

pub struct ReplHelper {
    filename_completer: FilenameCompleter,
    repl: Rc<RefCell<Repl>>,
}

impl ReplHelper {
    pub fn new(repl: Rc<RefCell<Repl>>) -> Self {
        Self {
            filename_completer: FilenameCompleter::new(),
            repl,
        }
    }

    fn complete_command(&self, line: &str, pos: usize) -> Result<(usize, Vec<Pair>)> {
        let repl = self.repl.borrow();

        let line = &line[..pos];
        let mut args = line.split_whitespace().collect::<Vec<_>>();
        // There is at-least one char because the line starts with a dot.
        if char::is_whitespace(line.chars().last().unwrap()) {
            // If the last character is whitespace, we should complete a new empty part.
            args.push("");
        }

        // There is at-least one part because the line starts with a dot.
        let Some(node) = repl.commands.traverse(&args[..args.len() - 1]) else {
            return Ok((pos, Vec::new()));
        };

        let last_part = args.last().unwrap();
        let last_part_start = line.rfind(last_part).unwrap_or(0);
        let mut suggestions = Vec::new();

        let keywords = node.next_keywords_by_prefix(last_part).map(|k| Pair {
            display: k.to_owned(),
            replacement: k.to_owned(),
        });
        suggestions.extend(keywords);

        if node.edges.contains_key(&Edge::Filename) {
            let (start, filenames) = self.filename_completer.complete_path_unsorted(line, pos)?;
            debug_assert_eq!(start, last_part_start);
            suggestions.extend(filenames);
        }

        if node.edges.contains_key(&Edge::Variable) {
            let variables = repl.state.list_relations();
            suggestions.extend(
                variables
                    .into_iter()
                    .filter(|v| v.starts_with(last_part))
                    .map(|v| Pair {
                        display: v.clone(),
                        replacement: v,
                    }),
            );
        }

        suggestions.sort_by(|a, b| a.display.cmp(&b.display));
        Ok((last_part_start, suggestions))
    }

    fn complete_statement(&self, line: &str, pos: usize) -> Result<(usize, Vec<Pair>)> {
        let mut word_start = pos;
        while let Some(c) = line[..word_start]
            .chars()
            .last()
            .filter(|&c| c.is_alphanumeric() || "-_".contains(c))
        {
            word_start -= c.len_utf8();
        }
        let prefix = &line[word_start..pos];

        let repl = self.repl.borrow();
        let mut suggestions = lang::list_builtins()
            .into_iter()
            .chain(repl.state.list_programs())
            .chain(repl.state.list_relations())
            .filter(|a| a.starts_with(prefix))
            .map(|a| Pair {
                display: a.clone(),
                replacement: a,
            })
            .collect::<Vec<_>>();
        suggestions.sort_by_key(|a| a.replacement.clone());

        Ok((word_start, suggestions))
    }
}

impl Completer for ReplHelper {
    type Candidate = Pair;

    fn complete(
        &self,
        line: &str,
        pos: usize,
        _ctx: &rustyline::Context,
    ) -> Result<(usize, Vec<Self::Candidate>)> {
        if line.starts_with('.') {
            self.complete_command(line, pos)
        } else {
            self.complete_statement(line, pos)
        }
    }
}

impl Hinter for ReplHelper {
    type Hint = String;
}

impl Highlighter for ReplHelper {}

impl Validator for ReplHelper {}

impl Helper for ReplHelper {}
