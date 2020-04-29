 #!/bin/bash

 for N in {1..50}
 do
    ./client host=127.0.0.1 PORT=9003
 done
 wait