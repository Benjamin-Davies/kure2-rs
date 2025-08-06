#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

use va_list::VaList as va_list;

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
