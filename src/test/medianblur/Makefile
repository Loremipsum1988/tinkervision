CC	:= gcc
CCFLAGS := -Wall -g -O0 -ansi -pedantic -D_POSIX_C_SOURCE=199309L

INC	:= `pkg-config --cflags opencv`
LDFLAGS := -g -Wall -lstdc++ -ltinkervision -lm -lv4l2 \
           `pkg-config --libs opencv`

OBJ	:= tv_medianblur.o
OUT	:= tv-medianblur

all: test

test: $(OUT)

%.o: %.c
	$(CC) $(CCFLAGS) $(INC) -c $<

%.o: %.cc
	$(CC) $(CCFLAGS) $(INC) -c $<

$(OUT): $(OBJ)
	$(CC) $(CCFLAGS) $(INC) $(OBJ) -o $(OUT) $(LDFLAGS)

clean:
	@rm -f $(OBJ) $(OUT)
