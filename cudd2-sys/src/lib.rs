#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(clippy::all)]

use libc::*;

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

#[cfg(target_pointer_width = "64")]
pub type DdHalfWord = u32;
#[cfg(target_pointer_width = "32")]
pub type DdHalfWord = u16;

#[cfg(target_pointer_width = "64")]
pub const CUDD_MAXINDEX: DdHalfWord = u32::MAX >> 1;
#[cfg(target_pointer_width = "32")]
pub const CUDD_MAXINDEX: DdHalfWord = u16::MAX;

pub const CUDD_CONST_INDEX: DdHalfWord = CUDD_MAXINDEX;

#[repr(C)]
pub struct DdNode {
    pub index: DdHalfWord,
    pub ref_: DdHalfWord,
    pub next: *mut DdNode,
    pub type_: DdNode__bindgen_ty_1,
}
#[repr(C)]
#[derive(Copy, Clone)]
pub union DdNode__bindgen_ty_1 {
    pub value: f64,
    pub kids: DdChildren,
}
#[allow(clippy::unnecessary_operation, clippy::identity_op)]
const _: () = {
    ["Size of DdNode__bindgen_ty_1"][::std::mem::size_of::<DdNode__bindgen_ty_1>() - 16usize];
    ["Alignment of DdNode__bindgen_ty_1"][::std::mem::align_of::<DdNode__bindgen_ty_1>() - 8usize];
    ["Offset of field: DdNode__bindgen_ty_1::value"]
        [::std::mem::offset_of!(DdNode__bindgen_ty_1, value) - 0usize];
    ["Offset of field: DdNode__bindgen_ty_1::kids"]
        [::std::mem::offset_of!(DdNode__bindgen_ty_1, kids) - 0usize];
};
#[allow(clippy::unnecessary_operation, clippy::identity_op)]
const _: () = {
    ["Size of DdNode"][::std::mem::size_of::<DdNode>() - 32usize];
    ["Alignment of DdNode"][::std::mem::align_of::<DdNode>() - 8usize];
    ["Offset of field: DdNode::index"][::std::mem::offset_of!(DdNode, index) - 0usize];
    ["Offset of field: DdNode::ref_"][::std::mem::offset_of!(DdNode, ref_) - 4usize];
    ["Offset of field: DdNode::next"][::std::mem::offset_of!(DdNode, next) - 8usize];
    ["Offset of field: DdNode::type_"][::std::mem::offset_of!(DdNode, type_) - 16usize];
};
