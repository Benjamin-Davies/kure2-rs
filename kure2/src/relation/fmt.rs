//! Formatting and parsing for [`Relation`].

use std::fmt;

use crate::Relation;

/// Temporary struct returned by [`Relation::display`].
pub struct Display<'a> {
    relation: &'a Relation,
    name: &'a str,
}

impl Relation {
    /// Displays this relation with the given name.
    pub fn display<'a>(&'a self, name: &'a str) -> Display<'a> {
        Display {
            relation: self,
            name,
        }
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
                        write!(f, "\n{}: {}", i + 1, j + 1)?;
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

#[cfg(test)]
mod tests {
    use crate::Relation;

    #[test]
    fn display() {
        let mut rel = Relation::identity_i32(3);
        rel.set_bit_i32(0, 2, true);

        let result = rel.to_string();

        assert_eq!(result, "Relation (3, 3)\n1: 1, 3\n2: 2\n3: 3\n");
    }
}
