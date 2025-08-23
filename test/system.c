// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#undef  NDEBUG
#include <assert.h>
#include "../src/system.h"
#include "../src/pipeline.h"
#include "../src/waitgroup.h"
#include "../src/events.h"

static void test_events() {
#ifndef _WIN32
    event_t evt;
    assert(cpr_initevt(&evt) == true);
    assert(cpr_waitevt(&evt, 0) == false);
    assert(cpr_setevt(&evt) == true);
    assert(cpr_waitevt(&evt, 0) == true);
    assert(cpr_clearevt(&evt) == true);
    cpr_freeevt(&evt);
#endif
}

int main(int argc, char **argv) {
    test_events();
}

