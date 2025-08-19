#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

use std::ffi::c_int;

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

pub unsafe fn lua_pop(L: *mut lua_State, index: c_int) {
    unsafe {
        lua_settop(L, -index - 1);
    }
}
