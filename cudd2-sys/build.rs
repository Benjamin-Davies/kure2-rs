use std::{path::PathBuf, process::Command};

fn main() {
    println!("cargo:rerun-if-changed=build.rs");

    let manifest_dir: PathBuf = std::env::var("CARGO_MANIFEST_DIR").unwrap().into();
    let src_dir = manifest_dir.join("cudd-cudd-2.5.1");
    let out_dir: PathBuf = std::env::var("OUT_DIR").unwrap().into();

    let status = Command::new("make").current_dir(&src_dir).status().unwrap();
    assert!(status.success());
}
