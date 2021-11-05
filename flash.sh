#!/bin/bash
set -exuo pipefail
PROJECT_ROOT="/home/roiz/Codes/found"
source "$PROJECT_ROOT/zephyr/zephyr-env.sh"
source "$PROJECT_ROOT/venv/bin/activate"
cd "$PROJECT_ROOT/Recognizing_bike_collision"
west -v flash
