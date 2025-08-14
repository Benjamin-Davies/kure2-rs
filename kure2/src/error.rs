use std::{ffi::CStr, fmt};

use kure2_sys as ffi;

use crate::Context;

/// An error that can occur in Kure operations.
#[derive(Debug)]
pub struct Error {
    message: String,
    code: u32,
}

impl Context {
    fn get_error(&self) -> Option<Error> {
        let error = unsafe { ffi::kure_context_get_error(self.ptr) };
        if error.is_null() {
            None
        } else {
            Some(unsafe { Error::from_ffi(error) })
        }
    }

    pub(crate) fn panic_with_error(&self) -> ! {
        if let Some(error) = self.get_error() {
            panic!("{error}");
        } else {
            panic!("Unknown Kure error");
        }
    }
}

impl Error {
    /// Error may be null.
    pub(crate) unsafe fn from_ffi(error: *const ffi::KureError) -> Self {
        if error.is_null() {
            return "Unknown error".into();
        }

        let message = unsafe { CStr::from_ptr((*error).message) };
        let code = unsafe { (*error).code };
        Self {
            message: message.to_str().unwrap().to_owned(),
            code,
        }
    }
}

impl From<&str> for Error {
    fn from(value: &str) -> Self {
        Self {
            message: value.to_owned(),
            code: 0,
        }
    }
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{} (code {})", self.message, self.code)
    }
}

impl std::error::Error for Error {}

#[cfg(test)]
mod tests {
    use crate::Context;

    #[test]
    #[should_panic]
    fn test_panic_with_error() {
        let context = Context::new();
        context.panic_with_error();
    }
}
