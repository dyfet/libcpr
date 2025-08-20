// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#undef  NDEBUG
#include <assert.h>
#include "../src/string.h"

int main(int argc, char **argv) {
    char *hello = "hello";
    assert(cpr_strlen(hello, 80) == 5);
    assert(cpr_strlen("hellohere", 5) == 5);
    assert(cpr_strlen(NULL, 80) == 0);
    assert(eq("ell", cpr_strdup(hello + 1, 3)));  // NOLINT

    char *untrimmed = "  hello";
    assert(eq(cpr_strtrim(untrimmed, " ", 16), "hello"));
}

