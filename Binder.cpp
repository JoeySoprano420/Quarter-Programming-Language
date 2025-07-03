// Binder.h
#pragma once
#include "AST.h"
#include <string>
#include <unordered_set>
class Binder {
public:
    void bind(FuncDecl* root) {
        // For now, simple: check main exists, walk body for errors
    }
};
