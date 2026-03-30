# debpkg Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS =

TARGET = debpkg

# 目录配置
SRCDIR = src
INCDIR = include
OBJDIR = obj

# 源文件和目标文件
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)

install: $(TARGET)
	@echo "Installing debpkg to /usr/local/bin..."
	sudo cp $(TARGET) /usr/local/bin/
	sudo chmod +x /usr/local/bin/$(TARGET)
	@echo "Installation complete!"

uninstall:
	@echo "Removing debpkg from /usr/local/bin..."
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "Uninstallation complete!"

help:
	@echo "Available targets:"
	@echo "  all       - Build the debpkg tool (default)"
	@echo "  clean     - Remove built files"
	@echo "  install   - Install debpkg to /usr/local/bin"
	@echo "  uninstall - Remove debpkg from /usr/local/bin"
	@echo "  help      - Show this help message"
