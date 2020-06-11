> out.txt
mpicc blur-effect1.c -o blur-effect -lm -fopenmp -Wall -pedantic
for((j=3;j<=15;j+=2))
do
	for ((i=1; i <= 4; i*=2))
		do
			for ((k=1; k <= 10; k+=1))
				do
	  			TIME="$(/usr/bin/time -f '%e' mpirun -np $i --hostfile /home/mpiuser/mpi-hosts ./blur-effect 720.jpg 720blur.jpg $j 2 2>&1 )"
	  			echo "720.jpg kernel: $j threads: $i tiempo: $TIME" >> out.txt
			done
		done
done
for((j=3;j<=15;j+=2))
do
	for ((i=1; i <= 4; i*=2))
		do
			for ((k=1; k <= 10; k+=1))
				do
	  			TIME="$(/usr/bin/time -f '%e' mpirun -np $i --hostfile /home/mpiuser/mpi-hosts ./blur-effect 1080.jpg 1080blur.jpg $j 2 2>&1 )"
	  			echo "1080.jpg kernel: $j threads: $i tiempo: $TIME" >> out.txt
				done
		done
done
for((j=3;j<=15;j+=2))
do
	for ((i=1; i <= 4; i*=2))
		do
			for ((k=1; k <= 10; k+=1))
				do
	  			TIME="$(/usr/bin/time -f '%e' mpirun -np $i --hostfile /home/mpiuser/mpi-hosts ./blur-effect 4k.jpg 4kblur.jpg $j 2 2>&1 )"
	  			echo "4k.jpg kernel: $j threads: $i tiempo: $TIME" >> out.txt
				done
		done
done
