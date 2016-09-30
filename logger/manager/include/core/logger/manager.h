// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_LOGGER_MANAGER_H_
#define CORE_LOGGER_MANAGER_H_

#include "core/logger/manager.h"

namespace sf {
namespace core {
namespace logger {

  //! Sends logs to the manager over protocol buffers.
  /*!
   * ?
   */
  class ManagerLogger : public sf::core::model::Logger {
  };

}  // namespace logger
}  // namespace core
}  // namespace sf

#endif  // CORE_LOGGER_MANAGER_H_
