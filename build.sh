
project="qwe"
includes="-Iexternal/cterm/"
cflags="-Wextra -Wall -pedantic -g"

cterm_path="external/cterm"

execute() {
    inp=$(echo -e "$1" | sed 's/\\\n//g')
    inp=$(echo "$inp" | tr -s '[:space:]' ' ')
    echo "$inp"
    eval "$inp"
}

red_echo() {
    local text=$1
    echo -e "\e[31m$text\e[0m"
}

green_echo() {
    local text=$1
    echo -e "\e[32m$text\e[0m"
}

yellow_echo() {
    local text=$1
    echo -e "\e[33m$text\e[0m"
}

########## Compiling Binary ##########
    echo -n "Compiling "
    yellow_echo "$project"
######### Add Source Files #########
    execute "gcc -o $project $cflags $includes \
        main.c ${cterm_path}/cterm.c \
        "
###################################
    error_code=$?
    echo -n "Compiling "
    if [ $error_code -eq 0 ]; then
        green_echo "finished" 
    else
        red_echo "failed" 
    fi
