CC		:= g++

ifeq ($(DEBUG),yes)
	CCFLAGS	:= -Wall -Werror -g -O0 -std=c++11 -shared -fPIC -DDEBUG
	INC		:= `pkg-config --cflags opencv`
	LDFLAGS	:= `pkg-config --libs opencv`
else
	CCFLAGS	:= -Wall -Werror -O3 -std=c++11 -shared -fPIC
endif

ifndef PRE
	PRE = /usr/lib/tinkervision
endif

LDFLAGS	+= -L/usr/lib/python2.7 -lpython2.7
INC		+= -I/usr/include/python2.7 -I/usr/include/tinkervision

SO		:= downscale.so
TV		:= interface/module.o interface/image.o interface/parameter.o
OBJ		:= downscale.o $(TV)

all: so

%.o: %.cc
	$(CC) $(CCFLAGS) -c $< -o $@ $(INC)

so: $(OBJ)
	$(CC) -shared -o $(SO) $(OBJ) $(LDFLAGS)

.phony:
	clean

clean:
	rm -f *.o *.so interface/*.o

install: $(SO)
	install -m 544 $(SO) $(PRE)/$(SO)

.PHONY: install
