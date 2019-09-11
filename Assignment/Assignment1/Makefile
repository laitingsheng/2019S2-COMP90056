SRC = main.cpp cms.hpp hash.hpp morris.hpp utils.hpp

main: $(SRC)
	g++ -std=c++17 -O3 -Wall -Wpedantic -Wno-reorder -Werror -o $@ $^

part1: part1.tex
	pdflatex -jobname=$@ $^

report: report.tex
	pdflatex -jobname=$@ $^

cms.zip: $(SRC)
	zip -u9 $@ $^ Makefile
