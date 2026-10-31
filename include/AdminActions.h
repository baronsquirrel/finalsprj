#pragma once
// Undo / redo logic that applies stack entries (Lab 4) to the inventory.
#include "Inventory.h"
#include "ActionStack.h"

void adjustStock(Inventory& inv, int id, int delta);
bool applyUndo(Inventory& inv, const AdminAction& a);   // reverses an action, prints result
bool applyRedo(Inventory& inv, const AdminAction& a);   // re-applies an undone action
