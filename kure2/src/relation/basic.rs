use std::ops;

use kure2_sys as ffi;

use crate::Relation;

impl Relation {
    /// Tests if the given relation is empty, that is, R is equal to the empty set.
    pub fn is_empty(&self) -> bool {
        let mut success = 0;
        let result = unsafe { ffi::kure_is_empty(self.ptr, &mut success) };
        if success == 0 {
            self.ctx.panic_with_error();
        }
        result != 0
    }

    /// Tests if the given relation is homogeneous, that is, it has as many rows as it has columns.
    pub fn is_homogeneous(&self) -> bool {
        let mut success = 0;
        let result = unsafe { ffi::kure_is_hom(self.ptr, &mut success) };
        if success == 0 {
            self.ctx.panic_with_error();
        }
        result != 0
    }

    /// Gets the number of rows in the relation.
    pub fn rows(&self) -> rug::Integer {
        let mut rows = rug::Integer::new();
        unsafe { ffi::kure_rel_get_rows(self.ptr, rows.as_raw_mut()) };
        rows
    }

    /// Gets the number of rows in the relation as an `i32`.
    pub fn rows_i32(&self) -> i32 {
        unsafe { ffi::kure_rel_get_rows_si(self.ptr) }
    }

    /// Gets the number of columns in the relation.
    pub fn cols(&self) -> rug::Integer {
        let mut cols = rug::Integer::new();
        unsafe { ffi::kure_rel_get_cols(self.ptr, cols.as_raw_mut()) };
        cols
    }

    /// Gets the number of columns in the relation as an `i32`.
    pub fn cols_i32(&self) -> i32 {
        unsafe { ffi::kure_rel_get_cols_si(self.ptr) }
    }

    /// Checks if the relation contains the given row and column.
    pub fn bit(&self, row: &rug::Integer, col: &rug::Integer) -> bool {
        let mut success = 0;
        let result = unsafe {
            ffi::kure_get_bit(
                self.ptr,
                row.as_raw() as *mut _,
                col.as_raw() as *mut _,
                &mut success,
            )
        };
        if success == 0 {
            self.ctx.panic_with_error();
        }
        result != 0
    }

    /// Checks if the relation contains the given row and column.
    pub fn bit_i32(&self, row: i32, col: i32) -> bool {
        let mut success = 0;
        let result = unsafe { ffi::kure_get_bit_si(self.ptr, row, col, &mut success) };
        if success == 0 {
            self.ctx.panic_with_error();
        }
        result != 0
    }

    /// Sets a given bit to true or false.
    pub fn set_bit(&mut self, row: &rug::Integer, col: &rug::Integer, value: bool) {
        let success = unsafe {
            ffi::kure_set_bit(
                self.ptr,
                value as i8,
                row.as_raw() as *mut _,
                col.as_raw() as *mut _,
            )
        };
        if success == 0 {
            self.ctx.panic_with_error();
        }
    }

    /// Sets a given bit to true or false.
    pub fn set_bit_i32(&mut self, row: i32, col: i32, value: bool) {
        let success = unsafe { ffi::kure_set_bit_si(self.ptr, value as i8, row, col) };
        if success == 0 {
            self.ctx.panic_with_error();
        }
    }
}

impl PartialEq for Relation {
    fn eq(&self, other: &Self) -> bool {
        let mut success = 0;
        let result = unsafe { ffi::kure_equals(self.ptr, other.ptr, &mut success) };
        if success == 0 {
            self.ctx.panic_with_error();
        }
        result != 0
    }
}

impl Eq for Relation {}

impl ops::Neg for Relation {
    type Output = Self;

    fn neg(self) -> Self::Output {
        let result = Self::empty_with_context(&self.ctx, &self.rows(), &self.cols());
        let success = unsafe { ffi::kure_complement(result.ptr, self.ptr) };
        if success == 0 {
            self.ctx.panic_with_error();
        }
        result
    }
}

impl ops::BitOr for Relation {
    type Output = Self;

    fn bitor(self, rhs: Self) -> Self::Output {
        let result = Self::empty_with_context(&self.ctx, &self.rows(), &self.cols());
        let success = unsafe { ffi::kure_or(result.ptr, self.ptr, rhs.ptr) };
        if success == 0 {
            self.ctx.panic_with_error();
        }
        result
    }
}

impl ops::BitAnd for Relation {
    type Output = Self;

    fn bitand(self, rhs: Self) -> Self::Output {
        let result = Self::empty_with_context(&self.ctx, &self.rows(), &self.cols());
        let success = unsafe { ffi::kure_and(result.ptr, self.ptr, rhs.ptr) };
        if success == 0 {
            self.ctx.panic_with_error();
        }
        result
    }
}

#[cfg(test)]
mod tests {
    use crate::Relation;

    #[test]
    fn test_is_empty() {
        let relation = Relation::empty_i32(3, 4);
        assert!(relation.is_empty());

        let relation = Relation::all_i32(3, 4);
        assert!(!relation.is_empty())
    }

    #[test]
    fn test_is_homogeneous() {
        let relation = Relation::empty_i32(3, 3);
        assert!(relation.is_homogeneous());

        let relation = Relation::empty_i32(3, 4);
        assert!(!relation.is_homogeneous());
    }

    #[test]
    fn test_set_bit() {
        let mut relation = Relation::empty_i32(3, 4);

        relation.set_bit(&rug::Integer::from(1), &rug::Integer::from(2), true);

        assert!(relation.bit_i32(1, 2));
        assert!(!relation.bit_i32(0, 0));
    }

    #[test]
    fn test_set_bit_i32() {
        let mut relation = Relation::empty_i32(3, 4);

        relation.set_bit_i32(1, 2, true);

        assert!(relation.bit_i32(1, 2));
        assert!(!relation.bit_i32(0, 0));
    }

    #[test]
    fn test_equals() {
        let relation1 = Relation::empty_i32(3, 4);
        let relation2 = Relation::empty_i32(3, 4);
        let relation3 = Relation::empty_i32(3, 5);

        assert!(relation1 == relation2);
        assert!(relation1 != relation3);
    }

    #[test]
    fn test_negation() {
        let mut relation = Relation::empty_i32(3, 4);
        relation.set_bit_i32(1, 2, true);

        let negated = -relation;

        assert!(!negated.bit_i32(1, 2));
        assert!(negated.bit_i32(0, 0));
    }

    #[test]
    fn test_or() {
        let mut relation1 = Relation::empty_i32(2, 2);
        relation1.set_bit_i32(1, 0, true);
        relation1.set_bit_i32(1, 1, true);
        let mut relation2 = Relation::empty_i32(2, 2);
        relation2.set_bit_i32(0, 1, true);
        relation2.set_bit_i32(1, 1, true);

        let result = relation1 | relation2;

        assert!(!result.bit_i32(0, 0));
        assert!(result.bit_i32(1, 0));
        assert!(result.bit_i32(0, 1));
        assert!(result.bit_i32(1, 1));
    }

    #[test]
    fn test_and() {
        let mut relation1 = Relation::empty_i32(2, 2);
        relation1.set_bit_i32(1, 0, true);
        relation1.set_bit_i32(1, 1, true);
        let mut relation2 = Relation::empty_i32(2, 2);
        relation2.set_bit_i32(0, 1, true);
        relation2.set_bit_i32(1, 1, true);

        let result = relation1 & relation2;

        assert!(!result.bit_i32(0, 0));
        assert!(!result.bit_i32(1, 0));
        assert!(!result.bit_i32(0, 1));
        assert!(result.bit_i32(1, 1));
    }
}
