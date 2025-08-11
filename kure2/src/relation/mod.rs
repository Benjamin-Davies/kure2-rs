use kure2_sys as ffi;

use crate::{Context, context};

mod basic;
mod fmt;

/// A relation, represented using a binary decision diagram (BDD).
pub struct Relation {
    pub(crate) ptr: *mut ffi::KureRel,
    pub(crate) ctx: Context,
}

// Factories
impl Relation {
    /// Creates an empty relation with the given number of rows and columns in the given [`Context`].
    pub fn empty(rows: &rug::Integer, cols: &rug::Integer) -> Self {
        Self::empty_with_context(context(), rows, cols)
    }

    /// Creates an empty relation with the given number of rows and columns in the given [`Context`].
    pub fn empty_i32(rows: i32, cols: i32) -> Self {
        Self::empty_i32_with_context(context(), rows, cols)
    }

    /// Creates an identity relation with the given size in the given [`Context`].
    pub fn identity(size: &rug::Integer) -> Self {
        Self::identity_with_context(context(), size)
    }

    /// Creates an identity relation with the given size in the given [`Context`].
    pub fn identity_i32(size: i32) -> Self {
        Self::identity_i32_with_context(context(), size)
    }

    /// Creates a universal relation with the given number of rows and columns in the given [`Context`].
    pub fn all(rows: &rug::Integer, cols: &rug::Integer) -> Self {
        Self::all_with_context(context(), rows, cols)
    }

    /// Creates a universal relation with the given number of rows and columns in the given [`Context`].
    pub fn all_i32(rows: i32, cols: i32) -> Self {
        Self::all_i32_with_context(context(), rows, cols)
    }

    /// Creates an empty relation with the given number of rows and columns in the given [`Context`].
    pub fn empty_with_context(ctx: Context, rows: &rug::Integer, cols: &rug::Integer) -> Self {
        let ptr = unsafe { ffi::kure_rel_new_with_size(ctx.ptr, rows.as_raw(), cols.as_raw()) };
        if ptr.is_null() {
            ctx.panic_with_error();
        }

        Self { ptr, ctx }
    }

    /// Creates an empty relation with the given number of rows and columns in the given [`Context`].
    pub fn empty_i32_with_context(ctx: Context, rows: i32, cols: i32) -> Self {
        let ptr = unsafe { ffi::kure_rel_new_with_size_si(ctx.ptr, rows, cols) };
        if ptr.is_null() {
            ctx.panic_with_error();
        }

        Self { ptr, ctx }
    }

    /// Creates an identity relation with the given size in the given [`Context`].
    pub fn identity_with_context(ctx: Context, size: &rug::Integer) -> Self {
        let mut relation = Self::empty_with_context(ctx, size, size);
        relation.identity_mut();
        relation
    }

    /// Creates an identity relation with the given size in the given [`Context`].
    pub fn identity_i32_with_context(ctx: Context, size: i32) -> Self {
        let mut relation = Self::empty_i32_with_context(ctx, size, size);
        relation.identity_mut();
        relation
    }

    /// Creates a universal relation with the given number of rows and columns in the given [`Context`].
    pub fn all_with_context(ctx: Context, rows: &rug::Integer, cols: &rug::Integer) -> Self {
        let mut relation = Self::empty_with_context(ctx, rows, cols);
        relation.all_mut();
        relation
    }

    /// Creates a universal relation with the given number of rows and columns in the given [`Context`].
    pub fn all_i32_with_context(ctx: Context, rows: i32, cols: i32) -> Self {
        let mut relation = Self::empty_i32_with_context(ctx, rows, cols);
        relation.all_mut();
        relation
    }

    /// Returns the context associated with this relation.
    pub fn context(&self) -> &Context {
        &self.ctx
    }

    /// Sets the given relation to the empty relation.
    pub fn empty_mut(&mut self) {
        let success = unsafe { ffi::kure_O(self.ptr) };
        if success == 0 {
            self.ctx.panic_with_error();
        }
    }

    /// Sets the given relation to the identity relation.
    pub fn identity_mut(&mut self) {
        let success = unsafe { ffi::kure_I(self.ptr) };
        if success == 0 {
            self.ctx.panic_with_error();
        }
    }

    /// Sets the given relation to the universal relation.
    pub fn all_mut(&mut self) {
        let success = unsafe { ffi::kure_L(self.ptr) };
        if success == 0 {
            self.ctx.panic_with_error();
        }
    }
}

impl Clone for Relation {
    fn clone(&self) -> Self {
        let ptr = unsafe { ffi::kure_rel_new_copy(self.ptr) };
        if ptr.is_null() {
            self.ctx.panic_with_error();
        }
        Self {
            ptr,
            ctx: self.ctx.clone(),
        }
    }
}

