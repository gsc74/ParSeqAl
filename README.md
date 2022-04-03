# ParSeqAl
## Sequence Alignment with Parallel Prefix Sums
Parallel implementation of sequence alignment with affine gap costs.


### Compiliation
```
git clone https://github.com/gsc74/ParSeqAl.git
cd ParSeqAl
make clean
make
```

### Running
```
export OMP_NUM_THREADS=8
./bin/ParSeqAl data/first data/second > log/log.txt 
```

## Results
### Execution Time 
![Alt text](ExecTime.png?raw=true "Execution Time (s)")
>                                       Figure(1): Execution Time (s) v/s Threads.

### Speedup
![Alt text](SpeedUp.png?raw=true "Speed Up")
>                                       Figure(2): Speedup v/s Threads.

## Reference
<b id="my_anchor">[1].</b> Parallel biological sequence comparison using prefix computations., [https://doi.org/10.1016/S0743-7315(03)00010-8](https://doi.org/10.1016/S0743-7315(03)00010-8)
