use std::{path::PathBuf, process::Command};

use fs_extra::dir::CopyOptions;

fn main() {
    let manifest_dir: PathBuf = std::env::var("CARGO_MANIFEST_DIR").unwrap().into();
    let src_dir = manifest_dir.join("kure2-2.2");
    let out_dir: PathBuf = std::env::var("OUT_DIR").unwrap().into();
    let build_dir = out_dir.join("kure2-2.2");

    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed={}", src_dir.display());

    let cudd_dir: PathBuf = std::env::var("DEP_CUDD_DIR").unwrap().into();
    let gmp_dir: PathBuf = std::env::var("DEP_GMP_OUT_DIR").unwrap().into();
    let lua_dir: PathBuf = std::env::var("DEP_KURE2_LUA_DIR").unwrap().into();
    let lua_luac: PathBuf = std::env::var("DEP_KURE2_LUA_LUAC").unwrap().into();
    let lua_pc: PathBuf = std::env::var("DEP_KURE2_LUA_PC").unwrap().into();

    fs_extra::copy_items(&[&src_dir], &out_dir, &CopyOptions::new().overwrite(true)).unwrap();

    let status = Command::new("./configure")
        .arg(format!("--with-cudd={}", cudd_dir.display()))
        .arg(format!("--with-gmp-dir={}", gmp_dir.display()))
        .arg(format!("--with-lua-pc={}", lua_pc.display()))
        .arg(format!("LUAC={}", lua_luac.display()))
        .current_dir(&build_dir)
        .status()
        .unwrap();
    assert!(status.success());

    let status = Command::new("make")
        .current_dir(&build_dir)
        .status()
        .unwrap();
    assert!(status.success());

    let bindings = bindgen::Builder::default()
        .header(build_dir.join("include/Kure.h").to_str().unwrap())
        .clang_arg(format!("-I{}", cudd_dir.join("include").display()))
        .clang_arg(format!("-I{}", gmp_dir.join("include").display()))
        .clang_arg(format!("-I{}", lua_dir.join("include").display()))
        .blocklist_file(".*/usr/.*")
        .blocklist_file(".*/cudd.h")
        .blocklist_file(".*/gmp.h")
        .blocklist_file(".*/lua.h")
        .allowlist_item("_?(KURE|Kure|kure).*")
        .generate()
        .unwrap();

    let out_path = PathBuf::from(&out_dir);
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .unwrap();

    pkg_config::Config::new()
        .atleast_version("2.0")
        .probe("glib-2.0")
        .unwrap();

    println!("cargo::metadata=dir={}", build_dir.display());
    println!(
        "cargo:rustc-link-search={}",
        build_dir.join("src/.libs").display()
    );
    println!("cargo:rustc-link-lib=kure2");
}
