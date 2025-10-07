use std::{fmt, marker::PhantomData, ops};

use cudd2_sys::{self as ffi, CUDD_CONST_INDEX, DdHalfWord};

use crate::Relation;

#[derive(Clone, Copy)]
pub(crate) struct DdNodeRef<'a> {
    ptr: *mut ffi::DdNode,
    _marker: PhantomData<&'a ffi::DdNode>,
}

impl Relation {
    /// Returns the underlying CUDD BDD node.
    pub(crate) fn dd_node(&self) -> DdNodeRef<'_> {
        let node = unsafe { kure2_sys::kure_rel_get_bdd(self.ptr) };
        unsafe { DdNodeRef::from_ptr(node) }
    }
}

impl<'a> DdNodeRef<'a> {
    pub(crate) unsafe fn from_ptr(ptr: *mut ffi::DdNode) -> Self {
        Self {
            ptr,
            _marker: PhantomData,
        }
    }

    pub(crate) fn to_regular(&self) -> Self {
        let ptr = (self.ptr as usize & !1) as *mut ffi::DdNode;
        unsafe { DdNodeRef::from_ptr(ptr) }
    }

    pub(crate) fn is_complement(&self) -> bool {
        (self.ptr as usize & 1) != 0
    }

    fn inner(&self) -> &ffi::DdNode {
        let regular = self.to_regular();
        unsafe { &*regular.ptr }
    }

    pub(crate) fn index(&self) -> DdHalfWord {
        self.inner().index
    }

    pub(crate) fn is_const(&self) -> bool {
        self.inner().index == CUDD_CONST_INDEX
    }

    fn value(&self) -> f64 {
        assert!(self.is_const());
        unsafe { self.inner().type_.value }
    }

    pub(crate) fn logic_value(&self) -> bool {
        (self.value() != 0.0) ^ self.is_complement()
    }

    pub(crate) fn is_false(&self) -> bool {
        self.is_const() && !self.logic_value()
    }

    pub(crate) fn then_child(&self) -> Self {
        assert!(!self.is_const());
        let ptr = unsafe { self.inner().type_.kids.T };
        let node = unsafe { DdNodeRef::from_ptr(ptr) };
        node ^ self.is_complement()
    }

    pub(crate) fn else_child(&self) -> Self {
        assert!(!self.is_const());
        let ptr = unsafe { self.inner().type_.kids.E };
        let node = unsafe { DdNodeRef::from_ptr(ptr) };
        node ^ self.is_complement()
    }
}

impl<'a> ops::Not for DdNodeRef<'a> {
    type Output = Self;

    fn not(self) -> Self::Output {
        let ptr = (self.ptr as usize ^ 1) as *mut ffi::DdNode;
        unsafe { DdNodeRef::from_ptr(ptr) }
    }
}

impl<'a> ops::BitXor<bool> for DdNodeRef<'a> {
    type Output = Self;

    fn bitxor(self, rhs: bool) -> Self::Output {
        let ptr = (self.ptr as usize ^ (rhs as usize)) as *mut ffi::DdNode;
        unsafe { DdNodeRef::from_ptr(ptr) }
    }
}

impl fmt::Debug for DdNodeRef<'_> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if self.is_const() {
            write!(f, "{}", self.logic_value())
        } else {
            write!(
                f,
                "(v{} ? {:?} : {:?})",
                self.index(),
                self.then_child(),
                self.else_child()
            )
        }
    }
}
