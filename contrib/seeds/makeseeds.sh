#!/bin/bash
# Copyright (c) 2018-2020 The Ion developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

# replace Ion chainparamsseeds.h
# Requirements:
#  - 1. RUN THIS SCRIPT FROM ION (Coins) ROOT FOLDER
#  - 2. File contrib/seeds/generate-seeds.py MUST BE PRESENT
#       Check requirements for usage of generate-seeds.py
#       for your operating system.
#
#  - 3. sudo apt-get install python3-dnspython

# How to use:
# Run from ion root folder:
#   ./contrib/seeds/makeseeds.sh
#
#   Note: if seeds are not update, unmark cleanup on bottom of this script
#         and check if every IP address has a port in nodes_main.txt and nodes_test.txt
#   thats it.

MAINSEEDS="95.217.78.168:12700
73.166.95.119:12700
95.216.230.101:12700
167.86.67.2:12700
164.68.120.4:12700 
207.148.84.210:12700
94.130.163.244:12700 
66.55.147.60:64081
45.32.80.95:12700
72.17.88.56:12700
149.29.2.18:12700
109.123.118.38:12700
155.138.225.26:12700
37.120.155.10:12700
88.202.231.139:12700"

MAINONIONSEEDS="mdbpyigvanhrv6jg.onion:12700
ndqgxcnvvd2s6pbr.onion:12700
3szeblrpwucr6eze.onion:12700
mdbpyigvanhrv6jg.onion:12700
g2x6xrzpef6fktez.onion:12700"

TESTNETSEEDS="72.17.88.56:27170
149.29.2.18:27170
109.123.118.38:27170
155.138.225.26:27170
37.120.155.10:27170
88.202.231.139:27170"

TESTNETONIONSEEDS="mdbpyigvanhrv6jg.onion:27170
ndqgxcnvvd2s6pbr.onion:27170
3szeblrpwucr6eze.onion:27170
mdbpyigvanhrv6jg.onion:27170
g2x6xrzpef6fktez.onion:27170"

setcustomseeds () {
cat <<EOF | tee -a $1
$2
EOF
}

getmainchainzmainseeds () {
    wget -O contrib/seeds/nodes_main.json https://chainz.cryptoid.info/ion/api.dws?q=nodes
    awk -F ':[ \t]*' '/^.*"nodes"/ {print $5 $9 $13 $17 $21 $25 $29 $33 $37 $41}' contrib/seeds/nodes_main.json >> contrib/seeds/nodes_main.txt
    rm -f contrib/seeds/nodes_main.json
    sed -i 's/,{\"subver\"/\\\n/g' contrib/seeds/nodes_main.txt
    sed -i 's/\[\"//g' contrib/seeds/nodes_main.txt
    sed -i 's/\",\"/:12700\\\n/g' contrib/seeds/nodes_main.txt
    sed -i 's/\"]}]//g' contrib/seeds/nodes_main.txt
    sed -i 's/\"]}/:12700/g' contrib/seeds/nodes_main.txt
    sed -i 's/\\//g' contrib/seeds/nodes_main.txt
}

updateseeds () {
    cd contrib/seeds
    python3 generate-seeds.py . > ../../src/chainparamsseeds.h
    rm -f ./nodes_main.txt
    rm -f ./nodes_test.txt
    cd ../..
    cat < ./src/chainparamsseeds.h
}

cleanup () {
    rm -f contrib/seeds/nodes_main.json
    rm -f contrib/seeds/nodes_main.txt
    rm -f contrib/seeds/nodes_test.txt
}

cleanup
setcustomseeds "./contrib/seeds/nodes_main.txt" "$MAINSEEDS"
setcustomseeds "./contrib/seeds/nodes_main.txt" "$MAINONIONSEEDS"
setcustomseeds "./contrib/seeds/nodes_test.txt" "$TESTNETSEEDS"
setcustomseeds "./contrib/seeds/nodes_test.txt" "$TESTNETONIONSEEDS"
getmainchainzmainseeds
sleep 2
cleanup
