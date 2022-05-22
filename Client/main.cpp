#include <iostream>
#include "UserInterface.h"

/*Using the Message messaging software, you must create an instance of the UserInterface class,
  "and use the start function to get the menu.*/
int main() {
    std::shared_ptr<UserInterface> ptr_server = std::make_shared<UserInterface>();
    ptr_server->start();
    return 0;
}
