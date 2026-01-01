#pragma once

#include <string>
#include <variant>
#include <vector>
#include "windows.h"

namespace TGW {
struct LoadModelCommand {
	std::string path;
};

struct SelectModelCommand {
	UINT id;
};

// TODO: simplify this...
using EditorCommand = std::variant<LoadModelCommand, SelectModelCommand>;
}