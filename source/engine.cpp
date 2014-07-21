// engine.cpp
// created: 1:17AM, 2/3/2014
// author: Daniel Sparks
// license: See text/license.txt
// purpose: The engine class.

#include "exacto.h"
#include "inlines.h"

CEngine::CEngine(void) {
    mt = 4;
    post_pretty = true;
    reset_engine();
    set_levels(0, 300, 6);
}

CEngine::~CEngine(void) {
}

void CEngine::st(int seconds) {
    time_use_exact = true;
    time_levels[2] = seconds;
}

void CEngine::set_levels(int MPS, int base, int inc) {
    time_levels[0] = MPS;
    time_levels[1] = base;
    time_levels[2] = inc;

    time_cs_left = base * 100;
    time_o_cs_left = base * 80;

    time_surplus = 0;
    time_use_exact = false;
}

void CEngine::time(int centiseconds) {
    time_cs_left = centiseconds;
}

void CEngine::otim(int centiseconds) {
    time_o_cs_left = centiseconds;
}

void CEngine::reset_engine() {
    post = false;
    force = false;
    total_nodes = 0;
    total_cs_used = 0;
    branching_factor = 0;

    draw_offered = false;
    if(is_book(false)) book_mode = _BOOK_MODE;

    engine_color = UNASSIGNED;

    time_cs_left = time_levels[1] * 100;
    time_o_cs_left = time_levels[1] * 80;

    time_surplus = 0;
    time_use_exact = false;

    hash.clear_table();
    opponent_name = "?";
}