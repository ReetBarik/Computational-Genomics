#!/bin/bash
gcc Main.c -o main;
./main Peach_reference.fasta Peach_simulated_reads.fasta DNA_alphabet.txt > Output1.txt;