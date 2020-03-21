> out.csv
gcc -fopenmp openmp_pi.c -o openmp_pi
for ((i=1; i <= 16; i++))
do
  TIME=$(/usr/bin/time -f "%e" ./openmp_pi "$i" 2>&1)
  echo "$i, $TIME" >> out.csv
done
cat out.csv
python3 plot.py
