use std::{path::PathBuf, process::Command};

use fs_extra::dir::CopyOptions;

fn main() {
    let manifest_dir: PathBuf = std::env::var("CARGO_MANIFEST_DIR").unwrap().into();
    let src_dir = manifest_dir.join("cudd-cudd-2.5.1");
    let out_dir: PathBuf = std::env::var("OUT_DIR").unwrap().into();
    let build_dir = out_dir.join("cudd-cudd-2.5.1");

    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed={}", src_dir.display());

    fs_extra::copy_items(&[&src_dir], out_dir, &CopyOptions::new().overwrite(true)).unwrap();

    let status = Command::new("make")
        .current_dir(&build_dir)
        .status()
        .unwrap();
    assert!(status.success());

    println!("cargo::metadata=dir={}", build_dir.display())
}
