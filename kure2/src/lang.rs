//! The Kure-Lua bindings and the embedded language.

use std::{ffi::CString, ptr};

use kure2_lua_sys as lua_ffi;
use kure2_sys as ffi;

use crate::{Context, Error, Relation, context};

/// State for the Kure-Lua bindings and the embedded language.
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

        let mut error_ptr = ptr::null_mut();
        let success = unsafe {
            ffi::kure_lang_assign(self.ptr, c_var.as_ptr(), c_expr.as_ptr(), &mut error_ptr)
        };
        if success == 0 {
            let error = unsafe { Error::from_ffi(error_ptr) };
            unsafe { ffi::kure_error_destroy(error_ptr) };
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

        let mut error_ptr = ptr::null_mut();
        let ptr = unsafe { ffi::kure_lang_exec(self.ptr, c_expr.as_ptr(), &mut error_ptr) };
        if ptr.is_null() {
            let error = unsafe { Error::from_ffi(error_ptr) };
            unsafe { ffi::kure_error_destroy(error_ptr) };
            return Err(error);
        }

        Ok(Relation {
            ptr,
            ctx: self.ctx.clone(),
        })
    }

    /// Loads a given translation unit written in the embedded language. Translations units may only
    /// contain function and program definitions.
    pub fn load(&mut self, transl_unit: &str) -> Result<(), Error> {
        let c_transl_unit = CString::new(transl_unit).expect("transl_unit contains null byte");

        let mut error_ptr = ptr::null_mut();
        let success =
            unsafe { ffi::kure_lang_load(self.ptr, c_transl_unit.as_ptr(), &mut error_ptr) };
        if success == 0 {
            let error = unsafe { Error::from_ffi(error_ptr) };
            unsafe { ffi::kure_error_destroy(error_ptr) };
            return Err(error);
        }

        Ok(())
    }

    /// Loads the contents of the given file using [`LuaState::load`].
    pub fn load_file(&mut self, file: &str) -> Result<(), Error> {
        let c_file = CString::new(file).expect("file contains null byte");

        let mut error_ptr = ptr::null_mut();
        let success =
            unsafe { ffi::kure_lang_load_file(self.ptr, c_file.as_ptr(), &mut error_ptr) };
        if success == 0 {
            let error = unsafe { Error::from_ffi(error_ptr) };
            unsafe { ffi::kure_error_destroy(error_ptr) };
            return Err(error);
        }

        Ok(())
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

/// Converts an expression in the embedded language to a Lua expression.
pub fn expr_to_lua(expr: &str) -> Result<String, Error> {
    let c_expr = CString::new(expr).expect("expr contains null byte");

    let mut error_ptr = ptr::null_mut();
    let lua_str_ptr = unsafe { ffi::kure_lang_expr_to_lua(c_expr.as_ptr(), &mut error_ptr) };
    if lua_str_ptr.is_null() {
        let error = unsafe { Error::from_ffi(error_ptr) };
        unsafe { ffi::kure_error_destroy(error_ptr) };
        return Err(error);
    }

    let lua_str = unsafe { CString::from_raw(lua_str_ptr) };
    Ok(lua_str
        .into_string()
        .expect("lua string contains invalid UTF-8"))
}

#[cfg(test)]
mod tests {
    use crate::lang::{self, LuaState};

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

    #[test]
    fn test_load() {
        let mut state = LuaState::new();
        let prog = include_str!("../../kure2-sys/kure2-2.2/data/programs/DFS.prog");

        state.load(prog).unwrap();

        let result = state.exec("Dfs(TRUE(), TRUE(), TRUE())").unwrap();
        assert!(!result.is_empty());
    }

    #[test]
    fn test_load_file() {
        let mut state = LuaState::new();
        let file = "../kure2-sys/kure2-2.2/data/programs/DFS.prog";

        state.load_file(file).unwrap();

        let result = state.exec("Dfs(TRUE(), TRUE(), TRUE())").unwrap();
        assert!(!result.is_empty());
    }

    #[test]
    fn test_expr_to_lua() {
        let expr = "R|S";

        let result = lang::expr_to_lua(expr).unwrap();

        assert_eq!(result, "kure.lor(R,S)");
    }
}
