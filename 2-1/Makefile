CC = gcc
CFLAGS = -Wall -O3 -fopenmp -I./include
LDFLAGS = -fopenmp

SRCDIR = src
BUILDDIR = build
BINDIR = bin
TESTDIR = test

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
TARGET = $(BINDIR)/game_of_life

.PHONY: all clean clean-test clean-all directories

all: directories $(TARGET)

directories:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BINDIR)
	@mkdir -p $(TESTDIR)/fig
	@mkdir -p $(TESTDIR)/results

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(BINDIR)

clean-test:
	rm -rf $(TESTDIR)/results/* $(TESTDIR)/fig/*

clean-all: clean clean-test
