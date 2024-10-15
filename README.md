# BWTRLE

C implementation of the **Burrows-Wheeler Transform** and **Run-Length Encoding** algorithms for the compression of text/FASTA sequences. It is also possible to decompress an already compressed file of sequences.

## Download and installation

Simply:

```
git clone https://github.com/DarkAdin/BWTRLE.git
cd BWTRLE
make
```

Set ```DEBUG = 1``` in the ```Makefile``` if you wish to use a debugger.

## Use

```
./BWTRLE c/d text/FASTA_file
```

Rows starting with ```>``` are ignored in the anaylsis. The program respects the placement of each name, and just outputs the compressed/decompressed sequence that belongs to it in the line below.

## Modification

* **Buffer size**: ```BUF``` holds the maximum number of characters per row to allocate in memory. Increase or decrease it to fit your needs.
* **Alphabet**: in the form of ```char *alphabet = "¡¿!$'+,.-:;?^_|[]()abcdefghijklmnñopqrstuvwxyz"```. This variable is important, since it decides the lexical order to follow. Be sure to include all possible characters that the algorithm may encounter, and put them in the lexical order you desire.

## Example

Let's say we have this FASTA sequence belonging to a random DNA sequence with its name, in a file called ```DNA.fasta```:

```
>DNASEQUENCE
ttagaactttttaaaagaggcaaaggcagaggagaacaaaggaaggaggaagtaacttgtggaatgttgagaaaggtaaaaaccccttcaaataaggaagaggaacaggctatgacctaatgttagaactttttaaaagaggcaaaggcagaggagaacaaaggaaggaggaagtaacttgtggaatgttgagaaaggtaaaaaccccttcaaataaggaagaggaacaggctatgacctaatgtagaactttttaaaagaggcaaagttagaactttttaaaagaggcaaaggcagaggagaacaaaggaaggaggaagtaacttgtggaatgttgagaaaggtaaaaaccccttcaaataaggaagaggaacaggctatgacctaatgttagaactttttaaaagaggcaaaggcagaggagaacaaaggaaggaggaagtaacttgtggaatgttgagaaaggtaaaaaccccttcaaataaggaagaggaacaggctatgacctaatgtagaactttttaaaagaggcaaagttagaactttttaaaagaggcaaaggcagaggagaacaaaggaaggaggaagtaacttgtggaatgttgagaaaggtaaaaaccccttcaaataaggaagaggaacaggctatgacctaatgttagaactttttaaaagaggcaaaggcagaggagaacaaaggaaggaggaagtaacttgtggaatgttgagaaaggtaaaaaccccttcaaataaggaagaggaacaggctatgacctaatgtagaactttttaaaagaggcaaagttagaactttttaaaagaggcaaaggcagaggagaacaaaggaaggaggaagtaacttgtggaatgttgagaaaggtaaaaaccccttcaaataaggaagaggaacaggctatgacctaatgttagaactttttaaaagaggcaaaggcagaggagaacaaaggaaggaggaagtaacttgtggaatgttgagaaaggtaaaaaccccttcaaataaggaagaggaacaggctatgacctaatgtagaactttttaaaagaggcaaagttagaactttttaaaagaggcaaaggcagaggagaacaaaggaaggaggaagtaacttgtggaatgttgagaaaggtaaaaaccccttcaaataaggaagaggaacaggctatgacctaatgttagaactttttaaaagaggcaaaggcagaggagaacaaaggaaggaggaagtaacttgtggaatgttgagaaaggtaaaaaccccttcaaataaggaagaggaacaggctatgacctaatgtagaactttttaaaagaggcaaag$
```

It is **very important** to have a **termination character** such as ```$``` at the end of every sequence. **If it is absent, the compression is possible but not the decompression**.

Compressing it yields a file called ```compression_results.txt``` with the same structure as ```DNA.fasta```, with the difference that now the sequence is **compressed**.

```
./BWTRLE c DNA.fasta
```

This is ```compression_results.txt```:

```
>DNASEQUENCE
g1t10a10t15a10c1a15c20g10c14g20a10t10g15a1g10a15t10a10g10a20g10a14t10g10a30g10a26g20t15a10c10a15g10a20g20a10g15a10c10a34t10a20g1a10g14t10g10a20c10a10c20g10c10a46g10a15g40t20g10a10g10a25g35a40t10a65g10a10t20a4t10g10t15g10a10c10g1t10g4c10t10a10t10g10a5t10a15t15g9$1c10g10c10t30c15
```

The **Burrows-Wheeler Transform** for this sequence was computed, and the **Run-Length Encoding** was applied on it.

This operation took approximately ```0.019637``` seconds in my machine, compressing ```1341``` characters into ```278``` characters, a ```20.73%``` or $\frac{1}{5}$ of the original size. This will vastly depend on the internal redundancy of the original string.

This exact file, ```compression_results.txt``` can be decompressed to obtain the original sequence/s in a file called ```decompression_results.txt``` through:

```
./BWTRLE d compression_results.txt
```

There can be any number of sequences in a file, they will all be treated the same way.

## Bibliography

* [Burrows-Wheeler Transform (Wikipedia)](https://en.wikipedia.org/wiki/Burrows%E2%80%93Wheeler_transform)
* [Bernhard Haubold's bwt](http://guanine.evolbio.mpg.de/cgi-bin/bwt/bwt.cgi.pl)
