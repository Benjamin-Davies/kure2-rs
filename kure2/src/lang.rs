//! The Kure-Lua bindings and the embedded language.

use std::{
    ffi::{CStr, CString, c_char, c_void},
    ptr,
    sync::{Mutex, MutexGuard},
};

use kure2_lua_sys::{self as lua_ffi, lua_next};
use kure2_sys as ffi;

use crate::{Context, Error, Relation, context};

/// List of functions that are available but not user defined.
const EXCLUDE_FNS: &[&str] = &["bool_to_rel"];

/// Lua does not seem to like running on multiple threads at the same time, so we use a global lock.
static GLOBAL_INTERPRETER_LOCK: Mutex<()> = Mutex::new(());

/// Callbacks for [`parse`] and [`parse_file`].
pub trait ParserObserver {
    /// Called with the original code as well as the generated Lua code whenever a function is parsed.
    fn on_function(&mut self, original_code: &str, lua_code: &str) -> bool {
        let _ = (original_code, lua_code);
        true
    }

    /// Called with the original code as well as the generated Lua code whenever a program (procedure) is parsed.
    fn on_program(&mut self, original_code: &str, lua_code: &str) -> bool {
        let _ = (original_code, lua_code);
        true
    }
}

/// State for the Kure-Lua bindings and the embedded language.
pub struct State {
    ptr: *mut lua_ffi::lua_State,
    ctx: Context,
}

impl State {
    /// Creates a new [`State`] which can be used with the Kure-Lua bindings and the embedded language.
    pub fn new() -> Self {
        State::with_context(context().clone())
    }

    /// Creates a new [`State`] which can be used with the Kure-Lua binding and the embedded language in the given [`Context`].
    pub fn with_context(ctx: Context) -> Self {
        let _lock: MutexGuard<()> = GLOBAL_INTERPRETER_LOCK.lock().unwrap();

        let ptr = unsafe { ffi::kure_lua_new(ctx.ptr) };
        if ptr.is_null() {
            ctx.panic_with_error();
        }
        State { ptr, ctx }
    }

    /// Returns the context associated with this Lua state.
    pub fn context(&self) -> &Context {
        &self.ctx
    }

