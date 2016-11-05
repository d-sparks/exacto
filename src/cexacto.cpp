#pragma once
#include "cexacto.h"
#include "cgame.cpp"
#include "cexacto_evaluate.cpp"
#include "cexacto_search.cpp"
#include "chash.cpp"

CExacto::CExacto(CGame initGame) {
    game = initGame;
}

CExacto::~CExacto() {
}
