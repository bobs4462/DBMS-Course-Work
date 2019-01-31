TEX := pdflatex
SRC := cw.tex
BIB := b.bib
BBL := cw.bbl
DOC := cw.pdf
AUX := cw.aux

$(DOC): $(SRC) $(BBL)
	$(TEX) $(SRC)

$(BBL): $(BIB) redo
	bibtex $(AUX) 

$(AUX):
	$(TEX) $(SRC)



     
