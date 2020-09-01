SRCS = main.c global.c vec.c physics.c random.c read_args.c render.c
ODIR = objs
OBJS = $(patsubst %,$(ODIR)/%,$(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)
TARGET = exe
OUTPUT = animation.gif
PROFDIR = profile

CC = clang-10
WFLAGS = -Wall -Wextra -Wwrite-strings -Wshadow
CFLAGS = -MMD -flto -march=native -ffast-math -D THREAD_COUNT=$$(grep -c ^processor /proc/cpuinfo)
LFLAGS = $(CFLAGS)
LIBS = -lm -fopenmp
OFLAGS = -O3 -DNTEST -DNDEBUG -fopenmp # -DNRENDER -DNBENCH
DFLAGS = -O2 -g3 -fno-omit-frame-pointer -DNTEST
DLFLAGS = $(DFLAGS) -fno-pic -no-pie

TSAN = -fsanitize=thread -fsanitize=undefined
ASAN = -fsanitize=address -fsanitize=leak -fsanitize=undefined
MSAN = -fsanitize=memory -fsanitize=undefined


-include $(DEPS)

-include $(TARGET).d

# Compile
$(ODIR)/%.o: %.c
	$(CC) $(WFLAGS) $(CFLAGS) $(OFLAGS) -c $< -o $@

#Link
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS) $(OFLAGS)

$(OUTPUT): $(TARGET)
	$(MAKE) run


.PHONY = clean all bench debug show run run_small

clean:
	$(RM) $(TARGET) $(TARGET).d $(DEPS) $(OBJS) $(OUTPUT)

all: $(TARGET)

debug:
	$(CC) $(SRCS) $(WFLAGS) $(CFLAGS) $(DFLAGS) -o $(TARGET) $(WFLAGS) $(DLFLAGS) $(LIBS) $(CFLAGS)

run: $(TARGET)
	./$(TARGET) --n=1682 --time=10.0 --box-radius=29.0 --avg-speed=1.0 --ups=1000.0 --fps=50.0 --resolution=480 --output-file=$(OUTPUT)

run_small: $(TARGET)
	./$(TARGET) --n=200 --time=1.0 --box-radius=10.0 --avg-speed=1.0 --ups=200.0 --fps=24.0 --resolution=240 --output-file=$(OUTPUT)

bench: $(PROFDIR)
	$(CC) $(SRCS) $(WFLAGS) $(CFLAGS) $(DFLAGS) -o $(TARGET) $(WFLAGS) $(DLFLAGS) $(LIBS)
	valgrind --tool=callgrind --callgrind-out-file=$(PROFDIR)/callgrind.out ./$(TARGET) --n=200 --time=1.0 --box-radius=15.0 --avg-speed=1.0 --ups=1000.0 --fps=50.0 --resolution=100 --output-file=animation.gif
	callgrind_annotate --auto=yes $(PROFDIR)/callgrind.out > $(PROFDIR)/callgrind.latest.log
	code $(PROFDIR)/callgrind.latest.log

show: $(OUTPUT)
	code $(OUTPUT)
