// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_MODEL_CLI_PARSER_H_
#define CORE_MODEL_CLI_PARSER_H_

#include <memory>
#include <string>
#include <vector>

#include "core/model/options.h"

namespace sf {
namespace core {
namespace model {

  class CLIOption;
  typedef std::shared_ptr<CLIOption> CLIOptionRef;

  //! Parses the command line options.
  /*!
   * Abstract class that defines what the command line parser will return.
   * How the parser is implemented or how the user interacts with it is
   * dependent on the concrete implementation in use.
   */
  class CLIParser : public Options {
   public:
    static void configOptions(CLIParser* parser);
    static void daemonOptions(CLIParser* parser);
    static void managerOptions(CLIParser* parser);
    static void spawnerOptions(CLIParser* parser);

   protected:
    std::vector<CLIOptionRef> options;

    //! Validates all the options.
    void validateOptions();

    //! Subclass definition of the parsing process.
    virtual void parseLogic(int* argc, char*** argv) = 0;

   public:
    CLIParser();
    virtual ~CLIParser();

    //! Adds a boolean option to the partser.
    void addBool(std::string name, std::string description, bool _default);

    //! Adds an option to the parser.
    void addOption(CLIOptionRef option);

    //! Adds a string option to the parser.
    void addString(std::string name, std::string description);
    void addString(
        std::string name, std::string description,
        std::string _default
    );

    //! Parses the command line options.
    /*!
     * @param argc The number of arguments passed to the program.
     * @param argv The array of C strings with the arguments to parse.
     */
    void parse(int* argc, char*** argv);
  };


  //! Supported option types.
  enum CLIOptionType {
    CLIT_BOOL = 0,
    CLIT_INT,
    CLIT_STRING
  };


  //! Comman line option definition.
  /*!
   * Abstract class to define a command line option.
   * A command option is a name, a description, an optional default value,
   * a value type, and a validation routine.
   */
  class CLIOption {
   protected:
    std::string _description;
    std::string _name;
    CLIOptionType _type;

    //! Command line parser the option belongs too.
    CLIParser* parser;

    //! Validates the the value of the option.
    virtual bool _validate() = 0;

   public:
    CLIOption(CLIOptionType type, std::string name, std::string description);
    ~CLIOption();

    //! @returns the description of the option.
    std::string description() const;

    //! @returns the name of the option.
    std::string name() const;

    //! @returns the type of the option's value.
    CLIOptionType type() const;

    //! Sets the default value of this option.
    virtual void setDefault();

    //! Sets the parser to act on.
    void setParser(CLIParser* parser);

    //! Validates the the value of the option.
    bool validate();
  };

namespace cli {

  //! Command line parser option for boolean values.
  class BoolOption : public CLIOption {
   protected:
    bool _default;
    std::string description;
    std::string name;

    bool _validate();

   public:
    BoolOption(
        std::string name, std::string description,
        bool _default
    );

    void setDefault();
  };

  //! Command line parser option for string values.
  class StringOption : public CLIOption {
   protected:
    std::string _default;
    std::string description;
    std::string name;
    bool set_default;

    bool _validate();

   public:
    StringOption(std::string name, std::string description);
    StringOption(
        std::string name, std::string description,
        std::string _default
    );

    void setDefault();
  };

}  // namespace cli

}  // namespace model
}  // namespace core
}  // namespace sf

#endif  // CORE_MODEL_CLI_PARSER_H_
