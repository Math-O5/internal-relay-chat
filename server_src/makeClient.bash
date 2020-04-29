 #!/bin/bash

MOREF='Ola'

 for N in {1..50}
 do
    ./client host=127.0.0.1 PORT=9003
    echo $MOREF
 done
 wait