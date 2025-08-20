use std::{fs::File, io::Write, path::PathBuf, process::Command};

use fs_extra::dir::CopyOptions;

fn main() {
    let manifest_dir: PathBuf = std::env::var("CARGO_MANIFEST_DIR").unwrap().into();
    let src_dir = manifest_dir.join("lua-5.1.5");
    let out_dir: PathBuf = std::env::var("OUT_DIR").unwrap().into();
    let build_dir = out_dir.join("lua-5.1.5");
    let pc_file = out_dir.join("lua.pc");

    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed=wrapper.h");
    println!("cargo:rerun-if-changed={}", src_dir.display());

    fs_extra::copy_items(&[&src_dir], &out_dir, &CopyOptions::new().overwrite(true)).unwrap();

    let status = Command::new("make")
        .arg("generic")
        .arg("local")
        .current_dir(&build_dir)
        .status()
        .unwrap();
    assert!(status.success());

    {
        let mut pc_file = File::create(&pc_file).unwrap();
        writeln!(pc_file, "prefix={}", build_dir.display()).unwrap();
        let _ = pc_file.write(include_bytes!("lua.pc")).unwrap();
    }

    let bindings = bindgen::Builder::default()
        .header("wrapper.h")
        .clang_arg(format!("-I{}", build_dir.join("include").display()))
        .blocklist_file(".*/usr/.*")
        .generate()
        .unwrap();

    let out_path = PathBuf::from(&out_dir);
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .unwrap();

    println!(
        "cargo::metadata=luac={}",
        build_dir.join("bin/luac").display()
    );
    println!("cargo::metadata=dir={}", build_dir.display());
    println!("cargo::metadata=pc={}", pc_file.display());
    println!(
        "cargo:rustc-link-search={}",
        build_dir.join("lib").display()
    );
    println!("cargo:rustc-link-lib=lua");
}
