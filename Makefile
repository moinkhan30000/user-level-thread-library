CC	:= gcc
CFLAGS := -g -Wall

TARGETS :=  libtsl.a app
# Make sure that 'all' is the first target
all: $(TARGETS)

TSL_SRC :=  tsl.c
TSL_OBJS := $(TSL_SRC:.c=.o)

libtsl.a: $(TSL_OBJS)
	ar rcs $@ $(TSL_OBJS)

TSL_LIB :=  -L . -l tsl





tsl.o: tsl.c tsl.h
	gcc -c $(CFLAGS)  -o $@ tsl.c


app.o: app.c  tsl.h
	gcc -c $(CFLAGS)  -o $@ app.c

app: app.o libtsl.a
	gcc $(CFLAGS) -o $@ app.o  $(TSL_LIB)


clean:
	rm -rf core  *.o *.out *~ $(TARGETS)