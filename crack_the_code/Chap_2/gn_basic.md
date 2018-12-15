---
layout: default
title: GN build system
---
## [](#header-2) GN build system

Previously, we demonstrated how to build Prelude with just a few commands. However, to truly be able to reuse Chromium code, you need to learn a bit more about the build system - GN. GN is a command line tool similar to *git*. It supports several sub-commands, such as `gen`, `args` used before. GN works with build files named BUILD.gn whose syntax is similar to Python, in which defines targets to be built with GN jargon, e.g., `bundle_data`, `config`.

### [](#header-3) Basic
Using GN is to write a number of BUILD.gn files organized in a tree-like structure. Here's a [quick starter](https://chromium.googlesource.com/chromium/src/tools/gn/+/48062805e19b4697c5fbd926dc649c78b6aaa138/docs/quick_start.md) from Chromium.

To get our hands dirty, let's play with *hello_bogart* executable, which spit out the following when executed:
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
*`sources` specifies all the source files to be compiled.
*`output_name` tells GN that the output binary should be named "hello_bogart" instead of the target name "hello_puppy" by default.
*`deps` lists all the libraries (or targets) required to build the binary.

Pay attention to how the executable specify the targets it depends on: `":bark_food"`. The format *:target_name* is used to point to the target within the same BUILD.gn file. There are three ways to refer a target:

|Label type      |Example                   |Meaning                                                         |
|:-------------- |:------------------------ |:-------------------------------------------------------------- |
|Implicit label  |`//base`                  |short for //base:base, look for target "base" in //base/BUILD.gn|
|Full label      |`//chrome/browser:version`|look for target "version" in //chrome/browser/BUILD.gn          |
|Local label     |`:foo`                    |look for target "foo" within the same file                      |


In the shared library target, `defines` is used to store the list of preprocessor defines:
```gn
shared_library("bark_ambulance") {
  sources = [
    "bark_ambulance.cpp",
  ]
  defines = [ "HOWL_IMPLEMENTATION" ]
}
```

Here, `HOWL_IMPLEMENTATION` is defined, which is used in c++ code [bark_ambulance.h]()

```c++
#if defined(HOWL_IMPLEMENTATION)
#define HOWL_EXPORT __declspec(dllexport)
#define HOWL_EXPORT_PRIVATE __declspec(dllexport)
#else
#define HOWL_EXPORT __declspec(dllimport)
#define HOWL_EXPORT_PRIVATE __declspec(dllimport)
#endif  // defined(HOWL_IMPLEMENTATION)
```
This BUILD.gn file contains all the instructions required to build the executable. However, it alone does not "trigger" the build because it's just a "leaf" in the build tree.

![build tree]({{ site.baseurl }}/crack_the_code/Chap_2/images/build_tree.png)

Take a look at the figure above, there is a BUILD.gn file at each level from root directory all the way to the folder that contains the source files. When we run `gn gen`, GN goes through all BUILD.gn files starting from the top level, pick up all the targets defined in there, then move to the next level recursively. For simplicity, all non-leaf BUILD.gn files in this example only defines one built-in target - `group`.

`group` conceptually is equivalent to the shortcut or symbolic link in OS. It contains nothing but a point to a set of dependencies, causing those targets to be picked up for build. In the figure above, the chain of targets is highlighted in red. The root build file (//BUILD.gn) depends on `"//prelude"`. This means gn expects there will be a `//prelude/BUILD.gn` file, which defines a target named `"prelude"`. ```//``` always point to the root directory in GN.

In //prelude/BUILD.gn, it depends on a target `"excerpt01_gn"`. This tells gn there is a subdirectory named `"excerpt01_gn"` under `//prelude`. `excerpt01_gn` subdirectory also has a BUILD.gn file which defines a target named "excerpt01_gn".

Assume the build directory specified is *out/debug*, the intermediate outputs can be found in *out/debug/obj/prelude/excerpt01_gn/hello_puppy* directory:

![hellO puppy intermediate output]({{ site.baseurl }}/crack_the_code/Chap_2/images/intermediate_output.png)

"bark_food" is built as a static library, so a *bark_food.lib* is generated. "sleep_run" is built as a source set, which does not generate a .lib file.

### [](#header-3) Advanced features
GN is a huge topic on its own. Frequently you may run into unknowns when browsing BUILD.gn files. The easiest way to find the answer is to use GN's help command. For example, you see the word `template` for the first time and want to know what it does. Go to the root directory and type:
```
gn help template
```

Another useful resource highly recommended is [this presentation](https://docs.google.com/presentation/d/15Zwb53JcncHfEwHpnG_PoIbbzQ3GQi_cpujYwbpcbZo/edit?usp=sharing) by Brett Wilson. It covers advanced topics such as toolchain, as well as many useful tricks for daily tasks, such as how to find out where is an argument set in the build tree. Hopefully, these material will get you started with GN and keep getting more comfortable with GN by using it.