CC	:= gcc
CCFLAGS := -Wall -Werror -g -O0 -ansi -pedantic -D_POSIX_C_SOURCE=199309L

INC	:= -I../../src/api -I../../src/imaging `pkg-config --cflags opencv`
LDFLAGS := -g -Wall -ltinkervision -lstdc++ -lm -lv4l2 `pkg-config --libs opencv`

OBJ	:= tfv_motiondetect.o
OUT	:= tfv-motiondetect

all: test

test: $(OUT)

%.o: %.c
	$(CC) $(CCFLAGS) $(INC) -c $<

$(OUT): $(OBJ)
	$(CC) $(CCFLAGS) $(INC) $(OBJ) -o $(OUT) $(LDFLAGS)

clean:
	@rm -f $(OBJ) $(OUT)
