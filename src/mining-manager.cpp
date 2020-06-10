// Copyright (c) 2014-2020 The Ion Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "mining-manager.h"

#include "chainparams.h"
#include "init.h"
#include "masternode/masternode-sync.h"
#include "miner.h"
#include "net.h"
#include "policy/policy.h"
#include "pos/blocksignature.h"
#include "pos/kernel.h"
#include "pos/rewards.h"
#include "pos/stakeinput.h"
#include "script/sign.h"
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

void static IONMiner(CWallet * const pwallet)
{
    LogPrintf("IONMiner started\n");
    RenameThread("ion-miner");

    std::shared_ptr<CReserveKey> coinbaseKey;
    CPubKey coinbase_pubkey;
    if (!pwallet->GetKeyForMining(coinbaseKey, coinbase_pubkey) || !coinbaseKey){
        throw std::runtime_error("No coinbase script available (mining requires a wallet)");
    }

    const auto& params = Params().GetConsensus();
    const bool fRegtest = Params().NetworkIDString() == CBaseChainParams::REGTEST;
    static const int nInnerLoopCount = 0x10000;
    bool fPosPowPhase;
    bool fPosPhase;
    int nHeight = 0;

    {   // Don't keep cs_main locked
        LOCK(cs_main);
        nHeight = chainActive.Height();
    }
    unsigned int nExtraNonce = 0;
    UniValue blockHashes(UniValue::VARR);
    try {
        while (true) {
            if (Params().MiningRequiresPeers()) {
                // Busy-wait for the network to come online so we don't waste time mining
                // on an obsolete chain. In regtest mode we expect to fly solo.
                do {
                    bool fHaveConnections = !g_connman ? false : g_connman->GetNodeCount(CConnman::CONNECTIONS_ALL) > 0;
                    if (fHaveConnections && !IsInitialBlockDownload())
                        break;
                    MilliSleep(1000);
                } while (true);
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
                    if (!tokenGroupManager->ElectronTokensCreated()) {
                        LogPrintf("Error: Mining in hybrid mode, but the Electron token group is not yet created\n");
                        MilliSleep(60 * 1000);
                        continue;
                    }
                    std::shared_ptr<CReserveScript> coinbase_script;
                    CBlockReward reward(nHeight + 1, 0, false, params);
                    if (!pwallet->GetScriptForHybridMining(coinbase_script, coinbaseKey, reward.GetCoinbaseReward())) {
                        LogPrintf("Error: Keypool ran out, need to call keypoolrefill\n");
                        MilliSleep(60 * 1000);
                        continue;
                    }
                    pblocktemplate = BlockAssembler(Params()).CreateNewBlock(coinbase_script->reserveScript);
                } else {
                    std::shared_ptr<CReserveScript> coinbase_script;
                    if (!pwallet->GetScriptForPowMining(coinbase_script, coinbaseKey)) {
                        LogPrintf("Error: Keypool ran out, need to call keypoolrefill\n");
                        MilliSleep(60 * 1000);
                        continue;
                    }
                    pblocktemplate = BlockAssembler(Params()).CreateNewBlock(coinbase_script->reserveScript);
                }
            }
            if (!pblocktemplate.get()) {
                LogPrintf("Couldn't create new block\n");
                MilliSleep(60 * 1000);
                continue;
            }
            CBlock *pblock = &pblocktemplate->block;
            {
                LOCK(cs_main);
                IncrementExtraNonce(pblock, chainActive.Tip(), nExtraNonce);
            }
            while (pblock->nNonce < nInnerLoopCount && !CheckProofOfWork(pblock->GetHash(), pblock->nBits, Params().GetConsensus())) {
                boost::this_thread::interruption_point();
                ++pblock->nNonce;
            }
            if (pblock->nNonce == nInnerLoopCount) {
                continue;
            }

            std::shared_ptr<const CBlock> shared_pblock = std::make_shared<const CBlock>(*pblock);
            if (!ProcessNewBlock(Params(), shared_pblock, true, nullptr)) {
                LogPrintf("ProcessNewBlock, block not accepted\n");
                MilliSleep(1000);
                continue;
            }
            ++nHeight;
            LogPrintf("IONMiner:\n");
            LogPrintf("proof-of-work found  \n  hash: %s  \ntarget: %s\n", pblock->GetHash().GetHex(), pblock->nBits);

            //mark script as important because it was used at least for one coinbase output if the script came from the wallet
            coinbaseKey->KeepKey();
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

CMiningManager::CMiningManager(const CChainParams& chainparams, CConnman* const connman, CWallet * const pwalletIn) :
        nExtraNonce(0),
        fEnableMining(false), fEnableIONMining(false), pwallet(pwalletIn), chainParams(chainparams), g_connman(connman),
        nHashInterval(22) {}


bool CMiningManager::GenerateBitcoins(bool fGenerate, int nThreads)
{
    if (pwallet == nullptr) return false;

    LOCK2(cs_main, pwallet->cs_wallet);

    if (miningManager->fEnableMining) {
//        scheduler.scheduleEvery(boost::bind(&CMiningManager::DoMaintenance, boost::ref(miningManager), boost::ref(*g_connman)), 100);
        static boost::thread_group* minerThreads = NULL;

        if (nThreads < 0)
            nThreads = GetNumCores();

        if (minerThreads != NULL)
        {
            minerThreads->interrupt_all();
            delete minerThreads;
            minerThreads = NULL;
        }

        if (nThreads == 0 || !fGenerate)
            return false;

        minerThreads = new boost::thread_group();
        for (int i = 0; i < nThreads; i++)
            minerThreads->create_thread(boost::bind(&IONMiner, boost::ref(pwallet)));
    }

    return true;
}

void CMiningManager::UpdatedBlockTip(const CBlockIndex* pindex)
{
    LOCK(cs);

    tipIndex = pindex;
}
