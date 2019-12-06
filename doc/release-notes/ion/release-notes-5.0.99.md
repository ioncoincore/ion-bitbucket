## ION Core version 5.0.99 is now available  

Download at: https://github.com/ioncoincore/ion/releases

This is a new major version release, including various bug fixes, performance improvements, implementation of the Atomic Token Protocol (ATP), as well as updated translations.

Please report bugs using the issue tracker at github: https://github.com/ioncoincore/ion/issues

### Mandatory Update
___  

ION Core v5.0.99 is a mandatory update for all users. This release contains new consensus rules and improvements that are not backwards compatible with older versions. Users will have a grace period of up to two week to update their clients before enforcement of this update is enabled - a grace period that will end at block 1320000 the latest.

### How to Upgrade
___
If you are running an older version, shut it down. Wait until it has completely shut down (which might take a few minutes for older versions), then run the installer (on Windows) or just copy over /Applications/ION-Qt (on Mac) or iond/ion-qt (on Linux).

### Compatibility
ION Core is extensively tested on multiple operating systems using the Linux kernel, macOS 10.8+, and Windows Vista and later.

Microsoft ended support for Windows XP on April 8th, 2014, No attempt is made to prevent installing or running the software on Windows XP, you can still do so at your own risk but be aware that there are known instabilities and issues. Please do not report issues about Windows XP to the issue tracker.

ION Core should also work on most other Unix-like systems but is not frequently tested on them.

#### Mac OSX High Sierra  
Currently there are issues with the 4.x gitian release on MacOS version 10.13 (High Sierra), no reports of issues on older versions of MacOS.
### Atomic Token Protocol (ATP)
_____

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

### Noteable Changes
______
- core: switch core from pivx to ion
- updated dependencies
- QT 5.9.8
- expat 2.2.9
- libevent 2.1.11
- zeromq 4.3.2
- dbus 1.13.12
- miniupnpc 2.0.20180203
- native_ds_storke 1.1.2
- native_biplist 1.0.3
- native_mac_alias 2.0.7

##### Zerocoin
- Reimplement zerocoin to new source

##### Protocol change
- 

### New RPC Commands
__________

#### Tokens

`configuremanagementtoken "ticker" "name" decimalpos "description_url" description_hash ( confirm_send )  `  
`configuretoken "ticker" "name" ( decimalpos "description_url" description_hash ) ( confirm_send )  `  
`createtokenauthorities "groupid" "ionaddress" authoritylist  `  
`droptokenauthorities "groupid" "transactionid" outputnr [ authority1 ( authority2 ... ) ] `   
`getsubgroupid "groupid" "data"  `  
`gettokenbalance ( "groupid" )  `  
`listtokenauthorities "groupid"`    
`listtokenssinceblock "groupid" ( "blockhash" target-confirmations includeWatchonly ) `   
`listtokentransactions "groupid" ( count from includeWatchonly ) `   
`melttoken "groupid" quantity  `  
`minttoken "groupid" "ionaddress" quantity  `  
`sendtoken "groupid" "address" amount  `  
`tokeninfo [list, all, stats, groupid, ticker, name] ( "specifier " )  `  
`scantokens <action> ( <scanobjects> ) `

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


### Deprecated RPC Commands
___
#### Masternodes
`masternode count`  
`masternode current`  
`masternode debug`  
`masternode genkey`  
`masternode outputs`  
`masternode start`  
`masternode start-alias`  
`masternode start-<mode>`  
`masternode status`  
`masternode list`  
`masternode list-conf`  
`masternode winners`  


### 5.0.99 Change log