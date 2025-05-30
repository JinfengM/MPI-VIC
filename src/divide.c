#include <stdio.h>
#include <stdlib.h>

int* divide_into_parts(int ntotal, int npart) {
    int* parts = (int*) malloc(npart * sizeof(int));
    int quotient = ntotal / npart;
    int remainder = ntotal % npart;

    for (int i = 0; i < npart; i++) {
        parts[i] = quotient;
        if (remainder > 0) {
            parts[i]++;
            remainder--;
        }
    }

    return parts;
}

int main() {
    int ntotal = 10;
    int npart = 3;
    int* parts = divide_into_parts(ntotal, npart);

    printf("Dividing %d into %d parts:\n", ntotal, npart);
    for (int i = 0; i < npart; i++) {
        printf("Part %d: %d\n", i + 1, parts[i]);
    }

    free(parts);
    return 0;
}
