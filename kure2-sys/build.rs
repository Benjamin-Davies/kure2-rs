use std::{path::PathBuf, process::Command};

use fs_extra::dir::CopyOptions;

fn main() {
    println!("cargo:rerun-if-changed=build.rs");

    let manifest_dir: PathBuf = std::env::var("CARGO_MANIFEST_DIR").unwrap().into();
    let src_dir = manifest_dir.join("kure2-2.2");
    let out_dir: PathBuf = std::env::var("OUT_DIR").unwrap().into();
    let build_dir = out_dir.join("kure2-2.2");

    let cudd_dir: PathBuf = std::env::var("DEP_CUDD_DIR").unwrap().into();
    let gmp_dir: PathBuf = std::env::var("DEP_GMP_OUT_DIR").unwrap().into();
    let lua_pc: PathBuf = std::env::var("DEP_KURE2_LUA_PC").unwrap().into();

    fs_extra::copy_items(&[&src_dir], &out_dir, &CopyOptions::new().overwrite(true)).unwrap();

    let status = Command::new("./configure")
        .arg(format!("--with-cudd={}", cudd_dir.display()))
        .arg(format!("--with-gmp-dir={}", gmp_dir.display()))
        .arg(format!("--with-lua-pc={}", lua_pc.display()))
        .current_dir(&build_dir)
        .status()
        .unwrap();
    assert!(status.success());

    let status = Command::new("make")
        .current_dir(&build_dir)
        .status()
        .unwrap();
    assert!(status.success());
}
