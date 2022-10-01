rm ./score_snippet
gcc -Wall -o score_snippet score_snippet.c
./score_snippet alice feet< data1.txt
git add .
git commit -m "run"