make clean && make all &&
sudo insmod "$MOD_NAME".ko
dmesg | tail -n 10