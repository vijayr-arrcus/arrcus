#!/bin/bash

pcid_bg_internal=~/scripts/pcid_bg_internal.sh


export PCID_BG_VERBOSE=${PCID_BG_VERBOSE-0}  # Default no-verbose

run_fg_command=""
$run_fg_command "${pcid_bg_internal}" "$@"
