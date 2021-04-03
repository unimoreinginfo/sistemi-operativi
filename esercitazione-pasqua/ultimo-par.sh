#!/bin/bash

echo Ultimo parametro: ${@: -1:1}
echo Tutti i parametri meno l\'ultimo: ${@:1:$#-1} 