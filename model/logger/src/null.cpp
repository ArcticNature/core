// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/model/logger/null.h"

#include <string>

using sf::core::model::NullLogger;
using sf::core::model::Logger;
using sf::core::model::LogLevel;
using sf::core::model::LogInfo;


NullLogger::NullLogger() : Logger("") {}

void NullLogger::log(
    const LogLevel level, const std::string message,
    LogInfo variables
) {
}
