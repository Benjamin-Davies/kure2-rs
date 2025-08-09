use kure2_sys as ffi;

/// A context for managing relations in Kure.
pub struct Context {
    pub(crate) ptr: *mut ffi::KureContext,
}

impl Default for Context {
    fn default() -> Self {
        Self::new()
    }
}

impl Context {
    /// Creates a new context.
    pub fn new() -> Self {
        let ptr = unsafe { ffi::kure_context_new() };
        debug_assert!(!ptr.is_null());

        // Increment the reference count to 1
        unsafe { ffi::kure_context_ref(ptr) };

        Self { ptr }
    }
}

impl Clone for Context {
    fn clone(&self) -> Self {
        unsafe { ffi::kure_context_ref(self.ptr) };

        Self { ptr: self.ptr }
    }
}

impl Drop for Context {
    fn drop(&mut self) {
        unsafe { ffi::kure_context_deref(self.ptr) };
    }
}

/// Access a pre-initialized context. This is a handle to the local [`Context`].
pub fn context() -> Context {
    thread_local! {
        static CONTEXT: Context = Context::new();
    }

    CONTEXT.with(|ctx| ctx.clone())
}

#[cfg(test)]
mod tests {
    use crate::Context;

    #[test]
    fn test_create_destroy_context() {
        let context = Context::new();
        drop(context);
    }

    #[test]
    fn test_clone_context() {
        let context = Context::new();
        let _ = context.clone();
    }
}
