make clean
make
./mi_mkfs disco 100000
./simulacion disco > dir_simul_name.txt
time ./verificacion disco "$(cat dir_simul_name.txt)"
#./mi_cat disco "$(cat dir_simul_name.txt)/informe.txt" > resultado.txt
#ls -l resultado.txt
#cat resultado.txt
#./leer_sf disco
