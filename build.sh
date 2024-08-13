
project="qwe"

cterm_path="external/cterm"
cstring_path="external/cstring"
cvecs_path="external/cvecs"

includes="-I./ -I${cterm_path} -I${cstring_path} -I${cvecs_path}"
cflags="-Wextra -Wall -pedantic -g -O3"

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
        main.c qwe.c ${cterm_path}/cterm.c ${cstring_path}/cstring.c ${cvecs_path}/cvecs.c \
        "
###################################
    error_code=$?
    echo -n "Compiling "
    if [ $error_code -eq 0 ]; then
        green_echo "finished" 
    else
        red_echo "failed" 
    fi
