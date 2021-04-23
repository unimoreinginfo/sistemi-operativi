#!/bin/bash

export a=ciao
echo $a, $(pwd), .* # punto 1)
echo "\$a", "\$(pwd)", ".*" # punto 2)
echo $a, $(pwd), ".*" # punto 3)