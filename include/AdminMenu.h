#pragma once
// Admin panel: add / update stock / remove, plus undo & redo (stacks).
#include "AppContext.h"

// menu 7. Returns true when the user chose "Reload all data", so the caller can rebuild everything.
bool adminMenu(AppContext& ctx);
