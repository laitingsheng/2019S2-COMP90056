BIN_DIR = bin
TEST_DIR = test

define compile_exe
	mkdir -p $(1)
	g++ -std=c++17 -O3 -Wall -Wextra -Wpedantic -Wno-reorder -Werror -o $(2) $(3) $(4)
endef

define execute_test
	$1 --logger=HRF,test_suite --color_output=true --report_format=HRF
endef

all: $(BIN_DIR)/a $(BIN_DIR)/b

$(BIN_DIR)/a: a_main.cpp prime.hpp hash.hpp sampler.hpp
	$(call compile_exe,$(BIN_DIR),$@,$^,-lboost_program_options)

$(BIN_DIR)/b: b_main.cpp prime.hpp hash.hpp recovery.h stream.hpp
	$(call compile_exe,$(BIN_DIR),$@,$^,-lboost_program_options)

$(BIN_DIR)/c: c_main.cpp prime.hpp hash.hpp recovery.h stream.hpp
	$(call compile_exe,$(BIN_DIR),$@,$^,-lboost_program_options)

$(BIN_DIR)/d: d_main.cpp prime.hpp hash.hpp sampler.hpp recovery.h stream.hpp
	$(call compile_exe,$(BIN_DIR),$@,$^,-lboost_program_options -fopenmp)

tests: $(TEST_DIR)/hash_test $(TEST_DIR)/stream_test

$(TEST_DIR)/hash_test: hash_test.cpp hash.hpp prime.hpp
	$(call compile_exe,$(TEST_DIR),$@,$^)
	$(call execute_test,$@)

$(TEST_DIR)/stream_test: stream_test.cpp stream.hpp
	$(call compile_exe,$(TEST_DIR),$@,$^)
	$(call execute_test,$@)

part1: part1.tex
	pdflatex -jobname=$@ $^

report: report.tex
	pdflatex -jobname=$@ $^

assignment.zip: *.h *.hpp *.cpp Makefile
	zip -u9 $@ $^

.PHONY: clean

clean:
	rm -rf bin test
