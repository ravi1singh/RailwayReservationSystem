#!/bin/bash

gcc ./src/client.c ./src/clientModules.c -o ./executables/client
gcc ./src/admin.c ./src/normal.c ./src/agent.c ./src/server.c ./src/serverModules.c -o ./executables/server

