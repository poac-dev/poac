#pragma once

#include "../Rustify.hpp"

#include <span>

struct Subcmd; // forward decl for Global.hpp
extern const Subcmd helpCmd;

int helpMain(std::span<const StringRef> args) noexcept;
