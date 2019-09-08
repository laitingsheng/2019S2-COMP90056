main: main.cpp cms.hpp hash.hpp morris.hpp
	g++ -std=c++17 -fopenmp -O3 -Wall -Wpedantic -Wno-reorder -Werror -o $@ $^

part1: part1/part1.tex
	pdflatex -output-directory=$@ -jobname=$@ $^

report: report.tex
	pdflatex -output-directory=$@ -jobname=$@ $^

cms.zip: main.cpp cms.hpp hash.hpp morris.hpp
	zip -u9 $@ $^ Makefile

.PHONY: clean

clean:
	rm -rf main
