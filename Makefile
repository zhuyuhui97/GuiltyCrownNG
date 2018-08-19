#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := guiltycrown-esp

CFLAGS += -D ESP32

include $(IDF_PATH)/make/project.mk

