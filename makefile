CC = clang-10
WFLAGS = -Wall -Wextra -Wwrite-strings -Wshadow
CFLAGS = -flto -march=native -ffast-math -fopenmp
LFLAGS = -flto -march=native -ffast-math -fopenmp
LIBS = -lm -fopenmp
OFLAGS = -O3 -DNTEST -DNDEBUG
DFLAGS = -O1 -g3 -fno-omit-frame-pointer# -fPIE -pie

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

$(ODIR)/%.o: %.c
	$(CC) $(CFLAGS) $(WFLAGS) $(DFLAGS) -c $^ -o $@

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS)

$(OUTPUT): $(TARGET) makefile
	touch $@


.PHONY = all run clean

all: $(TARGET)

run: $(TARGET)
	./$(TARGET) --n=1682 --time=1.0 --box-radius=20.0 --avg-speed=32.0 --ups=400.0 --fps=50.0 --resolution=500 --output-file=$(OUTPUT)

clean:
	$(RM) $(TARGET) $(OBJS) $(OUTPUT)

show: $(OUTPUT)
	code $(OUTPUT)