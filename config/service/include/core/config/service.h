// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CONFIG_SERVICE_H_
#define CORE_CONFIG_SERVICE_H_

#include <string>

#include "core/config/base.h"


namespace sf {
namespace core {
namespace config {

  //! Configurtion loader for service descriptions.
  /*!
   * The `load` promise settles to a ServiceDescription instance.
   *
   * The configuration script is loaded from a string passed to
   * the constructor.
   *
   * Only one service description can be returned by the
   * configuration script.
   *
   * Details of service configuration can be found in the
   * configuration reference book.
   */
  class ServiceLoader : public ConfigLoader_ {
   public:
    ServiceLoader(std::string script, std::string version);

   protected:
    const std::string script_;
    const std::string version_;

    // Extend base config logic.
    virtual void initLuaEnv();

    // Implement service specific logic.
    virtual ConfigStep collectStep();
    virtual poolqueue::Promise runLua();
  };

}  // namespace config
}  // namespace core
}  // namespace sf


#endif  // CORE_CONFIG_SERVICE_H_
