 #!/bin/bash

 for N in {1..30}
 do
    $1 -e bash runClient.bash &
 done