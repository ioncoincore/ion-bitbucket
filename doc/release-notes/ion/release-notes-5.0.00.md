# ION Core release notes

- [ION Core release notes](#ion-core-release-notes)
  - [ION Core version 5.0.00 is now available](#ion-core-version-5000-is-now-available)
    - [Mandatory Update](#mandatory-update)
    - [How to Upgrade](#how-to-upgrade)
    - [Compatibility](#compatibility)
    - [Noteable Changes](#noteable-changes)
      - [Migrate Travis as pipeline](#migrate-travis-as-pipeline)
      - [Hybrid Proof-of-Work / Proof-of-Stake](#hybrid-proof-of-work--proof-of-stake)
      - [Zerocoin](#zerocoin)
      - [Masternodes](#masternodes)
      - [Token implementation](#token-implementation)
    - [New RPC Commands](#new-rpc-commands)
      - [Evo](#evo)
      - [Tokens](#tokens)
      - [Util](#util)
    - [Deprecated RPC Commands](#deprecated-rpc-commands)
      - [Masternodes](#masternodes-1)
    - [5.0.00 Change log](#5000-change-log)

## ION Core version 5.0.00 is now available

Download at: https://bitbucket.org/ioncoin/ion/downloads/

This is a new major version release, including a new base code, various bug fixes, performance improvements, upgrades to the Atomic Token Protocol (ATP), as well as updated translations.

Please report bugs using the issue tracker at github: https://bitbucket.org/ioncoin/ion/issues?status=new&status=open

### Mandatory Update

ION Core v5.0.00 is an experimental release, intended only for TESTNET. It is not a mandatory update. This release contains new consensus rules and improvements for TESTNET only that are not backwards compatible with older versions. Users will have a grace period of up to one week to update their testnet clients before enforcement of this update is enabled.

### How to Upgrade

- If you are running an older version, shut it down.
- Wait until it has completely shut down (which might take a few minutes for older versions).
- Copy the files in the testnet data folder to a backup medium, and delete all files in the testnet folder except ioncoin.conf and wallet.dat.
- Run the installer (on Windows) or just copy over /Applications/ION-Qt (on Mac) or iond/ion-qt (on Linux).

### Compatibility

ION Core is extensively tested on multiple operating systems using the Linux kernel, macOS 10.8+, and Windows Vista and later.

Microsoft ended support for Windows XP on April 8th, 2014, No attempt is made to prevent installing or running the software on Windows XP, you can still do so at your own risk but be aware that there are known instabilities and issues. Please do not report issues about Windows XP to the issue tracker.

ION Core should also work on most other Unix-like systems but is not frequently tested on them.

### Noteable Changes

- Move to bitbucket
  - The ION Core project is now available through the [ION Coin Bitbucket repository](https://bitbucket.org/ioncoin/ion/)
  - The ION Core repository at github will for now remain available, but will no longer be updated.
- Implementation of [IIP0006](https://github.com/ionomy/iips/blob/master/iip_0006.md)
  - Changes are adopted on mainnet at block <FORKBLOCK> and on testnet at block 155400.
  - Block rewards are reduced to 0.5 ION per block
    - 70% of block rewards are awarded to masternodes, 30% to stakers
  - Fee policy
    - The new fee policy, proposed and adopted in IIP0006, is implemented. As a result, this client will only relay and mine transactions with
      a fee rate of 0.01 ION per KB.
    - The fee calculation approach for token transactions has been made more accurate.
    - 50% of fees are awarded to masternodes, 30% to staking nodes and 20% is burned (50% on mining blocks).
- Switch from Proof-of-Stake to Hybrid Proof-of-Work/Proof-of-Stake
  - During the Hybrid phase, miners can generate POW blocks and staking nodes can generate POS blocks.
  - Both the POW difficulty algorithm and the POS difficulty algorithm aim at producing 1 block every 2 minutes.
    - Combined, the ION chain will keep producing 1 block every 1 minute.
  - Miners do not receive ION rewards. Instead, they receive rewards in the Electron token (ELEC).
  - Mining uses Dash' X11 algorithm.
  - The block version number includes a bit to specify if it is a POW or a POS block.
- Core: the core code has been rebased from PivX to Dash.
  - The staking functionality has been ported to the new code base.
  - The token implementation (ATP) has been ported to the new code base.
  - The zerocoin functionality (xION) has been partially ported to the new code base.
  - Fixes to regtest and testnet
    - A previously unsolved issue related to running regtest scripts no longer occurs in the new code base
    - The regtest mining and staking functionality allows generating POS, POW and Hybrid blocks
    - The new regtest genesis is inline with the updated source
    - Testnet is running the new code base, including deterministic masternodes
  - Updated dependencies
    - boost 1.70
    - QT 5.9.8
    - expat 2.2.9
    - libevent 2.1.11
    - [zeromq latest master](https://github.com/zeromq/libzmq/tree/eb54966cb9393bfd990849231ea7d10e34f6319e)
    - dbus 1.13.12
    - miniupnpc 2.0.20180203
    - native_ds_store 1.1.2
    - native_biplist 1.0.3
    - native_mac_alias 2.0.7
    - X11
      - libSM 1.2.3
      - libX11 1.6.9
      - libICE 1.0.10
      - libXau 1.0.9
      - libxcb 1.13
      - libXext 1.3.4
      - xcb_proto 1.13
      - xproto 7.0.31
      - xtrans 1.4.0
  - More supported architectures
    - mips
    - mipsel
    - s390x
    - powerppc64
    - powerppc64le
  - New gui and artworks
    - spinner
  - Updated and refactored build process
    - Gitian build script extended
      - can be used with latest debian or ubuntu
      - added upload to a server over SSH
      - added hashing of resulted archives
    - fix latest dependencies
- Developers tools
  - VSCode
    - added spellchecker exclusion list
    - build, debug and scripts for vscode debugger
- All sources spellchecked
- BLS
  - [BLS Signature Scheme](https://github.com/dashpay/dips/blob/master/dip-0006/bls_signature_scheme.md)
  - [BLS M-of-N Threshold Scheme and Distributed Key Generation](https://github.com/dashpay/dips/blob/master/dip-0006/bls_m-of-n_threshold_scheme_and_dkg.md#bls-m-of-n-threshold-scheme-and-distributed-key-generation)
- New version keeps all languages which our old sources had. For more info please check [translation process](translation_process.md).
- New hardcoded seeds

#### Migrate Travis as pipeline

- CircleCI is now being used for continuous integration

#### Hybrid Proof-of-Work / Proof-of-Stake

Combining POW with POS overcomes limitations in both. ION now supports mixed POW and POS blocks, where each algorithm
manages its own difficulty targets, but provide and use entropy from both type of blocks.

In POW blocks:
- Miners receive 0.5 ELEC per block
- Masternodes receive 70% of 0.5 ION = 0.35 ION rewards
- Masternodes receive 50% of the mined transaction fees
- 50% of the mined transaction fees are burned

In POS blocks:
- Stakers receive 2 x 30% of 0.5 ION = 0.30 ION rewards
- Masternodes receive 70% of 0.5 ION = 0.35 ION rewards
- Stakers receive 30% of the mined transaction fees
- Masternodes receive 50% of the mined transaction fees
- 20% of the mined transaction fees are burned

Since 50% of the blocks will be POS blocks, stakers will receive an average of 30% of the block rewards.

#### Zerocoin

- The core part of the zerocoin functionality has been ported to the new code base.
  - This includes all functionality related to verification of zerocoin transactions.
  - This excludes functionality to create new zerocoin transactions.

#### Masternodes

The full documentation on ION v5 masternodes can be found in the Dash Documentation section
on [Setting up Masternodes](https://docs.dash.org/en/stable/masternodes/setup.html).

An example of how this can be done using an ION controller wallet and an ION remote masternode
can be found at the ionomy wiki entry on
[Setting up ION coin Masternodes](https://github.com/ionomy/ion/wiki/HOW-TO:-Setup-ION-coin-Masternode-with-Ubuntu-remote-and-local-QT-wallet-(DASH-Rebase)).

- Testnet masternodes require using the `protx` command:
- Ensure a 20k collateral is available (`masternode outputs`).
- Generate the keys:
  - Generate the 'owner private key' (`getnewaddress`).
  - Generate the 'voting key address' (`getnewaddress`).
  - Generate the 'masternode payout address' (`getnewaddress`).
  - Optionally, generate the 'fee source address' (`getnewaddress`).
  - Generate the 'operator public and private keys' (`bls generate`).
- Configure the remote wallet:
  - Add `masternode=1` to the remote ioncoin.conf
  - Add `masternodeblsprivkey=<BLS_PRIVKEY>` to the remote ioncoin.conf (the private part of the 'operator public and private keys')
  - Add `externalip=<EXTERNAL_IP>` to the remote ioncoin.conf (the external IP address of the server hosting the remote masternode)
- Configure the controller wallet:
  - Run the following command in the debug console: `protx register_prepare <collateralHash> <collateralIndex> <ipAndPort> <ownerKeyAddr>
  <operatorPubKey> <votingKeyAddr> <operatorReward> <payoutAddress> (<feeSourceAddress>)`
  - The above command returns:
    - A binary transaction ("tx")
    - A collateral address ("collateralAddress")
    - A message to sign ("signMessage")
  - Run the following command in the debug console: `signmessage <collateralAddress> <signMessage>`
  - The above command returns a signature
  - Finally, submit the registration transaction with the following command: `protx register_submit <tx> <signature>`
- When the remote node is started, it will automatically activate.
- Run the sentinel on the remote server to let other masternodes know about yours
  - Edit `sentinel.conf` to set the network to 'testnet'.

#### Token implementation

- The Atomic Token Protocol (ATP) has been fully ported to the new code base.
  - Various improvements have been added to the verification code and the database code for tokens.

  **Introduction:**

  As part of the integration of game development functionality and blockchain technology, the ION community chose to adopt a token system as part of its blockchain core. The community approved proposal IIP 0002 was put to vote in July 2018, after which development started. Instead of developing a solution from scratch, the team assessed a number of proposals and implementations that were currently being worked on for other Bitcoin family coins. Selection criteria were:

  * Fully open, with active development
  * Emphasis on permissionless transactions
  * Efficient in terms of resource consumption
  * Simple and elegant underlying principles

  The ATP system implemented is based on the Group Tokenization proposal by Andrew Stone / BU.

  **References:**

  [GROUP Tokenization specification by Andrew Stone](https://docs.google.com/document/d/1X-yrqBJNj6oGPku49krZqTMGNNEWnUJBRFjX7fJXvTs/edit#heading=h.sn65kz74jmuf)
  [GROUP Tokenization reference implementation for Bitcoin Cash](https://github.com/gandrewstone/BitcoinUnlimited/commits/tokengroups)

  For the technical principles underlying ION Group Tokenization, the above documentation is used as our standard.

  ION developers fine tuned, extended and customized the Group Tokenization implementation. This documentation aims to support the ION community in:

  * Using the ION group token system
  * Creating additional tests as part of the development process
  * Finding new use cases that need development support

### New RPC Commands

A full list of RPC commands can be found in the [API Calls List](https://bitbucket.org/ioncoin/ion/wiki/API-Calls-List).

The most notable new commands relate to masternodes, voting and tokens.

####  Evo

Command|Description
---|---
bls "command" ...  |  Set of commands to execute BLS related actions. To get help on individual commands, use "help bls command".
protx "command" ...  |  Set of commands to execute ProTx related actions. To get help on individual commands, use "help protx command".
quorum "command" ...  |  Set of commands for quorums/LLMQs.To get help on individual commands, use "help quorum command".

masternode "command" ...  |  Set of commands to execute masternode related actions
masternodelist ( "mode" "filter" )  |  Get a list of masternodes in different modes. This call is identical to 'masternode list' call.
####  Tokens

Command|Description
---|---
configuremanagementtoken "ticker" "name" decimalpos "description_url" description_hash ( confirm_send )  |  Configures a new management token type. Currelty the only management tokens are MAGIC, XDM and ATOM.
configuretoken "ticker" "name" decimalpos "description_url" description_hash ( confirm_send )  |  Configures a new token type.
createrawtokentransaction [{"txid":"id","vout":n},...] {"address":amount,"data":"hex",...} ( locktime )  |  Create a transaction spending the given inputs and creating new outputs.
createtokenauthorities "groupid" "ionaddress" authoritylist  |  Creates new authorities and sends them to the specified address.
droptokenauthorities "groupid" "transactionid" outputnr [ authority1 ( authority2 ... ) ]  |  Drops a token group's authorities.
getsubgroupid "groupid" "data"  |  Translates a group and additional data into a subgroup identifier.
gettokenbalance ( "groupid" ) ( "address" )  |  If groupID is not specified, returns all tokens with a balance (including token authorities).
gettokentransaction "txid" ( "blockhash" )  |  Return the token transaction data.
listtokenauthorities ( "groupid" )  |  Lists the available token authorities.
listtokenssinceblock "groupid" ( "blockhash" target-confirmations includeWatchonly )  |  Get all transactions in blocks since block [blockhash], or all transactions if omitted
listtokentransactions "groupid" ( count from includeWatchonly )  |  Returns up to 'count' most recent transactions skipping the first 'from' transactions for account 'account'.
listunspenttokens ( minconf maxconf ["addresses",...] [include_unsafe] [query_options])  |  Returns array of unspent transaction outputs
melttoken "groupid" quantity  |  Melts the specified amount of tokens.
minttoken "groupid" "ionaddress" quantity  |  Mint new tokens.
scantokens \<action\> ( \<tokengroupid\> )  |  Scans the unspent transaction output set for possible entries that belong to a specified token group.
sendtoken "groupid" "address" amount ( "address" amount ) ( .. )  |  Sends token to a given address. Specify multiple addresses and amounts for multiple recipients.
tokeninfo [list, all, stats, groupid, ticker, name] ( "specifier " ) ( "extended_info" )  |  Returns information on all tokens configured on the blockchain.

####  Util

Command|Description
---|---
createmultisig nrequired ["key",...]  |  Creates a multi-signature address with n signature of m keys required.
verifymessage "address" "signature" "message"  |  Verify a signed message
getextendedbalance  |  Returns extended balance information
signmessage "address" "message"  |  Sign a message with the private key of an address

### Deprecated RPC Commands

#### Masternodes

Configuring a masternode is now done using the command `protx`.
The following commands are deprecated:

`createmasternodekey `
`getmasternodeoutputs `
`getmasternodecount`
`getmasternodeoutputs`
`getmasternodescores ( blocks )`
`getmasternodestatus`
`getmasternodewinners ( blocks "filter" )`
`startmasternode "local|all|many|missing|disabled|alias" lockwallet ( "alias" )`
`listmasternodeconf ( "filter" )`
`listmasternodes ( "filter" )`

### 5.0.00 Change log
ckti <ckti@i2pmail.org> (1):

- `3ad7b4d` CircleCI is now being used for continuous integration
- new version of QT solves the arm build issue where text could not be entered in the debug window
