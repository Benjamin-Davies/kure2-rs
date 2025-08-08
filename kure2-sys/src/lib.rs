#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(rustdoc::broken_intra_doc_links)]
#![allow(clippy::all)]

use cudd2_sys::*;
use gmp_mpfr_sys::gmp::mpz_t as __mpz_struct;
use kure2_lua_sys::*;
use libc::*;
use va_list::VaList as va_list;

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

#[cfg(test)]
mod tests {
    use crate::{
        kure_context_destroy, kure_context_get_error, kure_context_new, kure_lua_destroy,
        kure_lua_new,
    };

    #[test]
    fn test_create_destroy_context() {
        unsafe {
            let context = kure_context_new();
            assert!(!context.is_null());

            let error = kure_context_get_error(context);
            assert!(error.is_null());

            kure_context_destroy(context);
        }
    }

    #[test]
    fn test_create_destroy_lua_state() {
        unsafe {
            let context = kure_context_new();
            assert!(!context.is_null());

            let lua_state = kure_lua_new(context);
            assert!(!lua_state.is_null());

            let error = kure_context_get_error(context);
            assert!(error.is_null());

            kure_lua_destroy(lua_state);
            kure_context_destroy(context);
        }
    }
}
