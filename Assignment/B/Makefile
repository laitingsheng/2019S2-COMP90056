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

all: $(BIN_DIR)/a

$(BIN_DIR)/a: a_main.cpp l.hpp k_hash.hpp
	$(call compile_exe,$(BIN_DIR),$@,$^,-lboost_program_options -fopenmp)

tests: $(TEST_DIR)/k_hash_test
	$(call execute_tests,$^)

$(TEST_DIR)/k_hash_test: k_hash_test.cpp k_hash.hpp
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
