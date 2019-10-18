BIN_DIR = bin
TEST_DIR = test

define compile_exe
	mkdir -p $(1)
	g++ -std=c++17 -O3 -Wall -Wextra -Wpedantic -Wno-reorder -Werror -o $(2) $(3) $(4)
endef

define execute_test
	$1 --logger=HRF,all --color_output=false --report_format=HRF
endef

define execute_tests
	$(foreach exe,$(1),$(call execute_test,$(exe)))
endef

all: $(BIN_DIR)/a $(BIN_DIR)/b

$(BIN_DIR)/a: a_main.cpp sampler.hpp hash.hpp prime.hpp
	$(call compile_exe,$(BIN_DIR),$@,$^,-lboost_program_options)

$(BIN_DIR)/b: b_main.cpp recovery.hpp prime.hpp
	$(call compile_exe,$(BIN_DIR),$@,$^,-lboost_program_options)

tests: $(TEST_DIR)/hash_test
	$(call execute_tests,$^)

$(TEST_DIR)/hash_test: hash_test.cpp hash.hpp prime.hpp
	$(call compile_exe,$(TEST_DIR),$@,$^)

part1: part1.tex
	pdflatex -jobname=$@ $^

report: report.tex
	pdflatex -jobname=$@ $^

cms.zip: $(SRC) Makefile
	zip -u9 $@ $^

.PHONY: clean

clean:
	rm -rf bin test
