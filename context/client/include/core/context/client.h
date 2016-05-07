// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_CONTEXT_CLIENT_H_
#define CORE_CONTEXT_CLIENT_H_

#include <memory>

#include "core/utility/lua.h"

namespace sf {
namespace core {
namespace context {

  //! Global context for the SnowFox Client.
  class Client {
   protected:
    sf::core::utility::Lua _lua;

    Client();

   public:
    ~Client();

   protected:
    static std::shared_ptr<Client> _instace;
    static void ensureInstance();

   public:
    static void destroy();
    static sf::core::utility::Lua lua();
  };

}  // namespace context
}  // namespace core
}  // namespace sf

#endif  // CORE_CONTEXT_CLIENT_H_
