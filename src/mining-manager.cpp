// Copyright (c) 2014-2020 The Ion Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "mining-manager.h"

#include "chainparams.h"
#include "init.h"
#include "miner.h"
#include "net.h"
#include "policy/policy.h"
#include "pos/blocksignature.h"
#include "pos/kernel.h"
#include "pos/rewards.h"
#include "pos/stakeinput.h"
#include "script/sign.h"
#include "script/tokengroup.h"
#include "tokens/tokengroupmanager.h"
#include "utilmoneystr.h"
#include "validation.h"
#include "wallet/wallet.h"

// fix windows build
#include <boost/thread.hpp>

std::shared_ptr<CMiningManager> miningManager;

//////////////////////////////////////////////////////////////////////////////
//
// Internal miner
//

//	
// ScanHash scans nonces looking for a hash with at least some zero bits.	
// The nonce is usually preserved between calls, but periodically or if the	
// nonce is 0xffff0000 or above, the block is rebuilt and nNonce starts over at	
// zero.	
//	

bool static ScanHash(const CBlockHeader *pblock, const arith_uint256& hashTarget, uint32_t& nNonce, uint256& phash)
{
    // HashX11 currently does not have an intermediary state
    // So we revert to doing a full hash calculation and check
    CBlockHeader block = *pblock;
    while (true) {
        nNonce++;
        block.nNonce = nNonce;
        phash = block.GetHash();

        if (UintToArith256(phash) <= hashTarget)
            return true;
        // If nothing found after trying for a while, return -1	
        if ((nNonce & 0xfff) == 0)	
            return false;	
    }
}

void static IONMiner(CWallet * const pwallet)
{
    LogPrintf("IONMiner started\n");
    RenameThread("ion-miner");

    if (!g_connman) {
        throw std::runtime_error("No connection manager");
    }

    std::shared_ptr<CReserveKey> coinbaseKey;
    CPubKey coinbasePubKey;
    {
        LOCK(pwallet->cs_wallet);
        if (!pwallet->GetKeyForMining(coinbaseKey, coinbasePubKey) || !coinbaseKey){
            throw std::runtime_error("No coinbase script available (mining requires a wallet and keys in the keypool)");
        }
    }


    const auto& params = Params().GetConsensus();
    const bool fRegtest = Params().NetworkIDString() == CBaseChainParams::REGTEST;
    bool fPosPowPhase;
    bool fPosPhase;
    int nHeight = 0;
    unsigned int nExtraNonce = 0;

    UniValue blockHashes(UniValue::VARR);
    try {
        while (true) {
            if (Params().MiningRequiresPeers()) {
                // Busy-wait for the network to come online so we don't waste time mining
                // on an obsolete chain. In regtest mode we expect to fly solo.
                do {
                    bool fHaveConnections = g_connman->GetNodeCount(CConnman::CONNECTIONS_ALL) > 0;
                    if (fHaveConnections)
                        break;
                    MilliSleep(5 * 1000);
                } while (true);
            }

            unsigned int nTransactionsUpdatedLast = mempool.GetTransactionsUpdated();
            CBlockIndex* pindexPrev;
            {   // Don't keep cs_main locked
                LOCK(cs_main);
                pindexPrev = chainActive.Tip();
                nHeight = chainActive.Height();
            }

            fPosPowPhase = nHeight + 1 >= params.POSPOWStartHeight;
            fPosPhase = (nHeight + 1 >= params.POSStartHeight && nHeight + 1 < params.POSPOWStartHeight);

            std::unique_ptr<CBlockTemplate> pblocktemplate = nullptr;
            if (fPosPhase) {
                // Do nothing and wait
                MilliSleep(60 * 1000);
                continue;
            } else {
                if (fPosPowPhase) {
                    LOCK2(cs_main, pwallet->cs_wallet);

                    if (!tokenGroupManager->ElectronTokensCreated()) {
                        LogPrintf("Error: Mining in hybrid mode, but the Electron token group is not yet created\n");
                        MilliSleep(60 * 1000);
                        continue;
                    }
                    CScript coinbase_script;
                    CBlockReward reward(nHeight + 1, 0, false, params);
                    CReward coinbaseReward = reward.GetCoinbaseReward();

                    CTxDestination dst = coinbasePubKey.GetID();

                    std::map<CTokenGroupID, CAmount>::const_iterator it = coinbaseReward.tokenAmounts.begin();
                    if (it == coinbaseReward.tokenAmounts.end()) {
                        coinbase_script = CScript();
                    } else {
                        coinbase_script = GetScriptForDestination(dst, it->first, it->second);
                    }

                    pblocktemplate = BlockAssembler(Params()).CreateNewBlock(coinbase_script);
                } else {
                    LOCK2(cs_main, pwallet->cs_wallet);

                    CScript coinbase_script;
                    coinbase_script = CScript() << ToByteVector(coinbasePubKey) << OP_CHECKSIG;

                    pblocktemplate = BlockAssembler(Params()).CreateNewBlock(coinbase_script);
                }
            }
            if (!pblocktemplate.get()) {
                LogPrintf("Couldn't create new block\n");
                MilliSleep(60 * 1000);
                continue;
            }
            CBlock *pblock = &pblocktemplate->block;
            miningManager->GetIncrementedExtraNonce(pblock, pindexPrev, nExtraNonce);

            //
            // Search
            //
            int64_t nStart = GetTime();

            bool fNegative;
            bool fOverflow;
            arith_uint256 hashTarget;
            hashTarget.SetCompact(pblock->nBits, &fNegative, &fOverflow);
            // Check range
            if (fNegative || hashTarget == 0 || fOverflow || hashTarget > UintToArith256(Params().GetConsensus().powLimit)) {
                LogPrintf("%s - Incorrect difficulty\n");
                MilliSleep(1000);
                break;
            }
            
            uint256 hash;
            uint32_t nNonce = 0;
            while (true) {
                if (ScanHash(pblock, hashTarget, nNonce, hash)) {
                    // Found a solution
                    pblock->nNonce = nNonce;
                    assert(hash == pblock->GetHash());

                    LogPrintf("IONMiner:\n");
                    LogPrintf("proof-of-work found  \n  hash: %s  \ntarget: %s\n", pblock->GetHash().GetHex(), pblock->nBits);
                    std::shared_ptr<const CBlock> shared_pblock = std::make_shared<const CBlock>(*pblock);
                    if (!ProcessNewBlock(Params(), shared_pblock, true, nullptr)) {
                        LogPrintf("ProcessNewBlock, block not accepted\n");
                        MilliSleep(500);
                        continue;
                    }
                    ++nHeight;
                    //mark script as important because it was used at least for one coinbase output if the script came from the wallet
                    coinbaseKey->KeepKey();

                    break;
                }
                // Check for stop or if block needs to be rebuilt	
                boost::this_thread::interruption_point();	

                if (nNonce >= 0xffff0000) {
                    nNonce = 0;
                    break;	
                }
                if (mempool.GetTransactionsUpdated() != nTransactionsUpdatedLast && GetTime() - nStart > 60)
                    break;
                {
                    LOCK(cs_main);
                    if (pindexPrev != chainActive.Tip())
                        break;
                }
                // Update nTime every few seconds
                if (UpdateTime(pblock, Params().GetConsensus(), pindexPrev) < 0)
                    break; // Recreate the block if the clock has run backwards,
                           // so that we can use the correct time.
                if (Params().GetConsensus().fPowAllowMinDifficultyBlocks)
                {
                    // Changing pblock->nTime can change work required on testnet:
                    hashTarget.SetCompact(pblock->nBits);
                }
                if (Params().MiningRequiresPeers() && g_connman->GetNodeCount(CConnman::CONNECTIONS_ALL) < 1)
                    break;
            }
        }
    }
    catch (const boost::thread_interrupted&)
    {
        LogPrintf("IONMiner terminated\n");
        throw;
    }
    catch (const std::runtime_error &e)
    {
        LogPrintf("IONMiner runtime error: %s\n", e.what());
        return;
    }
}

