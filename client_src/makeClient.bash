 #!/bin/bash

 for N in {1..3}
 do
    if [ "$1" == "konsole" ]; then
      konsole -e make run &
      konsole --new-tab -e make run 1! &
    elif [ "$1" == "gnome-terminal" ]; then
      gnome-terminal -- /bin/sh -c 'make run' &
    elif [ "$1" == "xterm"]; then
      xterm -hold -e "make run" &
    elif [ $1 == '' ]; then
      gnome-terminal -- /bin/sh -c 'make run' &
    else
      $1 -e make run &
    fi
 done