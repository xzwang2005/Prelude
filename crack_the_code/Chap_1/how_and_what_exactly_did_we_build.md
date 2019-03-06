---
layout: default
title: How build process work?
---
## [](#header-2) How the build process works?

Let's take a closer look at Prelude repository. It's based on Chromium codebase version [71.0.3559.3](https://chromium.googlesource.com/chromium/src.git/+/71.0.3559.3) with some modifications, plus the folder *prelude* which contains all the examples.

Chromium source repository alone does not have everything. The build process pulls in hundreds of other projects, e.g., skia, as dependencies. Therefore, it has to know what to import and where to put it for later use. Such information is stored in a file named [*DEPS*](https://chromium.googlesource.com/chromium/src.git/+/71.0.3559.3/DEPS) at the root directory.

Open *DEPS*, variable *vars* stores three types of information:
1. some gn flags: `'checkout_nacl': True,`
2. server urls for core repositories: `'chromium_git': 'https://chromium.googlesource.com',`
3. revision for cour components: `'skia_revision': 'f610bae66163723d8fc3463546b4110c86345d79',`

Next, variable *deps* specifies information about libaries Chromium depends on. For example:

```
  'src/third_party/breakpad/breakpad':
    (Var("chromium_git")) + '/breakpad/breakpad.git@a61afe7a3e865f1da7ff7185184fe23977c2adca',
```

This means grab *breakpad* from: *https://chromium.googlesource.com/breakpad/breakpad.git* and sync to revision *a61afe7a3e865f1da7ff7185184fe23977c2adca*. The code should be downloaded to *src/third_party/breakpad/breakpad*.

*DEPS* also defines a list of *hooks* for extra works. For example, the following hook gets the *gn* executable we used in the last post:

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

These tools and external source code must be in place before build start. For Chromium, it is done when you run *fetch chromium*. In Prelude, all these nuts and bolts are already in repository (windows only), so that you can start using gn and fire up building process immediately.

Once the build is complete, all the artifacts (i.e., executables, dlls) are in *out/debug* folder, or whatever folder you specified in command *gn gen*. Examples of components built in Prelude include base library, v8 engine, ui service. We will get to these later. Many executables built in Prelude are unit tests. It is included because unit tests are one of the best resources to learn how the code works.
