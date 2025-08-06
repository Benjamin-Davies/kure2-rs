use std::{path::PathBuf, process::Command};

use fs_extra::dir::CopyOptions;

fn main() {
    let manifest_dir: PathBuf = std::env::var("CARGO_MANIFEST_DIR").unwrap().into();
    let src_dir = manifest_dir.join("cudd-cudd-2.5.1");
    let out_dir: PathBuf = std::env::var("OUT_DIR").unwrap().into();
    let build_dir = out_dir.join("cudd-cudd-2.5.1");

    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed={}", src_dir.display());

    fs_extra::copy_items(&[&src_dir], &out_dir, &CopyOptions::new().overwrite(true)).unwrap();

    let status = Command::new("make")
        .current_dir(&build_dir)
        .status()
        .unwrap();
    assert!(status.success());

    let bindings = bindgen::Builder::default()
        .header("wrapper.h")
        .clang_arg(format!("-I{}", build_dir.join("include").display()))
        .blocklist_file(".*/usr/.*")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        .generate()
        .unwrap();

    let out_path = PathBuf::from(&out_dir);
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .unwrap();

    println!("cargo::metadata=dir={}", build_dir.display());
    println!(
        "cargo:rustc-link-search={}",
        build_dir.join("cudd").display()
    );
    println!("cargo:rustc-link-lib=cudd");
    println!(
        "cargo:rustc-link-search={}",
        build_dir.join("mtr").display()
    );
    println!("cargo:rustc-link-lib=mtr");
    println!("cargo:rustc-link-search={}", build_dir.join("st").display());
    println!("cargo:rustc-link-lib=st");
    println!(
        "cargo:rustc-link-search={}",
        build_dir.join("util").display()
    );
    println!("cargo:rustc-link-lib=util");
}
