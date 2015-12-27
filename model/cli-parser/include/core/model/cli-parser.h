// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_MODEL_CLI_PARSER_H_
#define CORE_MODEL_CLI_PARSER_H_

#include "core/model/options.h"

namespace sf {
namespace core {
namespace model {

  //! Parses the command line options.
  /*!
   * Abstract class that defines what the command line parser will return.
   * How the parser is implemented or how the user interacts with it is
   * dependent on the concrete implementation in use.
   */
  class CLIParser : public Options {
   protected:
    //! Ensure that all required options have been set.
    void checkRequiredOptions();

    //! When an instance is created, set the default option values.
    void setDefaultValues();

    //! Subclass definition of the parsing process.
    virtual void parseLogic(int* argc, char*** argv) = 0;

   public:
    CLIParser();
    virtual ~CLIParser();

    //! Parses the command line options.
    /*!
     * @param argc The number of arguments passed to the program.
     * @param argv The array of C strings with the arguments to parse.
     */
    void parse(int* argc, char*** argv);
  };

}  // namespace model
}  // namespace core
}  // namespace sf

#endif  // CORE_MODEL_CLI_PARSER_H_
