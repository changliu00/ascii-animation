CC = g++
CFLAGS = -O3 -std=c++11

%.o: %.c ascii-anim.h
	$(CC) -c $< -o $@ $(CFLAGS)

bunnybirth17: ascii-anim.o bunnybirth17.o
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY: clean
clean:
	rm *.o
