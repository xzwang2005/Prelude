---
layout: default
title: How exactly does the build work?
---
## [](#header-2) How exactly does the build work?

Now, let's take a look at Prelude repository. It's based on Chromium release [71.0.3559.3](https://chromium.googlesource.com/chromium/src.git/+/71.0.3559.3) with some modifications, plus the folder *prelude* which contains all the examples.

Chromium source repository alone does not include everything. It pulls in hundreds of other projects, e.g., skia, as dependencies. You may wonder how to know what projects, at which revision are imported for a particular Chromium release? The answer can be found in the file named *DEPS* like [this one](https://chromium.googlesource.com/chromium/src.git/+/71.0.3559.3/DEPS) at the root directory.

Open that file, you will see a list of arguments stored in variable *vars*, for example, *chromium_git* is defined to be *https://chromium.googlesource.com*, which points to the server hosting many projects needed by Chromium.

Then it defines the variable *deps*, which specifies where to find the code, which revision to use and the destination folder relative to the root directory. For example:

```
  'src/third_party/breakpad/breakpad':
    (Var("chromium_git")) + '/breakpad/breakpad.git@a61afe7a3e865f1da7ff7185184fe23977c2adca',
```

This line of code means grab *breakpad* from: *https://chromium.googlesource.com/breakpad/breakpad.git* and sync to revision *a61afe7a3e865f1da7ff7185184fe23977c2adca*. The code should be downloaded to *src/third_party/breakpad/breakpad*.

DEPS also defines a list of *hooks* for extra works. For example, the following hook gets the *gn* executable we used in the last post:

```
  {
    'action': [
      'vpython',
      'src/third_party/depot_tools/download_from_google_storage.py',
      '--no_resume',
      '--no_auth',
      '--bucket',
      'chromium-gn',
      '-s',
      'src/buildtools/win/gn.exe.sha1'
    ],
    'pattern':
      '.',
    'name':
      'gn_win',
    'condition':
      'host_os == "win"'
  },
```

 *action* specifies the command to run; *condition* dictates that this hook should only run on windows machines. You can find similar hooks to get gn for mac and linux.

These tools and external source code must be in place before build start. For Chromium, it is done when you run *fetch chromium*. In Prelude, all these nuts and bolts are already stored in repository, so that you can start using gn and fire up building process immediately.

Once the build is complete, all the artifacts (i.e., executables, dlls) are in *out/debug* folder, or whatever folder you specified in command *gn gen*. Examples of components built in Prelude include base library, v8 engine, ui service. We will get to these later. Many executables built in Prelude are unit tests. It is included because unit tests are one of the best resources to learn how the code works.
