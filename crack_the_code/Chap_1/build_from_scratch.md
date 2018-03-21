---
layout: default
title: Build prelude from scratch
---
## [](#header-2) Build from scratch

Prelude is based on Chromium release [65.0.3289.2](https://chromium.googlesource.com/chromium/src.git/+/65.0.3289.2). It requires about 26G disk space for compilation, comparing to ~160G for the whole Chromium project. Follow the steps below to build Prelude from scratch.


### [](#header-3) 1.  Checkout the code
```
git clone https://github.com/xzwang2005/Prelude.git
```

### [](#header-3) 2.  Generate build targets with *gn*

```
gn gen out/debug --args="enable_precompiled_headers=false use_jumbo_build=true"
```
This command creates a folder *out/debug* under the root directory, with files containing all the build information:
*   gn arguments - args.gn
*   ninja build files - build.ninja, toolchain.ninja etc.
*   environment setup files - environment.x64 etc.
*   dll files (Windows only)

>*enable_precompiled_headers* must be set to *false* as shown above. Currently, clang does not work with Visual Studio v15.5.x headers when precompile is enabled. This is a [known issue](https://bugs.chromium.org/p/chromium/issues/detail?id=780124) with no fix planned. Official Chrome build probably still use Visual Studio v15.4.x with SDK v10.0.15063.0, which does not have this problem. *use_jumbo_build=true* helps speed up the compile process.

### [](#header-3) 3.  Build the targets using *ninja*
```
ninja -C out/debug
```
It's expected to see some clang-specific warnings from one WTL sample project. Details will be provided in later sections.

[Back](/)
