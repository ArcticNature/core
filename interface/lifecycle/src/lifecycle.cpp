// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/interface/lifecycle.h"

#include <list>
#include <string>

#include "core/exceptions/base.h"

using sf::core::exception::AbortException;
using sf::core::exception::SfException;
using sf::core::exception::StopException;

using sf::core::interface::BaseLifecycleArg;
using sf::core::interface::BaseLifecycleHandler;

using sf::core::interface::Lifecycle;
using sf::core::interface::LifecycleHandlerRef;
using sf::core::interface::LifecycleInstance;


BaseLifecycleArg::~BaseLifecycleArg() {}
BaseLifecycleHandler::~BaseLifecycleHandler() {}


void LifecycleInstance::off(
    std::string event, LifecycleHandlerRef handler
) {
  this->handlers[event].remove(handler);
}

void LifecycleInstance::on(
    std::string event, LifecycleHandlerRef handler
) {
  this->handlers[event].push_back(handler);
}

void LifecycleInstance::trigger(std::string event, BaseLifecycleArg* arg) {
  std::list<LifecycleHandlerRef> to_call;
  std::list<LifecycleHandlerRef>::iterator handler;
  to_call = this->handlers[event];

  for (handler = to_call.begin(); handler != to_call.end(); handler++) {
    try {
      (*handler)->handle(event, arg);
    } catch (AbortException& ex) { throw;
    } catch (StopException& ex)  { return;
    } catch (SfException& ex)    { continue;
    }
  }
}


LifecycleInstance* Lifecycle::instance;

LifecycleInstance* Lifecycle::getInstance() {
  if (Lifecycle::instance == nullptr) {
    Lifecycle::instance = new LifecycleInstance();
  }
  return Lifecycle::instance;
}

void Lifecycle::off(std::string event, LifecycleHandlerRef handler) {
  Lifecycle::getInstance()->off(event, handler);
}

void Lifecycle::on(std::string event, LifecycleHandlerRef handler) {
  Lifecycle::getInstance()->on(event, handler);
}

void Lifecycle::trigger(std::string event, BaseLifecycleArg* arg) {
  Lifecycle::getInstance()->trigger(event, arg);
}


#if DEBUG_BUILD
void LifecycleInstance::reset() {
  this->handlers.clear();
}

void Lifecycle::reset() {
  Lifecycle::getInstance()->reset();
}
#endif  // DEBUG_BUILD
