out/main: main.cpp cms.hpp hash.hpp morris.hpp
	mkdir -p out
	g++ -std=c++17 -fopenmp -O3 -Wall -Wpedantic -Wno-reorder -Werror -o $@ $^

part1: part1.tex
	mkdir -p out
	pdflatex -output-directory=out -jobname=$@ $^

report: report.tex
	mkdir -p out
	pdflatex -jobname=$@ $^

cms.zip: main.cpp cms.hpp hash.hpp morris.hpp
	zip -u9 $@ $^ Makefile

.PHONY: clean

clean:
	rm -rf out
