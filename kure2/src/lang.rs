use kure2_lua_sys as lua_ffi;
use kure2_sys as ffi;

use crate::{Context, context};

/// The state of the Kure-Lua bindings.
pub struct LuaState {
    ptr: *mut lua_ffi::lua_State,
}

impl LuaState {
    /// Creates a new [`LuaState`] which can be used with the Kure-Lua bindings and the embedded language.
    pub fn new() -> Self {
        LuaState::with_context(context().clone())
    }

    /// Creates a new [`LuaState`] which can be used with the Kure-Lua binding and the embedded language in the given [`Context`].
    pub fn with_context(ctx: Context) -> Self {
        let ptr = unsafe { ffi::kure_lua_new(ctx.ptr) };
        if ptr.is_null() {
            ctx.panic_with_error();
        }
        LuaState { ptr }
    }
}

impl Default for LuaState {
    fn default() -> Self {
        LuaState::new()
    }
}

impl Drop for LuaState {
    fn drop(&mut self) {
        unsafe { ffi::kure_lua_destroy(self.ptr) };
    }
}

#[cfg(test)]
mod tests {
    use crate::LuaState;

    #[test]
    fn test_create_destroy_lua_state() {
        let lua_state = LuaState::new();
        drop(lua_state);
    }
}
