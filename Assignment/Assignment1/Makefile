main: main.cpp cms.hpp hash.hpp morris.hpp
	g++ -std=c++17 -fopenmp -O3 -Wall -Wpedantic -Wno-reorder -Werror -o $@ $^

report: report.tex
	pdflatex -jobname=$@ $^

.PHONY: clean

clean:
	rm -f main
