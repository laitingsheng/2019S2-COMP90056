define compile_test
	mkdir -p test
	g++ -std=c++17 -O3 -Wall -Wextra -Wpedantic -Wno-reorder -Werror $(3) -o $(1) $(2)
endef

define execute_test
	$1 --logger=HRF,all --color_output=false --report_format=HRF
endef

define execute_tests
	$(foreach exe,$(1),$(call execute_test,$(exe)))
endef

tests: test/k_hash_test
	$(call execute_tests,$^)

test/k_hash_test: k_hash_test.cpp k_hash.hpp
	$(call compile_test,$@,$^)

part1: part1.tex
	pdflatex -jobname=$@ $^

report: report.tex
	pdflatex -jobname=$@ $^

cms.zip: $(SRC) Makefile
	zip -u9 $@ $^

.PHONY: clean

clean:
	rm -rf bin test
