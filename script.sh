#!/bin/sh
./gui/gui1 1500 &
./gui/gui1 1501 &
sleep 1
./cmake-build-debug/siktacka-client janusz localhost localhost:1501 &
./cmake-build-debug/siktacka-client marian localhost localhost:1500 &
