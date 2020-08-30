CC = clang-10
WFLAGS = -Wall -Wextra -Wwrite-strings -Wshadow
CFLAGS = -flto -march=native -ffast-math
LFLAGS = $(CFLAGS)
LIBS = -lm -fopenmp
OFLAGS = -O3 -DNTEST -DNDEBUG -fopenmp # -DNRENDER -DNBENCH
DFLAGS = -O1 -g3 -fno-omit-frame-pointer
DLFLAGS = $(DFLAGS) -no-pie

TSAN = -fsanitize=thread -fsanitize=undefined
ASAN = -fsanitize=address -fsanitize=leak -fsanitize=undefined
MSAN = -fsanitize=memory -fsanitize=undefined

SRCS = main.c global.c vec.c physics.c random.c read_args.c render.c
ODIR = objs
OBJS = $(patsubst %,$(ODIR)/%,$(SRCS:.c=.o))
TARGET = exe
OUTPUT = animation.gif


%.c: *.h
	touch $@

# Compile
$(ODIR)/%.o: %.c
	$(CC) $(WFLAGS) $(CFLAGS) $(OFLAGS) -c $^ -o $@

#Link
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS) $(OFLAGS)

$(OUTPUT): $(TARGET) makefile
	touch $@


.PHONY = clean all debug show run run_small

clean:
	$(RM) $(TARGET) $(OBJS) $(OUTPUT)

all: $(TARGET)

debug: clean
	$(CC) $(SRCS) $(WFLAGS) $(DFLAGS) -o $(TARGET) $(DLFLAGS) $(LIBS)

show: $(OUTPUT)
	code $(OUTPUT)

run: $(TARGET)
	./$(TARGET) --n=1682 --time=10.0 --box-radius=20.0 --avg-speed=32.0 --ups=1000.0 --fps=50.0 --resolution=480 --output-file=$(OUTPUT)

run_small: $(TARGET)
	./$(TARGET) --n=400 --time=1.0 --box-radius=10.0 --avg-speed=32.0 --ups=100.0 --fps=24.0 --resolution=240 --output-file=$(OUTPUT)
