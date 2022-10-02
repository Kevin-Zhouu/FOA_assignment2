rm ./program
gcc -Wall -o program program.c
./program < test2.txt
git add .
git commit -m "run"