CC=avr32-gcc
MPART=-mpart=uc128l4u
CFLAGS=-g -Wall -std=gnu99
OBJS=main.o clock.o
TARGET=godwit.elf

# Link
$(TARGET): $(OBJS)
	$(CC) $(MPART) $(OBJS) -o $(TARGET)

# Dependency information for *existing* .o files
-include $(OBJS:.o=.d)

# Compile
%.o: %.c
	$(CC) $(MPART) -c $(CFLAGS) $*.c -o $*.o
	@$(CC) $(MPART) -MM $(CFLAGS) $*.c > $*.d

# Clean up
clean:
	rm -f $(TARGET) *.o *.d
