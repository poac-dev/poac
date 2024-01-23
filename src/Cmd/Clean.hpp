#pragma once

#include "../Rustify.hpp"
#include "Global.hpp"

#include <span>

extern const Subcmd cleanCmd;

int cleanMain(std::span<const StringRef> args) noexcept;
