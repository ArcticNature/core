// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/model/cli-parser.h"

#include <iostream>
#include <string>
#include <vector>

#include "core/compile-time/version.h"
#include "core/exceptions/base.h"
#include "core/exceptions/options.h"

using sf::core::exception::CleanExit;
using sf::core::exception::ContextUninitialised;
using sf::core::exception::InvalidCommandLine;
using sf::core::exception::VariableNotFound;

using sf::core::model::CLIOption;
using sf::core::model::CLIOptionType;
using sf::core::model::CLIParser;

using sf::core::model::cli::BoolOption;
using sf::core::model::cli::StringOption;


void CLIParser::clusterOptions(CLIParser* parser) {
  parser->addString("node-name", "unique name of the node", "<not-set>");
}


void CLIParser::configOptions(CLIParser* parser) {
  parser->addString(
      "repo-type", "The type of configuration repository", "git"
  );
  parser->addString(
      "repo-path", "The path to the configuration repsitory"
  );
  parser->addString(
      "repo-version", "Version of the configuration to use", "<latest>"
  );
}

void CLIParser::miscOptions(CLIParser* parser) {
  parser->addBool("help", "show usage and exit", false);
  parser->addBool("version", "show version and exit", false);
}

void CLIParser::daemonOptions(CLIParser* parser) {
  // Daemonise behaviour.
  parser->addBool(
      "daemonise", "Run the process in daemon mode", true
  );
  parser->addBool(
      "drop-privileges", "Drop prividelges even when not in daemon mode",
      false
  );

  // Identity options.
  parser->addString(
      "group", "Group for the daemon to run as", "snow-fox"
  );
  parser->addString(
      "user", "User for the daemon to run as", "snow-fox"
  );

  // I/O redirects.
  parser->addString("stderr", "Redirect stderr to this file", "");
  parser->addString("stdin", "Redirect stdin to this file", "");
  parser->addString("stdout", "Redirect stdout to this file", "");
  parser->addString(
      "work-dir", "Change the working directory for the process", "."
  );

  // Unix sockets paths.
  parser->addString(
      "manager-socket",
      "Path to the socket file to connect daemon and manager.",
      "/var/run/snow-fox-manager.socket"
  );
  parser->addString(
      "spawner-socket",
      "Path to the socket file to connect daemon and spawner.",
      "/var/run/snow-fox-spawner.socket"
  );
  parser->addString(
      "spawner-manager-socket",
      "Path to the socket file to connect manager and spawner.",
      "/var/run/snow-fox-manager-spawner.socket"
  );
}

void CLIParser::managerOptions(CLIParser* parser) {
  // Unix sockets paths.
  parser->addString(
      "manager-socket",
      "Path to the socket file to connect daemon and manager.",
      "/var/run/snow-fox-manager.socket"
  );
  parser->addString(
      "spawner-manager-socket",
      "Path to the socket file to connect manager and spawner.",
      "/var/run/snow-fox-manager-spawner.socket"
  );
}

void CLIParser::spawnerOptions(CLIParser* parser) {
  // Unix sockets paths.
  parser->addString(
      "spawner-socket",
      "Path to the socket file to connect daemon and spawner.",
      "/var/run/snow-fox-spawner.socket"
  );
  parser->addString(
      "spawner-manager-socket",
      "Path to the socket file to connect manager and spawner.",
      "/var/run/snow-fox-manager-spawner.socket"
  );
}


CLIParser::CLIParser() { }
CLIParser::~CLIParser() { }

void CLIParser::addBool(
    std::string name, std::string description, bool _default
) {
  this->addOption(CLIOptionRef(new BoolOption(name, description, _default)));
}

void CLIParser::addOption(CLIOptionRef option) {
  this->options.push_back(option);
  option->setParser(this);
  option->setDefault();
}

void CLIParser::addString(std::string name, std::string description) {
  this->addOption(CLIOptionRef(new StringOption(name, description)));
}

void CLIParser::addString(
    std::string name, std::string description,
    std::string _default
) {
  this->addOption(CLIOptionRef(new StringOption(name, description, _default)));
}

void CLIParser::handleVersionOption(std::string prefix) {
  if (this->getBoolean("version")) {
    std::cout << prefix << std::endl;
    std::cout << "  Version: " << VERSION_NUMBER << std::endl;
    std::cout << "  Commit:  " << VERSION_SHA << std::endl;
    throw CleanExit();
  }
}

void CLIParser::parse(int* argc, char*** argv) {
  this->parseLogic(argc, argv);
  this->validateOptions();
}

void CLIParser::validateOptions() {
  // Pass check if --help or --version are set.
  if (this->getBoolean("help") || this->getBoolean("version")) {
    return;
  }

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
    throw ContextUninitialised("Option's parser not set");
  }
  return this->_validate();
}


BoolOption::BoolOption(
    std::string name, std::string description, bool _default
) : CLIOption(CLIOptionType::CLIT_BOOL, name, description) {
  this->_default = _default;
}

bool BoolOption::_validate() {
  try {
    this->parser->getBoolean(this->_name);
    return true;
  } catch(VariableNotFound& ex) {
    return false;
  }
}

void BoolOption::setDefault() {
  CLIOption::setDefault();
  this->parser->setBoolean(this->_name, this->_default);
}


StringOption::StringOption(
    std::string name, std::string description
) : CLIOption(CLIOptionType::CLIT_STRING, name, description) {
  this->_default = "";
  this->set_default = false;
}

StringOption::StringOption(
    std::string name, std::string description, std::string _default
) : CLIOption(CLIOptionType::CLIT_STRING, name, description) {
  this->_default = _default;
  this->set_default = true;
}

bool StringOption::_validate() {
  try {
    this->parser->getString(this->_name);
    return true;
  } catch(VariableNotFound& ex) {
    // TODO(stefano): properly support optional strings.
    return this->_default == "<not-set>" && this->set_default;
  }
}

void StringOption::setDefault() {
  CLIOption::setDefault();
  if (this->set_default && this->_default != "<not-set>") {
    // TODO(stefano): properly support optional strings.
    this->parser->setString(this->_name, this->_default);
  }
}
