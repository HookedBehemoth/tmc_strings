export CC		:=	g++

export CFLAGS	:=	-O2 -Wall -Werror -Wextra
export CXXFLAGS	:=	$(CFLAGS) -std=c++17

export LIBS		:=	-lfmt

export DIFF		:= diff
export HEXDUMP	:= hexdump -C

all: compile dump run

compile:
	$(CC) -o tmc_strings main.cpp $(CXXFLAGS) $(LIBS)

run: extract pack

extract:
	./tmc_strings -x --source us.gba --region USA
	./tmc_strings -x --source eu.gba --region EU

pack:
	./tmc_strings -p --source USA.json --dest USA.bin --size 0x499E0
	./tmc_strings -p --source English.json --dest English.bin --size 0x488C0
	./tmc_strings -p --source French.json --dest French.bin --size 0x47A90
	./tmc_strings -p --source German.json --dest German.bin --size 0x42FC0
	./tmc_strings -p --source Spanish.json --dest Spanish.bin --size 0x41930
	./tmc_strings -p --source Italian.json --dest Italian.bin --size 0x438E0

dump:
	dd if=us.gba bs=1 skip=10165648 count=301536 status=none | $(HEXDUMP) > base_us.hex
	dd if=eu.gba bs=1 skip=10152800 count=297152 status=none | $(HEXDUMP) > base_en.hex
	dd if=eu.gba bs=1 skip=10449952 count=293520 status=none | $(HEXDUMP) > base_fr.hex
	dd if=eu.gba bs=1 skip=10743472 count=274368 status=none | $(HEXDUMP) > base_de.hex
	dd if=eu.gba bs=1 skip=11017840 count=268592 status=none | $(HEXDUMP) > base_es.hex
	dd if=eu.gba bs=1 skip=11286432 count=276704 status=none | $(HEXDUMP) > base_it.hex

diff:
	@$(HEXDUMP) USA.bin | $(DIFF) base_us.hex -
	@$(HEXDUMP) English.bin | $(DIFF) base_en.hex -
	@$(HEXDUMP) French.bin | $(DIFF) base_fr.hex -
	@$(HEXDUMP) German.bin | $(DIFF) base_de.hex -
	@$(HEXDUMP) Spanish.bin | $(DIFF) base_es.hex -
	@$(HEXDUMP) Italian.bin | $(DIFF) base_it.hex -