    /// Similar to [`State::exec`], but stores the result in a global Lua variable of the given name.
    /// If the global variable is non-NULL it is overwritten. If the call fails, the global Lua
    /// variable remains unchanged.
    pub fn assign(&mut self, var: &str, expr: &str) -> Result<(), Error> {
        let _lock: MutexGuard<()> = GLOBAL_INTERPRETER_LOCK.lock().unwrap();

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
        let _lock: MutexGuard<()> = GLOBAL_INTERPRETER_LOCK.lock().unwrap();

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

    /// Executes the given Lua code. The last statement must be a return statement and it must
    /// return a relation.
    pub fn exec_lua(&mut self, chunk: &str) -> Result<Relation, Error> {
        let _lock: MutexGuard<()> = GLOBAL_INTERPRETER_LOCK.lock().unwrap();

        let c_chunk = CString::new(chunk).expect("chunk contains null byte");

        let mut error_ptr = ptr::null_mut();
        let ptr = unsafe { ffi::kure_lua_exec(self.ptr, c_chunk.as_ptr(), &mut error_ptr) };
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
        let _lock: MutexGuard<()> = GLOBAL_INTERPRETER_LOCK.lock().unwrap();

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

    /// Loads the contents of the given file using [`State::load`].
    pub fn load_file(&mut self, file: &str) -> Result<(), Error> {
        let _lock: MutexGuard<()> = GLOBAL_INTERPRETER_LOCK.lock().unwrap();

        let c_file = CString::new(file).expect("file name contains null byte");

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

    /// Gets a relation variable by its name.
    pub fn relation(&self, name: &str) -> Option<Relation> {
        let _lock: MutexGuard<()> = GLOBAL_INTERPRETER_LOCK.lock().unwrap();

        let c_name = CString::new(name).expect("name contains null byte");

        let ptr = unsafe { ffi::kure_lua_get_rel_copy(self.ptr, c_name.as_ptr()) };
        if ptr.is_null() {
            return None;
        }

        Some(Relation {
            ptr,
            ctx: self.ctx.clone(),
        })
    }

    /// Sets a relation variable given its name.
    pub fn set_relation(&mut self, name: &str, rel: &Relation) -> Result<(), Error> {
        let _lock: MutexGuard<()> = GLOBAL_INTERPRETER_LOCK.lock().unwrap();

        let c_name = CString::new(name).expect("name contains null byte");

        let success = unsafe { ffi::kure_lua_set_rel_copy(self.ptr, c_name.as_ptr(), rel.ptr) };
        if success == 0 {
            return Err("Failed to set variable".into());
        }

        Ok(())
    }

    /// Lists all relation variables that are accessible by the embedded language.
    pub fn list_relations(&self) -> Vec<String> {
        let _lock: MutexGuard<()> = GLOBAL_INTERPRETER_LOCK.lock().unwrap();

        let mut keys = Vec::new();
        unsafe { lua_ffi::lua_pushnil(self.ptr) };
        loop {
            let has_next = unsafe { lua_next(self.ptr, lua_ffi::LUA_GLOBALSINDEX) } != 0;
            if !has_next {
                break;
            }

            let key_is_string = unsafe { lua_ffi::lua_isstring(self.ptr, -2) } != 0;
            let is_rel = unsafe { ffi::kure_lua_isrel(self.ptr, -1) } != 0;
            if key_is_string && is_rel {
                let c_key = unsafe { lua_ffi::lua_tolstring(self.ptr, -2, ptr::null_mut()) };
                let key = unsafe { CStr::from_ptr(c_key) }
                    .to_str()
                    .expect("key is not valid UTF-8")
                    .to_owned();
                keys.push(key);
            }

            unsafe { lua_ffi::lua_pop(self.ptr, 1) };
        }
        keys
    }

    /// Lists all user-defined programs and functions that are accessible by the
    /// embedded language.
    pub fn list_programs(&self) -> Vec<String> {
        let _lock: MutexGuard<()> = GLOBAL_INTERPRETER_LOCK.lock().unwrap();

        let mut keys = Vec::new();
        unsafe { lua_ffi::lua_pushnil(self.ptr) };
        loop {
            let has_next = unsafe { lua_next(self.ptr, lua_ffi::LUA_GLOBALSINDEX) } != 0;
            if !has_next {
                break;
            }

            let key_is_string = unsafe { lua_ffi::lua_isstring(self.ptr, -2) } != 0;
            let is_fn = unsafe { lua_ffi::lua_type(self.ptr, -1) } == lua_ffi::LUA_TFUNCTION as i32;
            let is_c_fn = unsafe { lua_ffi::lua_iscfunction(self.ptr, -1) } != 0;
            if key_is_string && is_fn && !is_c_fn {
                let c_key = unsafe { lua_ffi::lua_tolstring(self.ptr, -2, ptr::null_mut()) };
                let key = unsafe { CStr::from_ptr(c_key) }
                    .to_str()
                    .expect("key is not valid UTF-8")
                    .to_owned();
                if !EXCLUDE_FNS.contains(&key.as_str()) {
                    keys.push(key);
                }
            }

            unsafe { lua_ffi::lua_pop(self.ptr, 1) };
        }
        keys
    }
}

impl Default for State {
    fn default() -> Self {
        State::new()
    }
}

impl Drop for State {
    fn drop(&mut self) {
        unsafe { ffi::kure_lua_destroy(self.ptr) };
    }
}

/// Converts a translation unit (source file) in the embedded language to a Lua expression.
pub fn to_lua(transl_unit: &str) -> Result<String, Error> {
    let c_transl_unit = CString::new(transl_unit).expect("transl_unit contains null byte");

    let mut error_ptr = ptr::null_mut();
    let lua_str_ptr = unsafe { ffi::kure_lang_to_lua(c_transl_unit.as_ptr(), &mut error_ptr) };
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

/// Parses the given translation unit. Each time a function or program is parsed the corresponding
/// callback function (if non-NULL) is called and provided the original code as well as the
/// generated Lua code.
pub fn parse(transl_unit: &str, observer: &mut impl ParserObserver) -> Result<(), Error> {
    let c_transl_unit = CString::new(transl_unit).expect("transl_unit contains null byte");
    let mut ffi_observer = make_ffi_observer(observer);

    let mut error_ptr = ptr::null_mut();
    let success =
        unsafe { ffi::kure_lang_parse(c_transl_unit.as_ptr(), &mut ffi_observer, &mut error_ptr) };
    if success == 0 {
        let error = unsafe { Error::from_ffi(error_ptr) };
        unsafe { ffi::kure_error_destroy(error_ptr) };
        return Err(error);
    }

    Ok(())
}

/// Parses the contents of the given file using [`parse`].
pub fn parse_file(file: &str, observer: &mut impl ParserObserver) -> Result<(), Error> {
    let c_file = CString::new(file).expect("file name contains null byte");
    let mut ffi_observer = make_ffi_observer(observer);

    let mut error_ptr = ptr::null_mut();
    let success =
        unsafe { ffi::kure_lang_parse_file(c_file.as_ptr(), &mut ffi_observer, &mut error_ptr) };
    if success == 0 {
        let error = unsafe { Error::from_ffi(error_ptr) };
        unsafe { ffi::kure_error_destroy(error_ptr) };
        return Err(error);
    }

    Ok(())
}

fn make_ffi_observer<O: ParserObserver>(observer: &mut O) -> ffi::KureParserObserver {
    type StateObject<O> = *mut O;

    unsafe extern "C" fn on_function<O: ParserObserver>(
        object: *mut c_void,
        c_original_code: *const c_char,
        c_lua_code: *const c_char,
    ) -> c_char {
        let observer = unsafe { &mut *(object as StateObject<O>) };
        let original_code = unsafe { CStr::from_ptr(c_original_code) }
            .to_str()
            .expect("original_code is not valid UTF-8");
        let lua_code = unsafe { CStr::from_ptr(c_lua_code) }
            .to_str()
            .expect("lua_code is not valid UTF-8");

        let result = observer.on_function(original_code, lua_code);

        result as c_char
    }

    unsafe extern "C" fn on_program<O: ParserObserver>(
        object: *mut c_void,
        c_original_code: *const c_char,
        c_lua_code: *const c_char,
    ) -> c_char {
        let observer = unsafe { &mut *(object as StateObject<O>) };
        let original_code = unsafe { CStr::from_ptr(c_original_code) }
            .to_str()
            .expect("original_code is not valid UTF-8");
        let lua_code = unsafe { CStr::from_ptr(c_lua_code) }
            .to_str()
            .expect("lua_code is not valid UTF-8");

        let result = observer.on_program(original_code, lua_code);

        result as c_char
    }

    ffi::KureParserObserver {
        onFunction: Some(on_function::<O>),
        onProgram: Some(on_program::<O>),
        object: observer as StateObject<O> as *mut c_void,
    }
}

#[cfg(test)]
mod tests {
    use crate::{
        Relation,
        lang::{self, ParserObserver, State},
    };

    #[test]
    fn test_create_destroy_state() {
        let lua_state = State::new();
        drop(lua_state);
    }

    #[test]
    fn test_exec() {
        let mut state = State::new();

        let result = state.exec("TRUE()").unwrap();

        assert!(!result.is_empty());
    }

    #[test]
    fn test_assign() {
        let mut state = State::new();

        state.assign("R", "TRUE()").unwrap();

        let result = state.exec("R").unwrap();
        assert!(!result.is_empty());
    }

    #[test]
    fn test_exec_lua() {
        let mut state = State::new();

        let result = state.exec_lua("return kure.compat.TRUE()").unwrap();

        assert!(!result.is_empty());
    }

    #[test]
    fn test_load() {
        let mut state = State::new();
        let prog = include_str!("../../kure2-sys/kure2-2.2/data/programs/DFS.prog");

        state.load(prog).unwrap();

        let result = state.exec("Dfs(TRUE(), TRUE(), TRUE())").unwrap();
        assert!(!result.is_empty());
    }

    #[test]
    fn test_load_file() {
        let mut state = State::new();
        let file = "../kure2-sys/kure2-2.2/data/programs/DFS.prog";

        state.load_file(file).unwrap();

        let result = state.exec("Dfs(TRUE(), TRUE(), TRUE())").unwrap();
        assert!(!result.is_empty());
    }

    #[test]
    fn test_get_relation_not_found() {
        let state = State::new();

        let result = state.relation("R");

        assert!(result.is_none());
    }

    #[test]
    fn test_get_set_relation() {
        let mut state = State::new();
        let rel = Relation::identity_i32(3);

        state.set_relation("R", &rel).unwrap();
        state.assign("S", "-R").unwrap();
        let result = state.relation("S").unwrap();

        assert_eq!(result.rows_i32(), 3);
        assert_eq!(result.cols_i32(), 3);
        assert_eq!(result, -rel);
    }

    #[test]
    fn test_to_lua() {
        let prog = include_str!("../../kure2-sys/kure2-2.2/data/programs/Aux.prog");

        let result = lang::to_lua(prog).unwrap();

        assert!(result.starts_with("function rc"));
    }

    #[test]
    fn test_expr_to_lua() {
        let expr = "R|S";

        let result = lang::expr_to_lua(expr).unwrap();

        assert_eq!(result, "kure.lor(R,S)");
    }

    #[derive(Default, Debug)]
    struct TestObserver {
        functions: Vec<(String, String)>,
        programs: Vec<(String, String)>,
    }

    impl ParserObserver for TestObserver {
        fn on_function(&mut self, original_code: &str, lua_code: &str) -> bool {
            self.functions
                .push((original_code.to_string(), lua_code.to_string()));
            true
        }

        fn on_program(&mut self, original_code: &str, lua_code: &str) -> bool {
            self.programs
                .push((original_code.to_string(), lua_code.to_string()));
            true
        }
    }

    #[test]
    fn test_parse() {
        let prog = include_str!("../../kure2-sys/kure2-2.2/data/programs/Aux.prog");
        let mut observer = TestObserver::default();

        lang::parse(prog, &mut observer).unwrap();

        assert_eq!(observer.functions.len(), 81);
        assert_eq!(observer.functions[0].0, "rc(R)  = refl(R).");
        assert!(observer.functions[0].1.starts_with("function rc"));
        assert_eq!(observer.programs.len(), 3);
        assert!(observer.programs[0].0.starts_with("randompoint(v)"));
        assert!(observer.programs[0].1.starts_with("function randompoint"));
    }

    #[test]
    fn test_parse_file() {
        let file = "../kure2-sys/kure2-2.2/data/programs/Aux.prog";
        let mut observer = TestObserver::default();

        lang::parse_file(file, &mut observer).unwrap();

        assert_eq!(observer.functions.len(), 81);
        assert_eq!(observer.functions[0].0, "rc(R)  = refl(R).");
        assert!(observer.functions[0].1.starts_with("function rc"));
        assert_eq!(observer.programs.len(), 3);
        assert!(observer.programs[0].0.starts_with("randompoint(v)"));
        assert!(observer.programs[0].1.starts_with("function randompoint"));
    }

    #[test]
    fn test_list_relations() {
        let mut state = State::new();
        state.assign("R", "true()").unwrap();
        state.assign("S", "false()").unwrap();

        let vars = state.list_relations();

        assert!(vars.contains(&"R".to_owned()));
        assert!(vars.contains(&"S".to_owned()));
    }

    #[test]
    fn test_list_programs() {
        let mut state = State::new();
        let file = "../kure2-sys/kure2-2.2/data/programs/DFS.prog";
        state.load_file(file).unwrap();

        let progs = state.list_programs();

        assert_eq!(progs.len(), 2);
        assert!(progs.contains(&"Dfs".to_owned()));
        assert!(progs.contains(&"ReachDfs".to_owned()));
    }
}
