cat < README.md 

cat < README.md | tee new

cat < README.md  > new

cat < README.md | tee new

cat < README.md >> new

cat < README.md | tr a-z A-Z

cat < README.md | tr a-z A-Z > a

cat < README.md | tr a-z A-Z >> b

cat < README.md | tr A-Z a-z | tee newOutputFile1 | tr a-z A-Z > newOutputFile2

cat < a

cat < d | tee e

exit

