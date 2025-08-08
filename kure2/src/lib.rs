//! Rust wrapper for [Kure](https://www.informatik.uni-kiel.de/~progsys/kure2/), a C library which
//! provides manipulation of relations and operations of
//! [relation algebra](http://en.wikipedia.org/wiki/Relation_algebra) using a fast implementation
//! using [binary decision diagrams](http://en.wikipedia.org/wiki/Binary_decision_diagram).
//! It provides a [Lua](http://www.lua.org/) based domain-specific embedded programming language to
//! conveniently manipulate and work with relations. For instance, given relations R, S, the tupling
//! of them can be computed using \[R,S\] and they can be composed using R*S.
//!
//! Historically, the Kure library originates from the RelView tool. Today, Kure is a standalone
//! library that does not depend on any part of the RelView tool.
// Re-worded from the original docs

#![warn(missing_docs)]

mod context;
mod error;
mod lang;
mod relation;

pub use crate::{
    context::{Context, context},
    error::Error,
    lang::LuaState,
    relation::Relation,
};
