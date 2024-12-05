ifeq ($(OS),Windows_NT)
    BIN_SUFFIX := .exe
else
    BIN_SUFFIX := .out
endif

BLANKSPACE := $(addsuffix $(BIN_SUFFIX),../blankspace)
TESTS := $(basename $(sort $(wildcard *.bs)))
INPUTS_DIR := inputs
EXPECTS_DIR := expects
TRANSPILED_DIR := transpiled
MKDIR := mkdir
ECHO := echo
DIFF := diff -Z --strip-trailing-cr
RM := rm -f
CC := gcc
CFLAGS := -pipe -O2 -Wno-unused-result


define generate-interpreter-test
$1:
	@$(ECHO) -n "Interpreter test: $2.bs ... "
	@([ -f $(INPUTS_DIR)/$2.txt ] \
		&& $(BLANKSPACE) $2.bs < $(INPUTS_DIR)/$2.txt || $(BLANKSPACE) $2.bs) \
		| $(DIFF) - $(EXPECTS_DIR)/$2.txt > /dev/null
	@$(ECHO) 'Success'
endef

define generate-transpiler-test
$1: $(TRANSPILED_DIR)/$2$(BIN_SUFFIX)
	@$(ECHO) -n "Transpiler test: $2.bs ... "
	@([ -f $(INPUTS_DIR)/$2.txt ] \
		&& $$< < $(INPUTS_DIR)/$2.txt || $$<) \
		| $(DIFF) - $(EXPECTS_DIR)/$2.txt
	@$(ECHO) 'Success'

$(TRANSPILED_DIR)/$2$(BIN_SUFFIX): $2.bs .FORCE
	@[ ! -d $$(@D) ] && $(MKDIR) $$(@D) || :
	@$(BLANKSPACE) $$< -t | $(CC) $(CFLAGS) -xc - -o $$@ > /dev/null
endef


.PHONY: all interpreter binary clean $(TESTS)

.FORCE:

all: interpreter binary

interpreter: $(foreach TEST,$(TESTS),interpreter_$(TEST))

$(foreach TEST,$(TESTS),$(eval $(call generate-interpreter-test,interpreter_$(TEST),$(TEST))))

binary: $(foreach TEST,$(TESTS),transpiler_$(TEST))

$(foreach TEST,$(TESTS),$(eval $(call generate-transpiler-test,transpiler_$(TEST),$(TEST))))

clean:
	$(RM) $(TRANSPILED_DIR)/*.exe
