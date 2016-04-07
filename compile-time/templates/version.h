// Copyright 2016 Stefano Pogliani <stafano@spogliani.net>
#ifndef CORE_COMPILE_TIME_VERSION_H_
#define CORE_COMPILE_TIME_VERSION_H_

// Template expansion information.
// Build configuration: {{__BUILD_TARGET__}}
// Commit sha: {{__GIT_SHA__}}
// Tainted: {{__GIT_TAINT__}}

#define VERSION_NUMBER "0.0.1"
#define VERSION_SHA    "{{ __GIT_SHA__ 0 }}"
#define VERSION_TAINT  "{{ __GIT_TAINT__ }}"

#endif  // CORE_COMPILE_TIME_VERSION_H_
