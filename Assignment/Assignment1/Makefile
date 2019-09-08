main: main.cpp cms.hpp hash.hpp morris.hpp
	g++ -std=c++17 -fopenmp -O3 -Wall -Wpedantic -Wno-reorder -Werror -o $@ $^

part1: part1.tex
	pdflatex -jobname=$@ $^

report: report.tex
	pdflatex -jobname=$@ $^

cms.zip:
	zip -u9 $@ *.hpp *.cpp Makefile

.PHONY: clean submission

clean:
	rm -f main report.aux report.log report.out report.pdf submission.zip
