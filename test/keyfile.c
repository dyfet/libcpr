// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2025 David Sugar <tychosoft@gmail.com>

#undef  NDEBUG
#include <assert.h>
#include "../src/keyfile.h"

int main(int argc, char **argv) {
    keysection_t *root = load_keyfile(TEST_DATA "/test.conf");
    assert(root != NULL);
    keysection_t *section = find_keysection(root, "10");
    assert(section != NULL);
    assert(eq(section->id, "10"));
    assert(eq(get_keyvalue(section, "id"), "userid"));
    assert(eq(get_keyvalue(section, "secret"), "none"));
    assert(NULL == get_keyvalue(section, "XXX"));
    free(root);
}

