// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/model/cli-parser.h"

#include "core/exceptions/base.h"
#include "core/exceptions/options.h"

using sf::core::exception::ContextUninitialised;
using sf::core::exception::InvalidCommandLine;
using sf::core::exception::VariableNotFound;

using sf::core::model::CLIOption;
using sf::core::model::CLIOptionType;
using sf::core::model::CLIParser;

using sf::core::model::cli::BoolOption;
using sf::core::model::cli::StringOption;


void CLIParser::configOptions(CLIParser* parser) {
  parser->addOption(CLIOptionRef(new StringOption(
      "repo-type", "The type of configuration repository", "git", true
  )));
  parser->addOption(CLIOptionRef(new StringOption(
      "repo-path", "The path to the configuration repsitory", true
  )));
  parser->addOption(CLIOptionRef(new StringOption(
      "repo-version", "Version of the configuration to use", "<latest>", true
  )));
}

void CLIParser::daemonOptions(CLIParser* parser) {
  parser->addOption(CLIOptionRef(new BoolOption(
      "daemonise", "Run the process in daemon mode", true, true
  )));
  parser->addOption(CLIOptionRef(new StringOption(
      "group", "Group for the daemon to run as", "snow-fox", true
  )));
  parser->addOption(CLIOptionRef(new StringOption(
      "user", "User for the daemon to run as", "snow-fox", true
  )));

  parser->addOption(CLIOptionRef(new StringOption(
      "stderr", "Redirect stderr to this file", "/dev/null", true
  )));
  parser->addOption(CLIOptionRef(new StringOption(
      "stdin", "Redirect stdin to this file", "/dev/null", true
  )));
  parser->addOption(CLIOptionRef(new StringOption(
      "stdout", "Redirect stdout to this file", "/dev/null", true
  )));
  parser->addOption(CLIOptionRef(new StringOption(
      "work-dir", "Change the working directory for the process", ".", true
  )));
}


CLIParser::CLIParser() { }
CLIParser::~CLIParser() { }

void CLIParser::addOption(CLIOptionRef option) {
  this->options.push_back(option);
  option->setParser(this);
  option->setDefault();
}

void CLIParser::parse(int* argc, char*** argv) {
  this->parseLogic(argc, argv);
  this->validateOptions();
}

void CLIParser::validateOptions() {
  std::vector<CLIOptionRef>::iterator it;
  for (it = this->options.begin(); it != this->options.end(); it++) {
    std::string name = (*it)->name();
    bool valid = (*it)->validate();
    if (!valid) {
      throw InvalidCommandLine(
          "Option '" + name + "' is not valid or missing"
      );
    }
  }
}


CLIOption::CLIOption(
    CLIOptionType type, std::string name,
    std::string description
) {
  this->_description = description;
  this->_name = name;
  this->_type = type;
  this->parser = nullptr;
}
CLIOption::~CLIOption() {}

std::string CLIOption::description() const {
  return this->_description;
}

std::string CLIOption::name() const {
  return this->_name;
}

CLIOptionType CLIOption::type() const {
  return this->_type;
}

void CLIOption::setDefault() {
  if (this->parser == nullptr) {
    throw ContextUninitialised("option's parser not set");
  }
}

void CLIOption::setParser(CLIParser* parser) {
  this->parser = parser;
}

bool CLIOption::validate() {
  if (this->parser == nullptr) {
    throw ContextUninitialised("option's parser not set");
  }
  return this->_validate();
}


BoolOption::BoolOption(
    std::string name, std::string description, bool required
) : CLIOption(CLIOptionType::CLIT_BOOL, name, description) {
  this->_default = false;
  this->required = required;
  this->set_default = false;
}

BoolOption::BoolOption(
    std::string name, std::string description,
    bool _default, bool required
) : CLIOption(CLIOptionType::CLIT_BOOL, name, description) {
  this->_default = _default;
  this->required = required;
  this->set_default = true;
}

bool BoolOption::_validate() {
  try {
    this->parser->getBoolean(this->_name);
    return true;
  } catch(VariableNotFound& ex) {
    return !this->required;
  }
}

void BoolOption::setDefault() {
  CLIOption::setDefault();
  if (this->set_default) {
    this->parser->setBoolean(this->_name, this->_default);
  }
}


StringOption::StringOption(
    std::string name, std::string description, bool required
) : CLIOption(CLIOptionType::CLIT_STRING, name, description) {
  this->_default = "";
  this->required = required;
  this->set_default = false;
}

StringOption::StringOption(
    std::string name, std::string description,
    std::string _default, bool required
) : CLIOption(CLIOptionType::CLIT_STRING, name, description) {
  this->_default = _default;
  this->required = required;
  this->set_default = true;
}

bool StringOption::_validate() {
  try {
    this->parser->getString(this->_name);
    return true;
  } catch(VariableNotFound& ex) {
    return !this->required;
  }
}

void StringOption::setDefault() {
  CLIOption::setDefault();
  if (this->set_default) {
    this->parser->setString(this->_name, this->_default);
  }
}
