// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#include "core/model/cli-parser.h"


using sf::core::model::CLIParser;


void CLIParser::checkRequiredOptions() {
  // Daemonise options.
  this->getBoolean("daemonise");
  this->getString("group");
  this->getString("user");
  this->getString("stderr");
  this->getString("stdin");
  this->getString("stdout");
  this->getString("work-dir");

  // Repository options.
  this->getString("repo-type");
  this->getString("repo-version");
  this->getString("repo-path");
}

void CLIParser::setDefaultValues() {
  // Daemonise options.
  this->setBoolean("daemonise", true);
  this->setString("group", "snow-fox");
  this->setString("user", "snow-fox");
  this->setString("stderr", "/dev/null");
  this->setString("stdin", "/dev/null");
  this->setString("stdout", "/dev/null");
  this->setString("work-dir", ".");

  // Repository options.
  this->setString("repo-type", "git");
  this->setString("repo-version", "<latest>");
}


CLIParser::CLIParser() {
  this->setDefaultValues();
}
CLIParser::~CLIParser() { }

void CLIParser::parse(int* argc, char*** argv) {
  this->parseLogic(argc, argv);
  this->checkRequiredOptions();
}
