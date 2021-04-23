if [ "$#" -ne 1 ]
    then
    echo "serve esattamente un singolo parametro"
    exit 1
fi

if [[ ${1:0:1} != "/" ]]
    then 
        echo "non è un path assoluto"
        exit 1
fi