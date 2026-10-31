#pragma once
// Customer-facing features: cart + wishlist, and the checkout queue.
#include "Inventory.h"
#include "Cart.h"
#include "Wishlist.h"
#include "OrderQueue.h"

void cartMenu(Inventory& inv, Cart& cart, Wishlist& wish);                              // menu 5
void processNextOrder(Inventory& inv, OrderQueue& queue);
void checkoutMenu(Inventory& inv, Cart& cart, OrderQueue& queue, int& nextOrderId);      // menu 6
