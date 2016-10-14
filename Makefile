SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

CC = avr-gcc
CFLAGS = -x c -funsigned-char -funsigned-bitfields -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -Werror -c -std=gnu99
MCU = -mmcu=atxmega32a4u
AR = avr-ar

TARGET = libTWI.a

%.o: %.c
	$(CC) $(CFLAGS) $(MCU) -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -o "$@" "$<"

$(TARGET): $(OBJS)
	$(AR) -rcs -o"$@" $(OBJS)

.PHONY: clean
clean:
	rm -f *.o *.a *.d
