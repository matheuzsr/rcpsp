# Debug mode
DBG = off

# Optimization switch
OPT = on

# Include dir : -I//diretório 
INCLUDES = 

# Objects : nome.o
OBJS = 

# Libraries : -L//diretório -nomelib
LIB =  

# Programs
RCPSP_OBJ = rcpsp.o
RCPSP_EXE = rcpsp

CXX = g++

USER_FLAGS = -std=c++14

# Compiler flags for debugging
ifeq ($(OPT), off)
	USER_FLAGS += -ggdb3 -fexceptions -fno-omit-frame-pointer \
		-fno-optimize-sibling-calls -fno-inline
else
	USER_FLAGS += -fomit-frame-pointer -funroll-loops
	ifeq ($(CXX), g++)
		USER_FLAGS += -ftracer -fpeel-loops -fprefetch-loop-arrays
	endif
endif


# Enabling debug mode (printf)
ifeq ($(DBG), on)
	USER_FLAGS += -DDBG
endif


# Include parallel stuff
#USER_FLAGS += -fopenmp


# Warning flags
USER_FLAGS += -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization \
	-Wformat=2 -Winit-self -Wmissing-format-attribute -Wshadow \
	-Wpointer-arith -Wredundant-decls -Wstrict-aliasing=2 \
	-Wfloat-equal -Weffc++

ifeq ($(CXX), g++)
	USER_FLAGS += -Wunsafe-loop-optimizations
endif

CXXFLAGS = $(USER_FLAGS)

.PHONY: all p
.SUFFIXES: .cpp .o

all: rcpsp

p: rcpsp

rcpsp: $(OBJS) $(RCPSP_OBJ)
	@echo "--> Linking objects... "
	$(CXX) $(CXXFLAGS) $(OBJS) $(RCPSP_OBJ) -o $(RCPSP_EXE) $(LIB)
	@echo

.cpp.o:
	@echo "--> Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(USER_DEFINES) -c $< -o $@
	@echo

clean:
	@echo "--> Cleaning compiled..."
	rm -rf $(OBJS) $(RCPSP_OBJ) $(RCPSP_EXE)
	rm -rf *o

