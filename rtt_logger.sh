#!/bin/bash
set -exuo pipefail
PROJECT_ROOT="/home/roiz/Codes/found"
source "$PROJECT_ROOT/zephyr/zephyr-env.sh"
source "$PROJECT_ROOT/venv/bin/activate"
cd "$PROJECT_ROOT/pebblebee-found11-fw"
JLinkRTTLogger -Device NRF52833 -if SWD -Speed 4000 -RTTChannel 0
