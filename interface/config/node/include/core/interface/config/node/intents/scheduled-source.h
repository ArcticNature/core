// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#ifndef CORE_INTERFACE_CONFIG_NODE_INTENTS_SCHEDULED_SOURCE_H_
#define CORE_INTERFACE_CONFIG_NODE_INTENTS_SCHEDULED_SOURCE_H_

#include <string>
#include <vector>

#include "core/context/context.h"
#include "core/interface/config/node.h"

namespace sf {
namespace core {
namespace interface {

  //! Ensure that the ScheduledSource exists and is configured.
  /*!
   * If a ScheduledSource source is not configured already
   * one will be added with a tick of 1.
   *
   * If the current ScheduledSource stored any callbacks
   * that must be kept on configuration reload those callbacks
   * are registered with the new ScheduledSource too.
   */
  class DefaultScheduledSourceIntent : public NodeConfigIntent {
   protected:
    static const std::vector<std::string> AFTER;
    static const std::vector<std::string> DEPS;

   public:
    DefaultScheduledSourceIntent();

    std::vector<std::string> after() const;
    std::vector<std::string> depends() const;
    std::string provides() const;

    void apply(sf::core::context::ContextRef context);
    void verify(sf::core::context::ContextRef context);
  };


  //! Create a ScheduledSource based on user configuration.
  class ScheduledSourceIntent : public NodeConfigIntent {
   protected:
    static const std::vector<std::string> DEPS;

   protected:
    int tick;

   public:
    explicit ScheduledSourceIntent(int tick);

    std::vector<std::string> depends() const;
    std::string provides() const;

    void apply(sf::core::context::ContextRef context);
    void verify(sf::core::context::ContextRef context);
  };

}  // namespace interface
}  // namespace core
}  // namespace sf

#endif  // CORE_INTERFACE_CONFIG_NODE_INTENTS_SCHEDULED_SOURCE_H_
