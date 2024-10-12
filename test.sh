#!/bin/sh

WHITE="\033[0;37m"
GREEN="\033[1;32m"
RED="\033[1;31m"

ls="/bin/ls"
ft_ls="./ft_ls"

test_dir="."

options=$($ft_ls --help | grep -oE '^ *-[a-zA-Z0-9]' | sort | uniq | tr -d '\n -')

result_file="result.txt"
ls_result="ls.txt"
ft_ls_result="ft_ls.txt"

is_exception() {
  exceptions="gl"
  
  for char in $(echo $exceptions | fold -w1); do
    if [ "$char" = "$1" ]; then
      return 0
    fi
  done
  
  return 1
}

for opt in $(echo $options | fold -w1); do
  echo -n "[ $opt ] : "
  
  if is_exception $opt ; then
    unbuffer $ls -$opt $test_dir | expand -t 8 | grep -v $ls_result | grep -v $ft_ls_result
    echo "========="
    unbuffer $ls -$opt $test_dir | expand -t 8 | grep -v $ls_result | grep -v $ft_ls_result > $ls_result 2>&1
    $ft_ls -$opt $test_dir | sed 's/\x1B\[[0-9;]*[a-zA-Z]//g' | grep -v $ls_result | grep -v $ft_ls_result > $ft_ls_result 2>&1
  else
    unbuffer $ls -$opt $test_dir | expand -t 8 > $ls_result 2>&1
    $ft_ls -$opt $test_dir | sed 's/\x1B\[[0-9;]*[a-zA-Z]//g' > $ft_ls_result 2>&1
  fi

  if diff -q $ls_result $ft_ls_result > /dev/null; then
    echo -e "$GREEN[+]$WHITE"
  else
    echo -e "$RED[x]$WHITE"
  fi
done
