 #!/bin/bash
 # time ./manyClientstest.bash
 
 for N in {1..50}
 do
    g++ -ipthread client.cpp host=127.0.0.1 PORT=9003
 done
 wait
