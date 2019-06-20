#!/usr/bin/env bash

rm -f *.class utils/*.class utils/results/*.class utils/results/options/*.class

while getopts 't' option
do
  case ${option} in
    t) rm -f *.csv ;;
  esac
done
