---
layout: default
title: GN basics
---
## [](#header-2) GN Basics

Previous posts show how to get Prelude built with just a few commands. However, in order to reuse Chromium code, it requires a deeper understanding of the build system used by Chromium - GN.

To use GN is to write a bunch of BUILD.gn files organized in a tree-like structure. Let's start with *hello_bogart* executable, which spit out the following:
```
Hello, Bogart.
Woof Woof
ou-ooooooo
whif whif
```

Open the [BUILD.gn](https://github.com/xzwang2005/Prelude/blob/master/prelude/excerpt01_gn/hello_puppy/BUILD.gn) under *hello_puppy* directory, it shows four types of built-in targets:
* executable - generate hello_bogart.exe, prints the first line in output
* static_library - generate bark_food.lib, prints the second line
* shared_library - generate bark_ambulance.dll, prints the third line
* source_set - similar to static_library, compiled into executable directly without generating a .lib file; prints the last line

Take the executable target for example:
```gn
executable("hello_puppy") {
  output_name = "hello_bogart"
  sources = ["hello_bogart.cpp", ]
  deps = [
    ":bark_food",
    ":bark_ambulance",
  ]
}
```
`sources` specifies all the source files to be compiled. `output_name` tells GN that the output binary should be named "hello_bogart" instead of the target name "hello_puppy" by default. `deps` lists all the libraries required for building the binary.

Note how the executable specify the depending targets: `":bark_food"`. This format *:target_name* is used to point to the target that can be found in the same BUILD.gn file.

As shown in the shared library target, `defines` is used to store the list of preprocessor defines:
```gn
shared_library("bark_ambulance") {
  sources = [
    "bark_ambulance.cpp",
  ]
  defines = [ "HOWL_IMPLEMENTATION" ]
}
```

Here, `HOWL_IMPLEMENTATION` is defined which is used in [bark_ambulance.h]()

```c++
#if defined(HOWL_IMPLEMENTATION)
#define HOWL_EXPORT __declspec(dllexport)
#define HOWL_EXPORT_PRIVATE __declspec(dllexport)
#else
#define HOWL_EXPORT __declspec(dllimport)
#define HOWL_EXPORT_PRIVATE __declspec(dllimport)
#endif  // defined(HOWL_IMPLEMENTATION)
```
This BUILD.gn file contains all the instructions to build those targets. However, it alone does not "trigger" these targets to be compiled because it's just a "leaf" in the build tree.

![build tree]({{ site.baseurl }}/crack_the_code/Chap_2/images/build_tree.png)

As seen in the above figure, there is a BUILD.gn file at each level from root directory all the way to the folder that contains the source files. In this example, all upper level BUILD.gn files only defines one built-in target - `group`.

`group` is like a shortcut which points to a set of dependencies, causing those targets to be picked up for build. In the figure above, the chain of targets is highlighted in red. The root build file (//BUILD.gn) depends on `"//prelude"`. This means gn expects there will be a `//prelude/BUILD.gn` file, which defines a target named `"prelude"`. ```//``` always point to the root directory, which can be used to specify absolute path to the target.

In //prelude/BUILD.gn, it depends on a target `"excerpt01_gn"`. This tells gn there is a subdirectory named `"excerpt01_gn"` under `//prelude`. `excerpt01_gn` subdirectory also has a BUILD.gn file which defines a target named "excerpt01_gn".

There are three ways to refer (i.e., label) to a target:

|Label type      |Example                   |Meaning                                                         |
|:-------------- |:------------------------ |:-------------------------------------------------------------- |
|Implicit label  |`//base`                  |short for //base:base, look for target "base" in //base/BUILD.gn|
|Full label      |`//chrome/browser:version`|look for target "version" in //chrome/browser/BUILD.gn          |
|Local label     |`:foo`                    |look for target "foo" in current file                           |


Assume the build directory specified is *out/debug*, the intermediate outputs can be found in *out/debug/obj/prelude/excerpt01_gn/hello_puppy* directory:

![hellO puppy intermediate output]({{ site.baseurl }}/crack_the_code/Chap_2/images/intermediate_output.png)

"bark_food" is built as a static library, so a *bark_food.lib* is generated. "sleep_run" is built as a source set, which does not generate a .lib file.

So now you know the basics about how to use gn to build executables, libraries. Next we will cover some advanced gn features.
