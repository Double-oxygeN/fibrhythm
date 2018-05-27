.PHONY: compile
compile: fibrhythm.nim rhythm_maker.c
	nim c -d:release --hints:off fibrhythm.nim
	gcc -O2 -Wall -lsndfile -o rhythm_maker rhythm_maker.c

.PHONY: rhythms
rhythms: compile
	./fibrhythm 65536 1 1 > fib_1_1.txt
	./fibrhythm 65536 1 2 > fib_1_2.txt
	./fibrhythm 65536 2 3 > fib_2_3.txt
	./fibrhythm 65536 3 5 > fib_3_5.txt
	./fibrhythm 65536 5 8 > fib_5_8.txt
	./fibrhythm 65536 8 13 > fib_8_13.txt
	./fibrhythm 65536 13 21 > fib_13_21.txt
	./fibrhythm 65536 21 34 > fib_21_34.txt
	./fibrhythm 65536 34 55 > fib_34_55.txt

nanika.wav: compile rhythms
	./rhythm_maker 374 fib_1_1.txt 0 fib_1_2.txt 2 fib_2_3.txt 5 fib_3_5.txt 10 fib_5_8.txt 18 fib_8_13.txt 31 fib_13_21.txt 52 fib_21_34.txt 86 fib_34_55.txt 141

.PHONY: clean
clean:
	rm -f fib_*_*.txt fibrhythm rhythm_maker *.wav
	rm -rf nimcache
