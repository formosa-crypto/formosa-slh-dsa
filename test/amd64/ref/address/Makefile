# -*- Makefile -*-

# ------------------------------------------------------------------------------

AS       ?= as
CC       ?= clang
CFLAGS   ?= -O3 -Wall -Wextra -Wpedantic -Werror -std=c99 \
	          -Wundef -Wshadow -Wcast-align -Wpointer-arith -Wmissing-prototypes \
	          -fstrict-aliasing -fno-common -pipe -g

JASMIN       ?= jasminc
JFLAGS       ?= ${JADDFLAGS}
JINCLUDE     ?= -I Sphincs:../../../../src/amd64/common/

# ------------------------------------------------------------------------------

HASH_LIST     := shake # TODO: Add sha2 variant
PARAMS_LIST   := 128f 128s 192f 192s 256f 256s
THASH_LIST    := simple 

GET_HASH  = $(word 1, $(subst _, ,$*))
GET_PARAM = $(word 2, $(subst _, ,$*))
GET_THASH = $(word 3, $(subst _, ,$*))

TLIST     := $(foreach H,$(HASH_LIST), $(foreach P,$(PARAMS_LIST), $(foreach T,$(THASH_LIST), $(H)_$(P)_$(T))))
TESTS     := $(addprefix bin/test_address_, $(TLIST))
OUT       := $(addsuffix .out, $(TESTS))
ASM       := $(addsuffix .s, $(TESTS))

default: $(TESTS)
all: $(TESTS) $(OUT)
asm: $(ASM)
run: $(OUT)

# ------------------------------------------------------------------------------

SOURCES = ../../../../submodules/sphincsplus/ref/address.c ../../../../submodules/sphincsplus/ref/utils.c \
			../../../../submodules/sphincsplus/ref/thash_$(GET_HASH)_$(GET_THASH).c \
			../../../../submodules/sphincsplus/ref/fips202.c \
			../../../common/notrandombytes.c ../../../common/test_commons.c 

.PRECIOUS: bin/test_address_%.jpp
bin/test_address_%.jpp: | bin/
	cp test_address.jazz $@
	sed -i "1 i\from Sphincs require \"params/params-sphincs-$(GET_HASH)-$(GET_PARAM).jinc\"" $@
	$(JPP) $(JINCLUDE) -in $@ -out $@

.PRECIOUS: bin/test_address_%.s
bin/test_address_%.s: test.jazz | bin/
	cp $< tmp_$*.jazz
	sed -i "1 i\from Sphincs require \"params/params-sphincs-$(GET_HASH)-$(GET_PARAM).jinc\"" tmp_$*.jazz
	$(JASMIN) $(JFLAGS) $(JINCLUDE) tmp_$*.jazz -o $@
	-$(RM) tmp_$*.jazz


$(TESTS):
bin/test_address_%: test.c bin/test_address_%.s | bin/
	$(CC) $(CFLAGS) -o $@ -DPARAMS=sphincs-$(GET_HASH)-$(GET_PARAM) $(TEST_FLAGS) \
	-I../../../common -I../../../../submodules/sphincsplus/ref/ \
	-I../../../../submodules/sphincsplus/ref/params/ $^ $(SOURCES)

bin/test_address_%.out: bin/test_address_%
	@./$<

# ------------------------------------------------------------------------------

bin/:
	mkdir -p $@

.PHONY: clean
clean:
	-$(RM) -r bin/ tmp_*