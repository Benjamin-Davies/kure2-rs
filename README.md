# kure2-rs
This repo houses a Rust wrapper for [Kure](https://www.informatik.uni-kiel.de/~progsys/kure2/), a C library which provides manipulation of relations and operations of [relation algebra](http://en.wikipedia.org/wiki/Relation_algebra) using a fast implementation using [binary decision diagrams](http://en.wikipedia.org/wiki/Binary_decision_diagram). It also contains `kure2-cli`, a CLI for using Kure's domain-specific language.

Historically, the Kure library originates from the [RelView](https://www.informatik.uni-kiel.de/~progsys/relview/) tool. Today, Kure is a standalone library that does not depend on any part of the RelView tool. `kure2-cli` is intended to replicate most of the features of RelView, allowing them to be easily used on modern macOS and Linux systems.

# Usage
To use kure2-rs, you must have the [Rust toolchain](https://www.rust-lang.org/tools/install) installed.

## Command Line Interface
To download `kure2-cli`, run:

```sh
cargo install --git https://github.com/Benjamin-Davies/kure2-rs.git
```

You can then launch the REPL by running `kure` in your terminal.

The REPL also support various commands (starting with a `.`) to help with loading and saving programs and relations.

## Library
Using the `kure2` crate should be straightforward. You can create a relation using the `Relation` struct. You can also execute DSL code using an instance of the `lang::State` struct.

# Licensing
The crates in this repository are licensed under the [MIT License](https://choosealicense.com/licenses/mit/), except for those that directly or indirectly dependent on Kure, which are licensed under [GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/). The vendored C libraries (Cudd, Kure and Lua), and any patches I have made to them, are licensed under the same license as the original libraries (i.e. BSD 3-Clause, GPLv3 and MIT, respectively).
