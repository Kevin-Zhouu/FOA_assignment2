rm ./program
gcc -Wall -o program program.c
./program < test1.txt
git add .
git commit -m "run"