void CMiningManager::GetIncrementedExtraNonce(CBlock* pblock, const CBlockIndex* pindexPrev, unsigned int& nMinerExtraNonce) {
    {
        LOCK(cs);
        IncrementExtraNonce(pblock, pindexPrev, nExtraNonce);
        nMinerExtraNonce = nExtraNonce;
    }
}

CMiningManager::CMiningManager(const CChainParams& chainparams, CConnman* const connman, CWallet * const pwalletIn) :
        nExtraNonce(0),
        fEnableMining(false), fEnableIONMining(false), pwallet(pwalletIn), chainParams(chainparams), g_connman(connman),
        nHashInterval(22) {}


bool CMiningManager::GenerateBitcoins(bool fGenerate, int nThreads)
{
    if (pwallet == nullptr) return false;

    LOCK2(cs_main, pwallet->cs_wallet);

    if (miningManager->fEnableMining) {
        static boost::thread_group* minerThreads = NULL;

        if (nThreads < 0)
            nThreads = GetNumCores();

        if (minerThreads != NULL)
        {
            minerThreads->interrupt_all();
            MilliSleep(600);
            delete minerThreads;
            minerThreads = NULL;
        }

        if (nThreads == 0 || !fGenerate)
            return false;

        minerThreads = new boost::thread_group();
        for (unsigned int i = 0; i < nThreads; i++)
            minerThreads->create_thread(boost::bind(&IONMiner, boost::ref(pwallet)));
    }

    return true;
}

void CMiningManager::UpdatedBlockTip(const CBlockIndex* pindex)
{
    LOCK(cs);

    tipIndex = pindex;
}
