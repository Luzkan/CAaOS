#!/usr/bin/env bash

for file in *; 
do
  if [[ "$file" != "${file,,}" ]]; 
	then
   		 mv -b -- "$file" "${file,,}"
  	fi
done
