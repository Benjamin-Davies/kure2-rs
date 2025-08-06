#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

use cudd2_sys::*;
use gmp_mpfr_sys::gmp::mpz_t as __mpz_struct;
use kure2_lua_sys::*;
use libc::*;
use va_list::VaList as va_list;

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
