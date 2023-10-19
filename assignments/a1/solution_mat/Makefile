#CONTIKI_PROJECT = hello-world

#CONTIKI = ../..
#include $(CONTIKI)/Makefile.include

# The name of your project
CONTIKI_PROJECT = compress
all: $(CONTIKI_PROJECT)

TARGET_LIBFILES = -lm

# The path to the Contiki source tree
CONTIKI = ../..

# The list of applications you want to include (none in this case)
APPS =

# The list of modules you want to include (only the core module in this case)
MODULES = os/core

# The list of modules you want to exclude (all the other modules in this case)
MODULES_REMOVE = os/*

MAKE_NET=MAKE_NET_NULLNET

# Include the system-wide Contiki makefile
include $(CONTIKI)/Makefile.include