#!/bin/bash
gcc API_TestDriver.c -o test;
./test s1.fas English_alphabet.txt > Output.txt;
./test s2.fas English_alphabet.txt >> Output.txt;
./test colorblind_human_gene.fasta DNA_alphabet.txt >> Output.txt;
./test colorblind_mouse_gene.fasta DNA_alphabet.txt >> Output.txt;
./test Human-BRCA2-cds.fasta DNA_alphabet.txt >> Output.txt;
./test Slyco.fas DNA_alphabet.txt >> Output.txt;
./test chr12.fas DNA_alphabet.txt >> Output.txt
