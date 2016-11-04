#pragma once
#include "cexacto.h"
#include "cgame.cpp"
#include "cexacto_evaluate.cpp"
#include "cexacto_search.cpp"

CExacto::CExacto(CGame initGame) {
    game = initGame;
}

CExacto::~CExacto() {
}