// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/context/event.h"

#include <string>

using sf::core::context::EventContextCleanupHandler;
using sf::core::interface::BaseLifecycleArg;
using sf::core::interface::BaseLifecycleHandler;


EventContextCleanupHandler::EventContextCleanupHandler(
    void (*deleter)()
) : BaseLifecycleHandler() {
  this->deleter = deleter;
}

void EventContextCleanupHandler::handle(
    std::string event, BaseLifecycleArg* argument
) {
  this->deleter();
}
