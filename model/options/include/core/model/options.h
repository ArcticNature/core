// Copyright 2015 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_MODEL_OPTIONS_H_
#define CORE_MODEL_OPTIONS_H_

#include <map>
#include <string>


namespace sf {
namespace core {
namespace model {

  //! Generic variables container.
  /*!
   * Instances of this class can be used to store dinamically
   * defined varibles.
   *
   * Variables are typed and the type is made explicit when variables
   * are set or retreived.
   */
  class Options {
   protected:
    std::map<std::string, bool> booleans;
    std::map<std::string, int>  integers;
    std::map<std::string, std::string> strings;

   public:
    bool getBoolean(std::string name);
    int  getInteger(std::string name);
    std::string getString(std::string name);

    bool hasBoolean(std::string name);
    bool hasInteger(std::string name);
    bool hasString(std::string name);

    void setBoolean(std::string name, bool value);
    void setInteger(std::string name, int value);
    void setString(std::string name, std::string value);
  };

}  // namespace model
}  // namespace core
}  // namespace sf

#endif  // CORE_MODEL_OPTIONS_H_
