BULDDIR = build
SRCDIR = src

build/k-means : src/pca.c src/em.c src/em.h src/nn.c src/nn.h src/io.c src/io.h src/tools.c src/tools.h
	gcc -o build/k-means -L/usr/local/lib -lgsl -lgslcblas src/pca.c src/em.c src/nn.c src/io.c src/tools.c
