use std::fmt;

use crate::Relation;

impl fmt::Display for Relation {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let rows = self.rows();
        let cols = self.cols();

        write!(f, "Relation ({rows}, {cols})")
    }
}

impl fmt::Debug for Relation {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt::Display::fmt(self, f)
    }
}
