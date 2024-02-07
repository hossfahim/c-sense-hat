CC		= gcc
LIBS	= -li2c -lpthread
WARN	= -Wall
#################################################

all: led_matrix sock

# pressure.o:	pressure.c pressure.h
# 	$(CC) -c $(WARN) $? -o $@

# humidity.o:	humidity.c humidity.h
# 	$(CC) -c $(WARN) $? -o $@

pressure: pressure.o
	$(CC) $(WARN) $? -o $@ $(LIBS)

humidity: humidity.o
	$(CC) $(WARN) $? -o $@ $(LIBS)

SenseHat/senseHat: SenseHat/senseHat.o
	$(CC) $(WARN) $? -o $@ $(LIBS)
# SenseHat/senseHat.o: SenseHat/senseHat.c
# 	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

led_matrix:	led_matrix.o
	$(CC) $(WARN) $? -o $@ $(LIBS)

sock: sock.o humidity.o pressure.o SenseHat/senseHat.o
	$(CC) $(WARN) $? -o $@ $(LIBS)

user: user.o SenseHat/senseHat.o
	$(CC) $(WARN) $? -o $@ $(LIBS)

%.o:	%.c 
	$(CC) -c $(WARN) $? -o $@

clean:
	rm -f *.o sock led_matrix *.c~ SenseHat/*o SenseHat/joystick_test SenseHat/senseHat_test
