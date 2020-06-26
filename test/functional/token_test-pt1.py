#!/usr/bin/env python3
# Copyright (c) 2018-2020 The Ion Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test the functionality of all CLI commands.

"""
from test_framework.test_framework import BitcoinTestFramework

from test_framework.util import *

from time import sleep
from decimal import Decimal

import re
import sys
import os
import subprocess

ION_TX_FEE = 0.001
ION_AUTH_ADDR = "gAQQQjA4DCT2EZDVK6Jae4mFfB217V43Nt"

class TokenTest (BitcoinTestFramework):
    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 2
        #self.extra_args = [["-debug"],["-debug"]]

    def run_test(self):
        connect_nodes_bi(self.nodes, 0, 1)
        tmpdir=self.options.tmpdir
        self.log.info("Generating Tokens...")
        self.nodes[0].generate(100)
        self.nodes[0].importprivkey("cUnScAFQYLW8J8V9bWr57yj2AopudqTd266s6QuWGMMfMix3Hff4")
        self.nodes[0].generate(100)
        self.nodes[0].generate(99)
        self.nodes[0].sendtoaddress(ION_AUTH_ADDR, 10)
        self.nodes[0].generate(1)
        MagicTok=self.nodes[0].configuremanagementtoken("MAGIC", "MagicToken", "4", "https://github.com/ioncoincore/ATP-descriptions/blob/master/ION-testnet-MAGIC.json", "4f92d91db24bb0b8ca24a2ec86c4b012ccdc4b2e9d659c2079f5cc358413a765", "true")
        self.nodes[0].generate(1)
        MagicGroup_ID=MagicTok['groupID']
        mintaddr=self.nodes[0].getnewaddress()
        self.nodes[0].minttoken(MagicGroup_ID, mintaddr, 500)
        self.nodes[0].generate(1)
        XDMTok=self.nodes[0].configuremanagementtoken("XDM", "DarkMatter", "13", "https://github.com/ioncoincore/ATP-descriptions/blob/master/ION-testnet-XDM.json", "f5125a90bde180ef073ce1109376d977f5cbddb5582643c81424cc6cc842babd", "true")
        XDMGroup_ID=XDMTok['groupID']
        AtomTok=self.nodes[0].configuremanagementtoken("ATOM", "Atom", "0", "https://github.com/ioncoincore/ATP-descriptions/blob/master/ION-testnet-ATOM.json", "b0425ee4ba234099970c53c28288da749e2a1afc0f49856f4cab82b37f72f6a5", "true")
        AtomGroup_ID=AtomTok['groupID']
        ELECTok=self.nodes[0].configuremanagementtoken("ELEC", "Electron", "13", "https://github.com/ioncoincore/ATP-descriptions/blob/master/ION-testnet-ELEC.json", "6de2409add060ec4ef03d61c0966dc46508ed3498e202e9459e492a372ddccf5", "true")
        ELECGroup_ID=ELECTok['groupID']
        self.nodes[0].generate(1)
        self.log.info("Token Info %s" % json.dumps(self.nodes[0].tokeninfo("all"), indent=4))
        MagicAddr=self.nodes[0].getnewaddress()
        XDMAddr=self.nodes[0].getnewaddress()
        AtomAddr=self.nodes[0].getnewaddress()
        ELECAddr=self.nodes[0].getnewaddress()
        HulkAddr=self.nodes[0].getnewaddress()
        self.nodes[0].minttoken(MagicGroup_ID, MagicAddr, '4975')
        self.nodes[0].generate(1)
        self.nodes[0].minttoken(XDMGroup_ID, XDMAddr, '71')
        self.nodes[0].generate(1)
        self.nodes[0].minttoken(AtomGroup_ID, AtomAddr, '100')
        self.nodes[0].generate(1)
        self.nodes[0].minttoken(ELECGroup_ID, ELECAddr, '1')
        self.nodes[0].generate(1)
        HULKTok=self.nodes[0].configuretoken("HULK", "HulkToken", "10", "https://raw.githubusercontent.com/CeForce/hulktoken/master/hulk.json", "367750e31cb276f5218c013473449c9e6a4019fed603d045b51e25f5db29283a", "true")
        HulkGroup_ID=HULKTok['groupID']
        self.nodes[0].generate(1)
        self.nodes[0].minttoken(HulkGroup_ID, HulkAddr, '15')
        self.nodes[0].generate(1) 
        tokenBalance=self.nodes[0].gettokenbalance()
        for balance in tokenBalance:
            self.log.info("Token Name %s" % balance['name'])
            self.log.info("Token Balance %s" % balance['balance'])
        self.log.info("XDM Ticker %s" % json.dumps(self.nodes[0].tokeninfo('ticker', 'XDM'), indent=4))
        self.log.info("XDM Scan Tokens %s" % self.nodes[0].scantokens('start', XDMGroup_ID))
        tokenAuth=self.nodes[0].listtokenauthorities()
        for authority in tokenAuth:
            self.log.info("Ticker %s" % authority['ticker'])
            self.log.info("Authority address %s\n" % authority['address'])
            self.log.info("Token Authorities %s" % authority['tokenAuthorities'])
        self.log.info("Drop Mint Authoritiy for XDM")
        XDMDrop=self.nodes[0].listtokenauthorities(XDMGroup_ID)
        self.nodes[0].droptokenauthorities(XDMGroup_ID, XDMDrop[0]['txid'], str(XDMDrop[0]['vout']), 'configure')
        self.nodes[0].generate(1)
        tokenAuthority=(self.nodes[0].listtokenauthorities(XDMGroup_ID))
        tokenXDMAddr=tokenAuthority[0]['address']
        self.log.info("Token authorities XDM %s\n" % tokenXDMAddr)
        try:
            self.log.info("Try minting XDM tokens with mint flag removed")
            self.nodes[0].minttoken(XDMGroup_ID, XDMAddr, '100')
        except Exception as e:
            self.log.info(e)
        #self.log.info("Re-Enable mint XDM")
        #time.sleep(3600)
        #self.nodes[0].createtokenauthorities(XDMGroup_ID, tokenXDMAddr, 'configure')
        self.log.info("XDM Scan Tokens %s" % self.nodes[0].scantokens('start', XDMGroup_ID))
        tokenBalance=self.nodes[0].gettokenbalance()
        for balance in tokenBalance:
            self.log.info("Token Name %s" % balance['name'])
            self.log.info("Token Balance %s" % balance['balance'])
        AtomBalance=self.nodes[0].gettokenbalance(AtomGroup_ID)
        self.log.info("Atom Balance %s"  % AtomBalance['balance'])
        self.log.info("Melt 10 tokens from ATOM Group")
        self.nodes[0].melttoken(AtomGroup_ID, '10')
        AtomBalance=self.nodes[0].gettokenbalance(AtomGroup_ID)
        self.log.info("Atom Balance %s\n"  % AtomBalance['balance'])
        self.log.info("Token info all (from node1)\n%s\n" % json.dumps(self.nodes[1].tokeninfo('all'), indent=4))
        self.log.info("Token info ticker XDM\n%s\n" % json.dumps(self.nodes[0].tokeninfo('ticker', 'XDM'), indent=4))
        self.log.info("Token info name DarkMatter\n%s\n" % json.dumps(self.nodes[0].tokeninfo('name', 'darkmatter'), indent=4))
        self.log.info("Token info groupid %s\n%s\n" % (XDMGroup_ID, json.dumps(self.nodes[0].tokeninfo('groupid', XDMGroup_ID), indent=4)))
        ELEC_Trans=self.nodes[0].listtokentransactions(ELECGroup_ID)
        self.log.info("Token Transactions Electron Token\n%s\n" % ELEC_Trans)
        ElecTrans=ELEC_Trans[0]['txid']
        ELEC_BlockHash=self.nodes[0].getblockhash(200)
        self.log.info("Electron Transaction\n%s" % self.nodes[0].gettokentransaction(ElecTrans))
        self.log.info("Blockhash block 200 %s" % ELEC_BlockHash)
        self.log.info("\nTransaction ID %s" % ElecTrans)
        self.log.info("Transaction Details %s" % self.nodes[0].gettokentransaction(ElecTrans, ELEC_BlockHash))
        self.log.info("\nList tokens since block 200 Hulk\n%s" % self.nodes[0].listtokenssinceblock(ELECGroup_ID, ELEC_BlockHash))
        tokenHulkUnspent=self.nodes[0].listunspenttokens(HulkGroup_ID)
        newHulk=self.nodes[0].getnewaddress()
        self.log.info("Send tokens to new address %s" % self.nodes[0].sendtoken(HulkGroup_ID, newHulk, 2))
        self.nodes[0].generate(1)
        self.log.info(self.nodes[1].getaddressbalance)
        subgroupID=self.nodes[0].getsubgroupid(HulkGroup_ID,"Bruce_Banner")
        self.log.info("Subgroup Info %s " % self.nodes[0].tokeninfo('groupid',subgroupID))
        self.log.info("\nUnspent Tokens Hulk Token\n%s\n" % tokenHulkUnspent)
        tokenReceiveAddr=self.nodes[1].getnewaddress()
        rawTxid=tokenHulkUnspent[0]['txid']
        rawVout=tokenHulkUnspent[0]['vout']
        rawAddr=tokenReceiveAddr
        rawAmount=0.01
        self.log.info("txid %s" % rawTxid)
        self.log.info("vout %s" % rawVout)
        self.log.info("recaddr %s" % rawAddr)
        self.log.info("amount %s" % rawAmount )
        inputs=[{ "txid" : rawTxid, "vout" : rawVout }]
        outputs={ rawAddr : rawAmount }
        token={ rawAddr : { "amount" : 0.001, "groupid" : HulkGroup_ID, "token_amount" : 0.1 }}
        self.log.info(str(inputs))
        self.log.info(outputs)
        self.log.info(token)
        # ICC 86
        #rawtx=self.nodes[0].createrawtokentransaction(inputs, outputs, token)
        #self.log.info(rawtx)
        #time.sleep(3600)
if __name__ == '__main__':
    TokenTest().main()
