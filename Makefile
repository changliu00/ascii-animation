# <https://devhints.io/makefile>, <https://gist.github.com/isaacs/62a2d1825d04437c6f08>
# target [target2 ...]: [dependency1 dependency2 ...]
# 	[commands]
# `make` executes the first rule and anything depending on it recursively.
# `make target` executes the specified target and anything depending on it recursively.
# `$@`: the name of target.
# `$?`: names of changed dependents (newer than the target).
# `$<`: the name of the first dependent file.
# `$^`: names of all the dependents.
# `$+`: names of all the dependents, with duplication.
# `$*`: the prefix shared by the target and the dependents
# (i.e., what `%` represents in `%.o: %.cpp`. `%.o` should appear in the Makefile; appearing in the folder does not count).
# `-commands` (ignore errors); `@commands` (don't print the commands); `+commands` (run even if in "don't execute" mode).

CC = g++
CFLAGS = -O3 -std=c++11 -Wno-unused-result

%.o: %.cpp ascii-anim.h
	$(CC) -c $< -o $@ $(CFLAGS)

# bunnybirth17.out: ascii-anim.o bunnybirth17.o
# 	$(CC) $^ -o $@ $(CFLAGS)

bunnypiggy18.out: ascii-anim.o bunnypiggy18.o
	$(CC) $^ -o $@ $(CFLAGS)

bunnybirth18.out: ascii-anim.o bunnybirth18.o
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY: clean cleanall
clean:
	-rm *.o

cleanall:
	-rm *.o *.out
