// Copyright 2016 Stefano Pogliani <stefano@spogliani.net>
#include "core/lifecycle/event.h"

#include <map>
#include <sstream>
#include <string>
#include <typeinfo>

#include "core/compile-time/options.h"
#include "core/context/static.h"
#include "core/model/event.h"

using sf::core::context::Static;

using sf::core::interface::Lifecycle;
using sf::core::interface::LifecycleArgRef;
using sf::core::interface::LifecycleHandler;

using sf::core::lifecycle::EventLifecycle;
using sf::core::lifecycle::EventLifecycleArg;

using sf::core::model::Event;
using sf::core::model::EventRef;


std::map<std::string, uint64_t> event_id_sequences;


void EventLifecycle::Init(EventRef event) {
  LifecycleArgRef arg(new EventLifecycleArg(event));
  Lifecycle::trigger("event::_identify",  arg.get());
  Lifecycle::trigger("event::initialise", arg.get());
}


EventLifecycleArg::EventLifecycleArg(EventRef event) {
  this->event = event;
}


//! Handler for event::_identify that sets the event id.
class EventIdentifyHandler : public LifecycleHandler<EventLifecycleArg> {
 public:
  void handle(std::string lc_event, EventLifecycleArg* argument) {
    // Collect needed information.
    std::string proc_name = Static::options()->getString("process-name");
    Event* event = argument->event.get();
    std::string type_name   = typeid(*event).name();
    uint64_t instance_count = event_id_sequences[type_name]++;

    // Build and set the id.
    std::stringstream id;
    id << proc_name << "!" << type_name << "#" << instance_count;
    argument->event->id(id.str());
  }
};


#if TEST_BUILD
void EventLifecycle::reset() {
  event_id_sequences.clear();
}
#endif


// Static registration of handlers.
LifecycleStaticOn("event::_identify", EventIdentifyHandler);
