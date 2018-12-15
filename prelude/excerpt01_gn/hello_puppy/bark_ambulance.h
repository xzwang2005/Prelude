// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#ifndef LETSHOWL_H_
#define LETSHOWL_H_

#if defined(WIN32)

#if defined(HOWL_IMPLEMENTATION)
#define HOWL_EXPORT __declspec(dllexport)
#define HOWL_EXPORT_PRIVATE __declspec(dllexport)
#else
#define HOWL_EXPORT __declspec(dllimport)
#define HOWL_EXPORT_PRIVATE __declspec(dllimport)
#endif  // defined(HOWL_IMPLEMENTATION)

#else

#if defined(HOWL_IMPLEMENTATION)
#define HOWL_EXPORT __attribute__((visibility("default")))
#define HOWL_EXPORT_PRIVATE __attribute__((visibility("default")))
#else
#define HOWL_EXPORT
#define HOWL_EXPORT_PRIVATE
#endif  // defined(HOWL_IMPLEMENTATION)

#endif

HOWL_EXPORT const char* LetsHowl();
#endif // LETSHOWL_H_