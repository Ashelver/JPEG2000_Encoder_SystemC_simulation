# Setting
CC = /usr/bin/g++
ARCH := $(shell arch)
SYSTEMC_HOME = /usr/local/SystemC-2.3.0

# Library path
LINUXLIB := $(shell if [ ${ARCH} = "i686" ]; \
                    then \
                         echo lib-linux; \
                    else \
                         echo lib-linux64; \
                    fi)

# Included path
INCLUDES = -I$(SYSTEMC_HOME)/include -I.
INCLUDES += -I./include

# Linked Library
LIBRARIES = -L$(SYSTEMC_HOME)/$(LINUXLIB) -L. -lsystemc -lpng -lm

# RPATH to connect to SystemC package
RPATH = -Wl,-rpath=$(SYSTEMC_HOME)/$(LINUXLIB)

# Program name and source code
PROGRAM = jpeg2000_encoder.x
SRCS = src/sc_main.cpp src/DataStruct.cpp src/preprocess_module.cpp src/dwt_module.cpp src/quan_module.cpp src/entropy_module.cpp src/write_module.cpp func/huffman_encoder_decoder.cpp func/huffmanIO.cpp
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(PROGRAM)

# Compile each source file into object file
$(OBJS): %.o: %.cpp
	$(CC) $(INCLUDES) -c $< -o $@

# Link the object files to create the program
$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) $(LIBRARIES) $(RPATH) -o $(PROGRAM)

# Clean
clean:
	@rm -f $(OBJS) $(PROGRAM) *.cpp~ *.h~
