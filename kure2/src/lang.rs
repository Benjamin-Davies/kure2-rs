use std::{ffi::CString, ptr};

use kure2_lua_sys as lua_ffi;
use kure2_sys as ffi;

use crate::{Context, Error, Relation, context};

/// The state of the Kure-Lua bindings.
pub struct LuaState {
    ptr: *mut lua_ffi::lua_State,
    ctx: Context,
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
        LuaState { ptr, ctx }
    }

    /// Returns the context associated with this Lua state.
    pub fn context(&self) -> &Context {
        &self.ctx
    }

    /// Similar to [`LuaState::exec`], but stores the result in a global Lua variable of the given name.
    /// If the global variable is non-NULL it is overwritten. If the call fails, the global Lua
    /// variable remains unchanged.
    pub fn assign(&mut self, var: &str, expr: &str) -> Result<(), Error> {
        let c_var = CString::new(var).expect("var name contains null byte");
        let c_expr = CString::new(expr).expect("expr contains null byte");

        let mut error = ptr::null_mut();
        let result =
            unsafe { ffi::kure_lang_assign(self.ptr, c_var.as_ptr(), c_expr.as_ptr(), &mut error) };
        if result == 0 {
            let error = unsafe { Error::from_ffi(error) };
            return Err(error);
        }

        Ok(())
    }

    /// Executes the given expression in the embedded language and returns the resulting relation.
    /// Expressions are built from `|`, `&`, `[.,.]`, `(`, `)`, etc. and functions calls. Neither
    /// assignments nor definitions of other objects (functions, programs) are allowed. Expressions
    /// always return a relation if successful.
    ///
    /// Takes an expression in the embedded language. E.g. `R|S` or `[T+random10(R),X]`.
    ///
    /// Remark: Some functions in the embedded language depend on a special global variable `__c`
    /// which holds a [`Context`].
    pub fn exec(&mut self, expr: &str) -> Result<Relation, Error> {
        let c_expr = CString::new(expr).expect("expr contains null byte");

        let mut error = ptr::null_mut();
        let ptr = unsafe { ffi::kure_lang_exec(self.ptr, c_expr.as_ptr(), &mut error) };
        if ptr.is_null() {
            let error = unsafe { Error::from_ffi(error) };
            return Err(error);
        }

        Ok(Relation {
            ptr,
            ctx: self.ctx.clone(),
        })
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

    #[test]
    fn test_exec() {
        let mut state = LuaState::new();

        let result = state.exec("TRUE()").unwrap();

        assert!(!result.is_empty());
    }

    #[test]
    fn test_assign() {
        let mut state = LuaState::new();

        state.assign("R", "TRUE()").unwrap();

        let result = state.exec("R").unwrap();
        assert!(!result.is_empty());
    }
}
