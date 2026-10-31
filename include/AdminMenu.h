#pragma once
// Admin panel: add / update stock / remove, plus undo & redo (stacks).
#include "Inventory.h"
#include "ActionStack.h"

void adminMenu(Inventory& inv, ActionStack& undoStack, ActionStack& redoStack);   // menu 7
