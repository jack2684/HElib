CC = g++
CFLAGS = -g -O2 -Wfatal-errors -Wshadow -Wall -I/usr/local/include
GMP = -lgmp
HEdir = /home/jack/Documents/homework/cs55/HElib/src
HElib = $(HEdir)/fhe.a

LDLIBS = -L/usr/local/lib $(HElib) -lntl $(GMP) -lm
INCDIR = -I$(HEdir)

HEcalc: HEcalc.cpp
	$(CC) $(CFLAGS) -o $@ $(INCDIR) $< $(LDLIBS)

clean:
	rm -f HEcalc
