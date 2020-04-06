# path-finding
gtk path finding app

## requirements
gcc <br />
gtk3 <br />

## installation
do `$ make` !

## documentation
executable `./main` for doing path find on a maze created by executable `./designer`. explore each command for further details by running the following: <br />
run `$ ./main --help` <br />
run `$ ./designer --help` <br />

## desingner app (./desginer) examples
`$ ./desginer 10 20 map.bin random` runs designer gui with a new map of width 10 and height 20 with random drawing (you also customly draw in that window) and saving to `map.bin` <br />
`$ ./desginer 10 20 map.bin custom` is equal to above with exception that the drawing is done custom from start <br />
`$ ./desginer map.bin` just does the edit on an existing map <br />

## path finder app (./main) examples
`./main -i map.bin -D -t 10 run` does DFS on input file `map.bin` with 10 ms delay in between each step for more clear visualization <br />
`./main -i map.bin -D -t 10 run` is equal to above with exception that the algorithm is BFS <br />
`./main -i map.bin -D -t 10 run -p` automate the things and never shows a gui. this is the case for later time measurement <br />

## do_for_30_times.sh
`$ sh ./do_for_n_times.sh 10 bfs s.bin outfile.txt` runs bfs for 10 times on map `s.bin` and saves the each time measurment in `outfile.txt` <br />
