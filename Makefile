export CC		:=	g++

export CFLAGS	:=	-O2 -Wall -Werror -Wextra
export CXXFLAGS	:=	$(CFLAGS) -std=c++17

export LIBS		:=	-lfmt

all: compile extract run diff

compile:
	$(CC) -o tmc_strings main.cpp $(CXXFLAGS) $(LIBS)

run:
	./tmc_strings

extract:
	dd if=us.gba bs=1 skip=10165648 count=301536 status=none | hexdump > base_us.hex
	dd if=eu.gba bs=1 skip=10152800 count=297152 status=none | hexdump > base_en.hex
	dd if=eu.gba bs=1 skip=10449952 count=293520 status=none | hexdump > base_fr.hex
	dd if=eu.gba bs=1 skip=10743472 count=274368 status=none | hexdump > base_de.hex
	dd if=eu.gba bs=1 skip=11017840 count=268592 status=none | hexdump > base_es.hex
	dd if=eu.gba bs=1 skip=11286432 count=276704 status=none | hexdump > base_it.hex

diff:
	hexdump USA.bin | diff base_us.hex -
	hexdump English.bin | diff base_en.hex -
	hexdump French.bin | diff base_fr.hex -
	hexdump German.bin | diff base_de.hex -
	hexdump Spanish.bin | diff base_es.hex -
	hexdump Italian.bin | diff base_it.hex -