impl Drop for Relation {
    fn drop(&mut self) {
        unsafe { ffi::kure_rel_destroy(self.ptr) };
    }
}

#[cfg(test)]
mod tests {
    use crate::Relation;

    #[test]
    fn test_create_empty() {
        let rows = rug::Integer::from(3);
        let cols = rug::Integer::from(4);

        let relation = Relation::empty(&rows, &cols);

        assert_eq!(relation.rows(), rows);
        assert_eq!(relation.cols(), cols);
        for i in 0..3 {
            let i = rug::Integer::from(i);
            for j in 0..4 {
                let j = rug::Integer::from(j);
                assert!(!relation.bit(&i, &j));
            }
        }
    }

    #[test]
    fn test_create_empty_i32() {
        let rows = 3;
        let cols = 4;

        let relation = Relation::empty_i32(rows, cols);

        assert_eq!(relation.rows_i32(), rows);
        assert_eq!(relation.cols_i32(), cols);
        for i in 0..rows {
            for j in 0..cols {
                assert!(!relation.bit_i32(i, j));
            }
        }
    }

    #[test]
    fn test_create_identity() {
        let size = rug::Integer::from(3);

        let relation = Relation::identity(&size);

        assert_eq!(relation.rows(), size);
        assert_eq!(relation.cols(), size);
        for i in 0..3 {
            let i = rug::Integer::from(i);
            for j in 0..3 {
                let j = rug::Integer::from(j);
                assert_eq!(relation.bit(&i, &j), i == j);
            }
        }
    }

    #[test]
    fn test_create_identity_i32() {
        let size = 3;

        let relation = Relation::identity_i32(size);

        assert_eq!(relation.rows_i32(), size);
        assert_eq!(relation.cols_i32(), size);
        for i in 0..size {
            for j in 0..size {
                assert_eq!(relation.bit_i32(i, j), i == j);
            }
        }
    }

    #[test]
    fn test_create_all() {
        let rows = rug::Integer::from(3);
        let cols = rug::Integer::from(4);

        let relation = Relation::all(&rows, &cols);

        assert_eq!(relation.rows(), rows);
        assert_eq!(relation.cols(), cols);
        for i in 0..3 {
            let i = rug::Integer::from(i);
            for j in 0..4 {
                let j = rug::Integer::from(j);
                assert!(relation.bit(&i, &j));
            }
        }
    }

    #[test]
    fn test_create_all_i32() {
        let rows = 3;
        let cols = 4;

        let relation = Relation::all_i32(rows, cols);

        assert_eq!(relation.rows_i32(), rows);
        assert_eq!(relation.cols_i32(), cols);
        for i in 0..rows {
            for j in 0..cols {
                assert!(relation.bit_i32(i, j));
            }
        }
    }

    #[test]
    fn test_make_empty() {
        let rows = 3;
        let cols = 4;
        let mut relation = Relation::all_i32(rows, cols);

        relation.empty_mut();

        assert_eq!(relation.rows_i32(), rows);
        assert_eq!(relation.cols_i32(), cols);
        for i in 0..rows {
            for j in 0..cols {
                assert!(!relation.bit_i32(i, j));
            }
        }
    }

    #[test]
    fn test_make_identity() {
        let size = 3;
        let mut relation = Relation::empty_i32(size, size);

        relation.identity_mut();

        assert_eq!(relation.rows_i32(), size);
        assert_eq!(relation.cols_i32(), size);
        for i in 0..size {
            for j in 0..size {
                assert_eq!(relation.bit_i32(i, j), i == j);
            }
        }
    }

    #[test]
    #[should_panic]
    fn test_make_identity_non_homogenous() {
        let rows = 3;
        let cols = 4;
        let mut relation = Relation::empty_i32(rows, cols);

        relation.identity_mut();
    }

    #[test]
    fn test_make_all() {
        let rows = 3;
        let cols = 4;
        let mut relation = Relation::empty_i32(rows, cols);

        relation.all_mut();

        assert_eq!(relation.rows_i32(), rows);
        assert_eq!(relation.cols_i32(), cols);
        for i in 0..rows {
            for j in 0..cols {
                assert!(relation.bit_i32(i, j));
            }
        }
    }

    #[test]
    fn test_clone() {
        let mut relation = Relation::empty_i32(3, 4);
        relation.set_bit_i32(1, 2, true);

        let cloned = relation.clone();

        assert!(cloned.bit_i32(1, 2));
        assert!(!cloned.bit_i32(0, 0));
    }
}
