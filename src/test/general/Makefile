CC	:= gcc
CCFLAGS := -Wall -Werror -g -O0 -ansi -pedantic -D_POSIX_C_SOURCE=199309L

INC	:= `pkg-config --cflags opencv`
LDFLAGS := -g -Wall -ltinkervision -lstdc++ -lm -lv4l2 `pkg-config --libs opencv`

OBJ	:= tfv_test_api.o
OUT	:= tfv-test-api
OBJ1 := tfv_test_framesize.o
OUT1 := tfv-test_framesize


all: test test1

test: $(OUT)

test1: $(OUT1)

%.o: %.c
	$(CC) $(CCFLAGS) $(INC) -c $<

%.o: %.cc
	$(CC) $(CCFLAGS) $(INC) -c $<

$(OUT): $(OBJ)
	$(CC) $(CCFLAGS) $(INC) $(OBJ) -o $(OUT) $(LDFLAGS)

$(OUT1): $(OBJ1)
	$(CC) $(CCFLAGS) $(INC) $(OBJ1) -o $(OUT1) $(LDFLAGS)

clean:
	@rm -f $(OBJ) $(OUT) $(OBJ1) $(OUT1)
