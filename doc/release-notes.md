# ION Core release notes

- [ION Core release notes](#ion-core-release-notes)
  - [ION Core version 5.0.99 is now available](#ion-core-version-5099-is-now-available)
    - [Mandatory Update](#mandatory-update)
    - [How to Upgrade](#how-to-upgrade)
    - [Compatibility](#compatibility)
    - [Noteable Changes](#noteable-changes)
      - [Migrate Travis as pipeline](#migrate-travis-as-pipeline)
      - [Zerocoin](#zerocoin)
      - [Masternodes](#masternodes)
      - [Token implementation](#token-implementation)
    - [New RPC Commands](#new-rpc-commands)
      - [Masternodes](#masternodes-1)
    - [Deprecated RPC Commands](#deprecated-rpc-commands)
      - [Masternodes](#masternodes-2)
    - [5.0.99 Change log](#5099-change-log)

## ION Core version 5.0.99 is now available  

Download at: https://bitbucket.org/ioncoin/ion/downloads/

This is a new major version release, including a new base code, various bug fixes, performance improvements, upgrades to the Atomic Token Protocol (ATP), as well as updated translations.

Please report bugs using the issue tracker at github: https://bitbucket.org/ioncoin/ion/issues?status=new&status=open

### Mandatory Update

ION Core v5.0.99 is an experimental release, intended only for TESTNET. It is not a mandatory update. This release contains new consensus rules and improvements for TESTNET only that are not backwards compatible with older versions. Users will have a grace period of up to one week to update their testnet clients before enforcement of this update is enabled.

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
  - The ION Core project is now available throug the [ION Coin Bitbucket repository](https://bitbucket.org/ioncoin/ion/)
  - The ION Core repository at github will for now remain available, but will no longer be updated.
- Fee policy
  - The current release partially implements IIP0006 - the remainder of the IIP0006 implementation will follow in a subsequent update.
  - The new fee policy, proposed and adopted in IIP0006, is implemented. As a result, this client will only relay and mine transactions with
    a feerate of 0.01 ION per KB.
  - The fee calculation approach for token transactions has been made more accurate.
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
- ~~I2p support~~

#### Migrate Travis as pipeline

#### Zerocoin

- The core part of the zerocoin functionality has been ported to the new code base. 
  - This includes all functionality related to verification of zerocoin transactions.
  - This excludes functionality to create new zerocoin transactions.

#### Masternodes

Testnet masternodes requires using the `protx` command:
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

#### Masternodes

masternode "command" ...
masternodelist ( "mode" "filter" )

`masternode count`        - Get information about number of masternodes (DEPRECATED options: 'total', 'ps', 'enabled', 'qualify', 'all')
`masternode current`      - Print info on current masternode winner to be paid the next block (calculated locally)
`masternode outputs`      - Print masternode compatible outputs
`masternode status`       - Print masternode status information
`masternode list`         - Print list of all known masternodes (see masternodelist for more info)
`masternode winner`       - Print info on next masternode winner to vote for
`masternode winners`      - Print list of masternode winners

### Deprecated RPC Commands

#### Masternodes

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

### 5.0.99 Change log  
ckti <ckti@i2pmail.org> (1):

- `3ad7b4d` CircleCI is now being used for continuous integration
