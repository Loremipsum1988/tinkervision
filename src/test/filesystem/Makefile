CC	:= g++
CCFLAGS := -Wall -Werror -g -std=c++11 -O0 -pedantic

INC	:= -I../../lib/core -I../../lib/tools
LDFLAGS := -g -Wall -lstdc++ -lpthread

OBJ	:= tfv_test_fs.o
OUT	:= tfv-test-fs

all: test

test: $(OUT)

%.o: %.c
	$(CC) $(CCFLAGS) $(INC) -c $<

%.o: %.cc
	$(CC) $(CCFLAGS) $(INC) -c $<

$(OUT): $(OBJ)
	$(CC) $(CCFLAGS) $(INC) ../../lib/tools/filesystem.cc $(OBJ) -o $(OUT) $(LDFLAGS)

clean:
	@rm -f $(OBJ) $(OUT)
