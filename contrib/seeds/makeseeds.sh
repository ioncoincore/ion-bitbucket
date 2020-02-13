#!/bin/bash
# Copyright (c) 2018-2019 The Dash developers
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

MAINSEEDS="95.217.78.168:12700
73.166.95.119:12700
95.216.230.101:12700
167.86.67.2:12700
164.68.120.4:12700 
207.148.84.210:12700
94.130.163.244:12700 
66.55.147.60:64081
45.32.80.95:12700"

MAINONIONSEEDS="mdbpyigvanhrv6jg.onion:12700
heaoykp3ic2ykdb4.onion:12700
3szeblrpwucr6eze.onion:12700"

TESTNETSEEDS="38.32.80.2:27170
37.247.55.128:27170
149.29.2.18:27170
185.12.177.17:27170
37.120.155.10:27170
35.234.130.153:27170
64.156.195.229:27170
37.120.155.2
37.120.155.26
45.76.251.109:27170
149.28.13.221:27170
95.216.169.139:27170
76.20.70.162:27170
35.227.124.168:27170
50.254.113.130:27170
99.76.20.33:27170
13.211.229.87:27170
99.76.20.33:27170
99.76.20.33:27170
35.237.23.28:27170
99.76.20.33:27170
67.215.244.186:27170
34.73.86.245:27170
199.247.15.235:27170
149.248.59.142:27170
140.82.26.131:27170
45.63.89.41:27170
202.182.121.198:27170
149.28.13.221:27170
185.107.224.197:27170
45.77.215.23:27170
51.15.95.171:27170
104.238.167.129:27170
108.61.132.189:27170
73.115.9.124:27170
149.28.101.87:27170"

TESTNETONIONSEEDS="mdbpyigvanhrv6jg.onion:27170
heaoykp3ic2ykdb4.onion:27170
3szeblrpwucr6eze.onion:27170"

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
