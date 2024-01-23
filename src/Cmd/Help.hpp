#pragma once

#include "../Rustify.hpp"

#include <span>

class Subcmd; // forward decl for Global.hpp
extern const Subcmd helpCmd;

int helpMain(std::span<const StringRef> args) noexcept;
