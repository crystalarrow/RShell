echo A && echo B
echo A || echo B
echo A; echo B
ls -j && echo A
ls -j || echo A
ls -j; echo A
cal 4 2009 && echo This is a date || echo b
echo "A || B" && echo PrintHere
echo "A && B" || echo PrintHERE
pwd fileNotExist || echo Yes && echo "Temp || "

exit