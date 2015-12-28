// Copyright 2015 Stefano Pogliani <stafano@spogliani.net>
#ifndef CORE_COMPILE_TIME_OPTIONS_H_
#define CORE_COMPILE_TIME_OPTIONS_H_

// Template expansion information.
// Build configuration: {{__BUILD_TARGET__}}
// Commit sha: {{__GIT_SHA__}}
// Tainted: {{__GIT_TAINT__}}


// LUA related options.
#define LUA_HEADER_FILE     <{{findFile
    "/usr/include/lua.h"
    "/usr/include/lua5.2/lua.h"
}}>
#define LUA_AUX_HEADER_FILE <{{findFile
    "/usr/include/lauxlib.h"
    "/usr/include/lua5.2/lauxlib.h"
}}>


// Defaults.
#define DEFAULT_AUTOSTART_DELAY    5
#define DEFAULT_AUTOSTART_INTERVAL 1

#define DEFAULT_AUTOSTART_DELAY_NAME    "delay"
#define DEFAULT_AUTOSTART_INTERVAL_NAME "interval"

#define DEFAULT_LOG_FORMAT "{{default-log-format}}"  // NOLINT(whitespace/line_length)
#define DEFAULT_TICK 1


// System options.
#define DEBUG_BUILD {{debug}}
#define TRAP_SIGINT {{trap-sigint}}

#endif  // CORE_COMPILE_TIME_OPTIONS_H_
