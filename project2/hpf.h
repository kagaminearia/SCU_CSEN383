#ifndef HPF_H
#define HPF_H

#include "process.h"
#include "util.h"

// Non-preemptive HPF
int hpf_np(Process **process_ptr, const char* output_filename);

// Preemptive HPF
int hpf_p(Process **process_ptr, const char* output_filename);

#endif // HPF_H
