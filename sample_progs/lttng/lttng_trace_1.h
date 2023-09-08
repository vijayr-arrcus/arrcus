#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER lttng_trace

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./lttng_trace_1.h"

#if !defined(LTTNG_TP_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define LTTNG_TP_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(
                 lttng_trace,
                 trace_1,
                 TP_ARGS(char *, msg),
                 TP_FIELDS(ctf_string(msg, msg))
                )

TRACEPOINT_LOGLEVEL(lttng_trace, trace_1, TRACE_DEBUG_SYSTEM)

TRACEPOINT_EVENT(
                 lttng_trace,
                 trace_2,
                 TP_ARGS(int, var),
                 TP_FIELDS(ctf_integer(int, var, var))
                )

TRACEPOINT_LOGLEVEL(lttng_trace, trace_2, TRACE_DEBUG_SYSTEM)
#endif

#include <lttng/tracepoint-event.h>
