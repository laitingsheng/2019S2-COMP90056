define compile_test
	mkdir -p test
	g++ -std=c++17 -O3 -Wall -Wextra -Wpedantic $(3) -o $(1) $(2)
endef

test/k_hash_test: k_hash_test.cpp k_hash.hpp
	$(call compile_test,$@,$^)

part1: part1.tex
	pdflatex -jobname=$@ $^

report: report.tex
	pdflatex -jobname=$@ $^

cms.zip: $(SRC) Makefile
	zip -u9 $@ $^
