// Copyright 2016 Stefano Pogliani <stafano@spogliani.net>
#include "core/exceptions/configuration.h"

using sf::core::exception::SfException;
using sf::core::exception::InvalidConfiguration;
using sf::core::exception::MissingConfiguration;

MSG_DEFINITION(SfException, InvalidConfiguration, -2000);
MSG_DEFINITION(SfException, MissingConfiguration, -2001);
