#pragma once
// Customer-facing features: cart + wishlist, and the checkout queue.
#include "AppContext.h"

void cartMenu(AppContext& ctx);          // menu 5
void processNextOrder(AppContext& ctx);
void checkoutMenu(AppContext& ctx);      // menu 6
