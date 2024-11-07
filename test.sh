#!/bin/bash

WHITE="\033[0;37m"
GREEN="\033[1;32m"
RED="\033[1;31m"

ls="/bin/ls"
ft_ls="./ft_ls"

test_dir="/home/issamounejjar/Documents/projects/ft_ls"
garbage_dir="/tmp"

options=$($ft_ls --help | grep -oE '^ *-[a-zA-Z0-9]' | sort | uniq | tr -d '\n -')

result_file="result.txt"
ls_result="$garbage_dir/ls.txt"
ft_ls_result="$garbage_dir/ft_ls.txt"

is_exception() {
  exceptions="glo"
  
  for char in $(echo $exceptions | fold -w1); do
    if [ "$char" = "$1" ]; then
      return 0
    fi
  done
  
  return 1
}

list_has_exception() {
  exceptions="glo"
  
  for o in $1; do
    for char in $(echo $exceptions | fold -w1); do
      if [ "$char" = "$o" ]; then
        return 0
      fi
    done
  done
  
  return 1
}

# echo "Testing Single Option:"
# for opt in $(echo $options | fold -w1); do
#   
#   if is_exception $opt ; then
#     unbuffer $ls -$opt $test_dir | expand -t 8 | grep -v $ls_result | grep -v $ft_ls_result > $ls_result 2>&1
#     $ft_ls -$opt $test_dir | sed 's/\x1B\[[0-9;]*[a-zA-Z]//g' | grep -v $ls_result | grep -v $ft_ls_result > $ft_ls_result 2>&1
#   else
#     unbuffer $ls -$opt $test_dir | expand -t 8 > $ls_result 2>&1
#     $ft_ls -$opt $test_dir | sed 's/\x1B\[[0-9;]*[a-zA-Z]//g' > $ft_ls_result 2>&1
#   fi
#
#   if ! diff -q $ls_result $ft_ls_result > /dev/null; then
#     echo -e "  [ $opt ] : $RED[x]$WHITE"
#   fi
# done

# echo "Testing random options:"
counter=1
options_count=$(echo $options | fold -w1 | wc -l)
while [ $counter -le 20 ]
do
  test_len=$((1 + $RANDOM % options_count))
  opt_list=""

  i=0
  while [ $i -lt $test_len ]
  do
    rand=$((0 + $RANDOM % options_count))
    opt_list+="${options:rand:1}"
    ((i++))
  done

  if list_has_exception $opt_list ; then
    unbuffer $ls -$opt_list $test_dir | expand -t 8 | grep -v $ls_result | grep -v $ft_ls_result > $ls_result 2>&1
    $ft_ls -$opt_list $test_dir | sed 's/\x1B\[[0-9;]*[a-zA-Z]//g' | grep -v $ls_result | grep -v $ft_ls_result > $ft_ls_result 2>&1
  else
    unbuffer $ls -$opt_list $test_dir | expand -t 8 > $ls_result 2>&1
    $ft_ls -$opt_list $test_dir | sed 's/\x1B\[[0-9;]*[a-zA-Z]//g' > $ft_ls_result 2>&1
  fi

  if ! diff -q $ls_result $ft_ls_result > /dev/null; then
    echo -e "  [ $opt_list ] : $RED[x]$WHITE"
  fi

  ((counter++))
done

# echo "Testing Two Option:"
# for opt in $(echo $options | fold -w1); do
#   for second_opt in $(echo $options | fold -w1); do
#
#     if is_exception $opt || is_exception $second_opt ; then
#       unbuffer $ls -$opt$second_opt $test_dir | expand -t 8 | grep -v $ls_result | grep -v $ft_ls_result > $ls_result 2>&1
#       $ft_ls -$opt$second_opt $test_dir | sed 's/\x1B\[[0-9;]*[a-zA-Z]//g' | grep -v $ls_result | grep -v $ft_ls_result > $ft_ls_result 2>&1
#     else
#       unbuffer $ls -$opt$second_opt $test_dir | expand -t 8 > $ls_result 2>&1
#       $ft_ls -$opt$second_opt $test_dir | sed 's/\x1B\[[0-9;]*[a-zA-Z]//g' > $ft_ls_result 2>&1
#     fi
#
#     if ! diff -q $ls_result $ft_ls_result > /dev/null; then
#       echo -e "  [ $opt$second_opt ] : $RED[x]$WHITE"
#     fi
#   done
# done

rm -rf $ls_result $ft_ls_result
