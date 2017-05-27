// Copyright 2015 Stefano Pogliani <stafano@spogliani.net>
#ifndef CORE_COMPILE_TIME_OPTIONS_H_
#define CORE_COMPILE_TIME_OPTIONS_H_

// Template expansion information.
// Build configuration: {{__BUILD_TARGET__}}
// Commit sha: {{__GIT_SHA__}}
// Tainted: {{__GIT_TAINT__}}


// Constants.
#define HOSTNAME_BUFFER_LEN 1024


// Defaults.
#define DEFAULT_AUTOSTART_DELAY    5
#define DEFAULT_AUTOSTART_INTERVAL 1

#define DEFAULT_AUTOSTART_DELAY_NAME    "delay"
#define DEFAULT_AUTOSTART_INTERVAL_NAME "interval"

#define DEFAULT_BUFFER_SIZE 1024

#define DEFAULT_LOG_FORMAT "{{default-log-format}}"  // NOLINT(whitespace/line_length)
#define DEFAULT_TICK 1


// System options.
#define DEAFULT_EVENT_SOURCE_MANAGER "epoll"
#define DEBUG_BUILD {{debug}}
#define TEST_BUILD  {{test}}
#define TRAP_SIGINT {{trap-sigint}}

#endif  // CORE_COMPILE_TIME_OPTIONS_H_
