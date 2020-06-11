// Copyright (c) 2020 The Ion Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MINING_CLIENT_H
#define MINING_CLIENT_H

#include "amount.h"
#include "script/script.h"
#include "sync.h"

#include <univalue.h>

class CBlock;
class CBlockIndex;
class CChainParams;
class CConnman;
class CMutableTransaction;
class CStakeInput;
class CMiningManager;
class CWallet;
class uint256;

extern std::shared_ptr<CMiningManager> miningManager;

class CMiningManager
{
public:
    CCriticalSection cs;

private:
    const CBlockIndex* tipIndex{nullptr};
    const CChainParams& chainParams;
    CConnman* g_connman = nullptr;
    CWallet* pwallet = nullptr;

    unsigned int nExtraNonce;
    const unsigned int nHashInterval;

public:
    CMiningManager(const CChainParams& chainparams, CConnman* const connman, CWallet * const pwalletIn = nullptr);

    bool fEnableMining;
    bool fEnableIONMining;

    bool GenerateBitcoins(bool fGenerate, int nThreads);
    void GetIncrementedExtraNonce(CBlock* pblock, const CBlockIndex* pindexPrev, unsigned int& nMinerExtraNonce);

    void UpdatedBlockTip(const CBlockIndex* pindex);

};

#endif // MINING_CLIENT_H
