#!/bin/bash
xhost +local:docker
docker run -it --rm --net=host --name=peb-dev -e DISPLAY=$DISPLAY -v ~/.Xauthority:/home/pebble/.Xauthority -v /tmp/.X11-unix:/tmp/.X11-unix -v $PWD:/pebble/ bboehmke/pebble-dev sh -c 'pebble build && pebble install --emulator basalt -vvv && bash'
