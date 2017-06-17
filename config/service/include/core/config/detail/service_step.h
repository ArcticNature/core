// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CONFIG_DETAIL_SERVICE_STEP_H_
#define CORE_CONFIG_DETAIL_SERVICE_STEP_H_

#include <string>

#include "core/config/base.h"
#include "core/service/description.h"


namespace sf {
namespace core {
namespace config {

  //! Configuration step to return a ServiceDescription.
  class ServiceDescriptionStep : public ConfigStep_ {
   public:
    ServiceDescriptionStep(ConfigLoader loader, std::string version);

    virtual poolqueue::Promise execute();
    virtual poolqueue::Promise verify();

   protected:
    sf::core::service::ServiceDescriptionBuilder builder_;
    std::string version_;
  };

}  // namespace config
}  // namespace core
}  // namespace sf

#endif  // CORE_CONFIG_DETAIL_SERVICE_STEP_H_
