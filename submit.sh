#!/bin/bash

echo -e "******************* DISCLAIMER *******************"
echo -e "THIS SCRIPT IS PROVIDED FOR STUENDT'S CONVENIENCE."
echo -e "IT DOES NOT GAURANTEE THAT APPROPRIATE SUBMISSION FILE WOULD BE CREATED."
echo -e "BEFORE SUBMISSION, YOU HAVE TO CHECK THE CONTENTS OF THE COMPRESSED FILE AGAIN."
echo -e "THE RESULT OF THIS SCRIPT IS STRICTLY AT YOUR OWN RISK.\n\n\n\n"

echo -e "Input Pintos Project Number. e.g. 1/2/3/4"
read -p "Project Number: " PRJ
if [ ${PRJ} -gt 4 ] || [ ${PRJ} -lt 1 ]
then
    echo -e "Wrong Project Number" 
    exit 1
fi
echo -e "Input Your ID. e.g. 20189999"
read -p "Your ID: " ID

echo -e "******** WARNING WARNING WARNING ********"
echo -e "******** WARNING WARNING WARNING ********"
echo -e "******** WARNING WARNING WARNING ********"
echo -e "Do you have '${ID}' directory?"
read -p "y/n: " ANSWER
if [ ${ANSWER} = "y" ] || [ ${ANSWER} = "Y" ]
then
    echo -e "This script will remove '${ID}' directory."
    echo -e "So you have to back up the '${ID}' directory."
    exit 1
elif [ ${ANSWER} = "n" ] || [ ${ANSWER} = "N" ]
then
    echo -e "Proceed to next step.\n"
else
    echo -e "Input y or n"
    exit 1
fi

echo -e "******** FOR YOUR INFORMATION ********"
echo -e "Have you put '${ID}.docx' in the current directory?"
read -p "y/n: " ANSWER
if [ ${ANSWER} = "y" ] || [ ${ANSWER} = "Y" ]
then
    echo -e "Proceed Compression.\n"
elif [ ${ANSWER} = "n" ] || [ ${ANSWER} = "N" ]
then
    echo -e "You need to submit '${ID}.docx' document file as well."
    echo -e "Please copy it to current directory (pintos/src/)."
    exit 1
else
    echo -e "Input y or n"
    exit 1
fi

make clean

mkdir -p ${ID}
cp -r ./src ${ID}
cp ${ID}.docx ${ID}
COPY=$(echo $?)
tar -zcf ./os_prj${PRJ}_${ID}.tar.gz ${ID}
rm -rf ${ID}

if [ ${COPY} -ne 0 ]
then
    echo -e "\n\n'${ID}.docx' is not compressed to 'tar.gz' file."
    echo -e "Please check that you've put the document file in the current directory."
else
    echo -e "\n\nCompress is completed. Please check 'tar.gz' file."
fi
