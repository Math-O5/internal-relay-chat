 #!/bin/bash

 for N in {1..10}
 do
    $1 -e bash runClient.bash &
 done