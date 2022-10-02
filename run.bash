rm ./score_snippet
gcc -Wall -o program program.c
./program < test0.txt
git add .
git commit -m "run"