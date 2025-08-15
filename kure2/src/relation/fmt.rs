//! Formatting and parsing for [`Relation`].
//!
//! This functionality is not a part of kure2, but it is implemented here for compatibility with
//! other kure2 front-ends.

use std::fmt;

use crate::{Error, Relation};

/// Temporary struct returned by [`Relation::display`].
pub struct Display<'a> {
    relation: &'a Relation,
    name: &'a str,
}

/// Temporary struct returned by [`Relation::display_matrix`].
pub struct DisplayMatrix<'a> {
    relation: &'a Relation,
}

impl Relation {
    /// Displays this relation with the given name.
    pub fn display<'a>(&'a self, name: &'a str) -> Display<'a> {
        Display {
            relation: self,
            name,
        }
    }

    /// Displays this relation as a matrix.
    pub fn display_matrix<'a>(&'a self) -> DisplayMatrix<'a> {
        DisplayMatrix { relation: self }
    }
}

impl fmt::Display for Display<'_> {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let name = self.name;
        let rows = self.relation.rows_i32();
        let cols = self.relation.cols_i32();
        write!(f, "{name} ({rows}, {cols})")?;

        for i in 0..rows {
            let mut last_i = -1;
            for j in 0..cols {
                if self.relation.bit_i32(i, j) {
                    if i != last_i {
                        write!(f, "\n{} : {}", i + 1, j + 1)?;
                    } else {
                        write!(f, ", {}", j + 1)?;
                    }
                    last_i = i;
                }
            }
        }
        writeln!(f)?;

        Ok(())
    }
}

impl fmt::Display for DisplayMatrix<'_> {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let rows = self.relation.rows_i32();
        let cols = self.relation.cols_i32();

        write!(f, "+")?;
        for _ in 0..cols {
            write!(f, "-")?;
        }
        writeln!(f, "+")?;

        for i in 0..rows {
            write!(f, "|")?;
            for j in 0..cols {
                if self.relation.bit_i32(i, j) {
                    write!(f, "X")?;
                } else {
                    write!(f, " ")?;
                }
            }
            writeln!(f, "|")?;
        }

        write!(f, "+")?;
        for _ in 0..cols {
            write!(f, "-")?;
        }
        writeln!(f, "+")?;

        Ok(())
    }
}

impl fmt::Debug for Display<'_> {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt::Display::fmt(self, f)
    }
}

impl fmt::Display for Relation {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt::Display::fmt(&self.display("Relation"), f)
    }
}

impl fmt::Debug for Relation {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt::Display::fmt(self, f)
    }
}

/// Parses a relation from a string in RelView ASCII format.
pub fn parse_relation(input: &str) -> Result<(&str, Relation), Error> {
    let (name, rest) = input.split_once("(").ok_or("Missing `(`")?;
    let name = name.trim();

    let (rows, rest) = rest.split_once(",").ok_or("Missing `,`")?;
    let rows = rows.trim().parse().map_err(|_| "Invalid row count")?;
    let (cols, rest) = rest.split_once(")\n").ok_or("Missing `)`")?;
    let cols = cols.trim().parse().map_err(|_| "Invalid column count")?;
    let rest = rest.trim_start();

    let mut relation = Relation::empty_i32(rows, cols);
    for line in rest.lines() {
        let (i, rest) = line.split_once(':').ok_or("Missing `:`")?;
        let i = i.trim().parse::<i32>().map_err(|_| "Invalid row index")?;
        if i < 1 || i > rows {
            return Err("Row index out of bounds".into());
        }

        for j in rest.split(',') {
            let j = j
                .trim()
                .parse::<i32>()
                .map_err(|_| "Invalid column index")?;
            if j < 1 || j > cols {
                return Err("Column index out of bounds".into());
            }

            relation.set_bit_i32(i - 1, j - 1, true);
        }
    }

    Ok((name, relation))
}

/// Parses a relation from a string in matrix format.
pub fn parse_matrix(input: &str) -> Result<Relation, Error> {
    let mut lines = input.lines();

    let first_line = lines.next().ok_or("Empty input")?;
    let cols = (first_line.chars().count() as i32) - 2;

    let mut values = Vec::new();
    for line in lines.take_while(|l| l.starts_with('|')) {
        let line = line.trim_start_matches('|').trim_end_matches('|');

        let mut row = Vec::new();
        for c in line.chars() {
            match c {
                'X' => row.push(true),
                ' ' => row.push(false),
                _ => return Err("Invalid character in matrix".into()),
            }
        }

        if row.len() as i32 > cols {
            return Err("Row is too wide".into());
        }

        values.push(row);
    }

    let rows = values.len() as i32;
    let mut relation = Relation::empty_i32(rows, cols);
    for (i, row) in values.iter().enumerate() {
        for (j, &bit) in row.iter().enumerate() {
            if bit {
                relation.set_bit_i32(i as i32, j as i32, true);
            }
        }
    }

    Ok(relation)
}

#[cfg(test)]
mod tests {
    use crate::{
        Relation,
        fmt::{parse_matrix, parse_relation},
    };

    #[test]
    fn test_display() {
        let mut rel = Relation::identity_i32(3);
        rel.set_bit_i32(0, 2, true);

        let result = rel.to_string();

        assert_eq!(result, "Relation (3, 3)\n1 : 1, 3\n2 : 2\n3 : 3\n");
    }

    #[test]
    fn test_display_matrix() {
        let mut rel = Relation::identity_i32(3);
        rel.set_bit_i32(0, 2, true);

        let result = rel.display_matrix().to_string();

        assert_eq!(result, "+---+\n|X X|\n| X |\n|  X|\n+---+\n");
    }

    #[test]
    fn test_parse_relation() {
        let input = include_str!("../../examples/R1.ascii");
        let (name, relation) = parse_relation(input).unwrap();

        assert_eq!(name, "R1");
        assert_eq!(relation.rows_i32(), 5);
        assert_eq!(relation.cols_i32(), 5);
        assert_eq!(relation.display(name).to_string(), input);
    }

    #[test]
    fn test_parse_matrix() {
        let input = include_str!("../../examples/R1.matrix");
        let relation = parse_matrix(input).unwrap();

        assert_eq!(relation.rows_i32(), 5);
        assert_eq!(relation.cols_i32(), 5);
        assert_eq!(relation.display_matrix().to_string(), input);
    }
}
