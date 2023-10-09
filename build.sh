#!/bin/sh
gcc ./eel.c -lraylib -Wall -Wextra -Werror -fsanitize=address,undefined -g3 -o eel
