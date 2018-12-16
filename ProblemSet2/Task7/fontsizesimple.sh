ls | while read bigLetters;
	do smallLetters=`echo "${bigLetters}" | tr '[:upper:]' '[:lower:]'`;
	mv "$bigLetters" "$smallLetters";
done
