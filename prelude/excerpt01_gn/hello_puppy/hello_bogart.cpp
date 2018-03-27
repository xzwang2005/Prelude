// Copyright (c) 2018 The Prelude Authors.
// Use of this source code is governed by MIT license that can be
// found in MIT-LICENSE file.

#ifndef HELLO_BOGART_H_
#define HELLO_BOGART_H_

#include <stdio.h>
#include "bark_food.h"
#include "bark_ambulance.h"
#include "sleep_run.h"

int main() {
    printf("Hello, Bogart.\n");
    printf("%s\n", LetsEat());
    printf("%s\n", LetsHowl());
    printf("%s\n", LetsRun());
    return 0;
}
#endif // HELLO_BOGART_H_