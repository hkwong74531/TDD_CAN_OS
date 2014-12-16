#Set this to @ to keep the makefile quiet
SILENCE = @

#---- Outputs ----#
COMPONENT_NAME = CANProtocol_CppUTest
TARGET_LIB = \
	lib/lib$(COMPONENT_NAME).a
	
TEST_TARGET = \
	$(COMPONENT_NAME)_tests

#--- Inputs ----#
PROJECT_HOME_DIR = .
CPP_PLATFORM = Gcc

SRC_DIRS = \
	$(PROJECT_HOME_DIR)/src/CAN \

TEST_SRC_DIRS = \
	tests/CAN\
	tests/\

INCLUDE_DIRS =\
  .\
  $(CPPUTEST_HOME)/include\
  $(PROJECT_HOME_DIR)/include/CAN\
  
#CPPUTEST_WARNINGFLAGS += -pedantic-errors -Wconversion -Wshadow  -Wextra
CPPUTEST_WARNINGFLAGS += -Wall -Werror -Wswitch-default -Wswitch-enum 


include $(CPPUTEST_HOME)/build/MakefileWorker.mk
