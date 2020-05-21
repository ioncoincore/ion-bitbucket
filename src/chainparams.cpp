// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2014-2019 The Dash Core developers
// Copyright (c) 2018-2020 The Ion Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include "arith_uint256.h"

#include <assert.h>

#include "chainparamsseeds.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTxTime, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.nTime = nTxTime;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 1486045800 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

static CBlock CreateDevNetGenesisBlock(const uint256 &prevBlockHash, const std::string& devNetName, uint32_t nTxTime, uint32_t nTime, uint32_t nNonce, uint32_t nBits, const CAmount& genesisReward)
{
    assert(!devNetName.empty());

    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.nTime = nTxTime;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    // put height (BIP34) and devnet name into coinbase
    txNew.vin[0].scriptSig = CScript() << 1 << std::vector<unsigned char>(devNetName.begin(), devNetName.end());
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = CScript() << OP_RETURN;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = 4;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock = prevBlockHash;
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=00000ffd590b14, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=e0028e, nTime=1390095618, nBits=1e0ffff0, nNonce=28917698, vtx=1)
 *   CTransaction(hash=e0028e, ver=1, vin.size=1, vout.size=1, nTime=0, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d01044c5957697265642030392f4a616e2f3230313420546865204772616e64204578706572696d656e7420476f6573204c6976653a204f76657273746f636b2e636f6d204973204e6f7720416363657074696e6720426974636f696e73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0xA9037BAC7050C479B121CF)
 *   vMerkleTree: e0028e
 */
static CBlock CreateGenesisBlock(uint32_t nTxTime, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "The Guardian: [2nd Feb 2017] Finsbury Park mosque wins apology and damages from Thomson Reuters";
    const CScript genesisOutputScript = CScript() << ParseHex("045622582bdfad9366cdff9652d35a562af17ea4e3462d32cd988b32919ba2ff4bc806485be5228185ad3f75445039b6e744819c4a63304277ca8d20c99a6acec8") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTxTime, nTime, nNonce, nBits, nVersion, genesisReward);
}


void CChainParams::UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout, int64_t nWindowSize, int64_t nThreshold)
{
    consensus.vDeployments[d].nStartTime = nStartTime;
    consensus.vDeployments[d].nTimeout = nTimeout;
    if (nWindowSize != -1) {
            consensus.vDeployments[d].nWindowSize = nWindowSize;
    }
    if (nThreshold != -1) {
            consensus.vDeployments[d].nThreshold = nThreshold;
    }
}

void CChainParams::UpdateDIP3Parameters(int nActivationHeight, int nEnforcementHeight)
{
    consensus.DIP0003Height = nActivationHeight;
    consensus.DIP0003EnforcementHeight = nEnforcementHeight;
}

void CChainParams::UpdateBudgetParameters(int nMasternodePaymentsStartBlock, int nBudgetPaymentsStartBlock, int nSuperblockStartBlock)
{
    consensus.nMasternodePaymentsStartBlock = nMasternodePaymentsStartBlock;
    consensus.nBudgetPaymentsStartBlock = nBudgetPaymentsStartBlock;
    consensus.nSuperblockStartBlock = nSuperblockStartBlock;
}

void CChainParams::UpdateSubsidyAndDiffParams(int nMinimumDifficultyBlocks, int nHighSubsidyBlocks, int nHighSubsidyFactor)
{
    consensus.nMinimumDifficultyBlocks = nMinimumDifficultyBlocks;
    consensus.nHighSubsidyBlocks = nHighSubsidyBlocks;
    consensus.nHighSubsidyFactor = nHighSubsidyFactor;
}

void CChainParams::UpdateLLMQChainLocks(Consensus::LLMQType llmqType) {
    consensus.llmqTypeChainLocks = llmqType;
}

static CBlock FindDevNetGenesisBlock(const Consensus::Params& params, const CBlock &prevBlock, const CAmount& reward)
{
    std::string devNetName = GetDevNetName();
    assert(!devNetName.empty());

    CBlock block = CreateDevNetGenesisBlock(prevBlock.GetHash(), devNetName.c_str(), prevBlock.nTime + 1, prevBlock.nTime + 2, 0, prevBlock.nBits, reward);

    arith_uint256 bnTarget;
    bnTarget.SetCompact(block.nBits);

    for (uint32_t nNonce = 0; nNonce < UINT32_MAX; nNonce++) {
        block.nNonce = nNonce;

        uint256 hash = block.GetHash();
        if (UintToArith256(hash) <= bnTarget)
            return block;
    }

    // This is very unlikely to happen as we start the devnet with a very low difficulty. In many cases even the first
    // iteration of the above loop will give a result already
    error("FindDevNetGenesisBlock: could not find devnet genesis block for %s", devNetName);
    assert(false);
}

// this one is for testing only
static Consensus::LLMQParams llmq5_60 = {
        .type = Consensus::LLMQ_5_60,
        .name = "llmq_5_60",
        .size = 5,
        .minSize = 3,
        .threshold = 3,

        .dkgInterval = 24, // one DKG per hour
        .dkgPhaseBlocks = 2,
        .dkgMiningWindowStart = 10, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 18,
        .dkgBadVotesThreshold = 8,

        .signingActiveQuorumCount = 2, // just a few ones to allow easier testing

        .keepOldConnections = 3,
};

static Consensus::LLMQParams llmq50_60 = {
        .type = Consensus::LLMQ_50_60,
        .name = "llmq_50_60",
        .size = 50,
        .minSize = 40,
        .threshold = 30,

        .dkgInterval = 24, // one DKG per hour
        .dkgPhaseBlocks = 2,
        .dkgMiningWindowStart = 10, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 18,
        .dkgBadVotesThreshold = 40,

        .signingActiveQuorumCount = 24, // a full day worth of LLMQs

        .keepOldConnections = 25,
};

static Consensus::LLMQParams llmq400_60 = {
        .type = Consensus::LLMQ_400_60,
        .name = "llmq_400_60",
        .size = 400,
        .minSize = 300,
        .threshold = 240,

        .dkgInterval = 24 * 12, // one DKG every 12 hours
        .dkgPhaseBlocks = 4,
        .dkgMiningWindowStart = 20, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 28,
        .dkgBadVotesThreshold = 300,

        .signingActiveQuorumCount = 4, // two days worth of LLMQs

        .keepOldConnections = 5,
};

// Used for deployment and min-proto-version signalling, so it needs a higher threshold
static Consensus::LLMQParams llmq400_85 = {
        .type = Consensus::LLMQ_400_85,
        .name = "llmq_400_85",
        .size = 400,
        .minSize = 350,
        .threshold = 340,

        .dkgInterval = 24 * 24, // one DKG every 24 hours
        .dkgPhaseBlocks = 4,
        .dkgMiningWindowStart = 20, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 48, // give it a larger mining window to make sure it is mined
        .dkgBadVotesThreshold = 300,

        .signingActiveQuorumCount = 4, // four days worth of LLMQs

        .keepOldConnections = 5,
};

libzerocoin::ZerocoinParams* CChainParams::Zerocoin_Params(bool useModulusV1) const
{
    assert(this);
    static CBigNum bnHexModulus = 0;
    if (!bnHexModulus)
        bnHexModulus.SetHex(consensus.zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParamsHex = libzerocoin::ZerocoinParams(bnHexModulus);
    static CBigNum bnDecModulus = 0;
    if (!bnDecModulus)
        bnDecModulus.SetDec(consensus.zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParamsDec = libzerocoin::ZerocoinParams(bnDecModulus);

    if (useModulusV1)
        return &ZCParamsHex;

    return &ZCParamsDec;
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */


class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 210240; // Note: actual number of blocks per calendar year with DGW v3 is ~200700 (for example 449750 - 249050)
        consensus.nMasternodePaymentsStartBlock = 100000; // not true, but it's ok as long as it's less then nMasternodePaymentsIncreaseBlock
        consensus.nMasternodePaymentsIncreaseBlock = 158000; // actual historical value
        consensus.nMasternodePaymentsIncreasePeriod = 576*30; // 17280 - actual historical value
        consensus.nInstantSendConfirmationsRequired = 6;
        consensus.nInstantSendKeepLock = 24;
        consensus.nBudgetPaymentsStartBlock = 99999999; // currently not activated
        consensus.nBudgetPaymentsCycleBlocks = 16616; // ~(60*24*30)/2.6, actual number of blocks per month is 200700 / 12 = 16725
        consensus.nBudgetPaymentsWindowBlocks = 100;
        consensus.nSuperblockStartBlock = 99999999; // The block at which 12.1 goes live (end of final 12.0 budget cycle)
        consensus.nSuperblockStartHash = uint256S("0000000000020cb27c7ef164d21003d5d20cdca2f54dd9a9ca6d45f4d47f8aa3");
        consensus.nSuperblockCycle = 16616; // ~(60*24*30)/2.6, actual number of blocks per month is 200700 / 12 = 16725
        consensus.nGovernanceMinQuorum = 10;
        consensus.nGovernanceFilterElements = 20000;
        consensus.nMasternodeMinimumConfirmations = 15;
        consensus.BIP34Height = 1; // Start enforcing BIP34 (Height in Coinbase) for blocks with version 2 and higher
        consensus.BIP34Hash = uint256S("000000ed2f68cd6c7935831cc1d473da7c6decdb87e8b5dba0afff0b00002690");
        consensus.BIP65Height = 1014023; // Start enforcing BIP65 (CHECKLOCKTIMEVERIFY) for blocks with version 9 and higher
        consensus.BIP66Height = 1; // Start enforcing BIP66 (Strict DER signatures) for blocks with version 7 and higher
        consensus.DIP0001Height = 9999999;
        consensus.DIP0003Height = 9999999;
        consensus.DIP0003EnforcementHeight = std::numeric_limits<int>::max();
        consensus.DIP0003EnforcementHash = uint256();
        consensus.IIP0006Height = 9999999;
        consensus.CSVHeight = 9999999;
        consensus.BIP147Height = 9999999;
        consensus.DIP0008Height = 9999999;
        consensus.powLimit = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 24
        consensus.hybridPowLimit = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 24
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Ion: 1 day
        consensus.nPowTargetSpacing = 1 * 60; // Ion: 1 minute
        consensus.nHybridPowTargetSpacing = 2 * consensus.nPowTargetSpacing; // target at 50% of blocks Pow
        consensus.nHybridPosTargetSpacing = 2 * consensus.nPowTargetSpacing; // target at 50% of blocks Pos
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nPowKGWHeight = 99999999;
        consensus.nPowDGWHeight = 99999999;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        // ION
        consensus.nCoinbaseMaturity = 60;
        // POS
        consensus.POSStartHeight = 1001;
        consensus.MidasStartHeight = 176500;
        consensus.DGWStartHeight = 550000;
        consensus.DGWDifficultyStartHeight = 550000;
        consensus.DGWStartTime = 1521851265;
        consensus.posLimit = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 24
        consensus.nPosTargetTimespanMidas = 7 * 24 * 60 * 60; // 1 week
        consensus.nPosTargetSpacingMidas = 64;
        consensus.nPosTargetSpacing = 1 * 60; // 1 minute
        consensus.nStakeMinDepth = 600;
        // Zerocoin
        consensus.nZerocoinRequiredStakeDepth = 200;
        consensus.nZerocoinStartHeight = 550001;
        consensus.nZerocoinStartTime = 1521851265; // GMT: Saturday, March 24, 2018 12:27:45 AM
        consensus.nBlockZerocoinV2 = 1012710;
        consensus.nPublicZCSpends = 1320000;
        consensus.nBlockStakeModifierV2 = 1320000;
        consensus.nFakeSerialBlockheightEnd = -1;
        consensus.nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        consensus.nRequiredAccumulation = 1;
        consensus.zerocoinModulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784"
            "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
            "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
            "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
            "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
            "31438167899885040445364023527381951378636564391212010397122822120720357";

        // ATP
        consensus.ATPStartHeight = 1320000; // Start enforcing the Atomic Token Protocol (ATP) for blocks with version 11 and higher
        consensus.strTokenManagementKey = "inqaYuaES1cmRBXHodp25UceeVPbWQG5wY";
        consensus.nOpGroupNewRequiredConfirmations = 1;
        // POSPOW
        consensus.POSPOWStartHeight = 9999999;

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 25;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000000ab5cc326ba5cc01516"); // 1571132

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x1599d484cfd57e3dc0ef8a2e8bc428c6b12ed8f8f73d6f5afd6c405c100f9a15"); // 1571132

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xc4;
        pchMessageStart[1] = 0xe1;
        pchMessageStart[2] = 0xd8;
        pchMessageStart[3] = 0xec;
        nDefaultPort = 12700;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1486045800, 1486045800, 28884498, 0x1e00ffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x0000004cf5ffbf2e31a9aa07c86298efb01a30b8911b80af7473d1114715084b"));
        assert(genesis.hashMerkleRoot == uint256S("0x7af2e961c5262cb0411edcb7414ab7178133fc06257ceb47d349e4e5e35e2d40"));

        vSeeds.emplace_back("main.seeder.baseserv.com", true);
        vSeeds.emplace_back("main.seeder.uksafedns.net", true);

        // Ion addresses start with 'i'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,103);
        // Ion script addresses start with '?'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,88);
        // Ion private keys start with '?' or '?'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,153);
        // Ion BIP32 pubkeys start with 'xpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        // Ion BIP32 prvkeys start with 'xprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp = "ion";

        // Ion BIP44 coin type is '5'
        nExtCoinType = 7567736;

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_50_60] = llmq50_60;
        consensus.llmqs[Consensus::LLMQ_400_60] = llmq400_60;
        consensus.llmqs[Consensus::LLMQ_400_85] = llmq400_85;
        consensus.llmqTypeChainLocks = Consensus::LLMQ_400_60;
        consensus.llmqTypeInstantSend = Consensus::LLMQ_50_60;

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fRequireRoutableExternalIP = true;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = false;
        fAllowMultiplePorts = false;

        nPoolMinParticipants = 3;
        nPoolMaxParticipants = 5;
        nFulfilledRequestExpireTime = 60*60; // fulfilled requests expire in 1 hour

        vSporkAddresses = {"io6GhJVECocnDCuj7gsEvz11XyuJtqnMwL"};
        nMinSporkKeys = 1;
        fBIP9CheckMasternodesUpgraded = true;

        checkpointData = (CCheckpointData) {
            {
                {1500, uint256S("0x407c3d91837740bd9a2fe6c5e33e02dbca5f5d8d40fb3e3e97ff9d6f95a6b3a7")},
                {4991, uint256S("0x4bb890b6c3843c46812f4fbe073ba2683fb56029bc568de04e21a8834ba3f007")},
                {9918, uint256S("0xb26723732cdb09e02aca0e6966502f95951d9b0321445ce953d64e06ac5e85fc")},
                {16912, uint256S("0xe8163463ac3dfa2580f9d489efd10f82fd35a65a0cb0a6b5e58171a04fee92db")},
                {23912, uint256S("0x8961ed6dd03005c757683856015e06d1af5ec3d40afc271cfaa818cff55cb6b9")},
                {35457, uint256S("0x0c4f0844008603f02f4ede5c21941adf74753def6a9aa5a973225f939ee77cdc")},
                {45479, uint256S("0xa9d47468683463dc9b6340fefc33cfb1adfe7bb8bd82f373d823298a1712c401")},
                {55895, uint256S("0x472ff2ffa3b20342e983c3fccbf1bf9c6198a4d3ce5bc24278a592de674442db")},
                {68899, uint256S("0x26470898eabf6f5c5726a15ff7e38465183eef127aea9008e01cb7fb04a89e4a")},
                {74619, uint256S("0x0e2e1be11ce19d63ec08cff6206ce5550d273e9a408d0899a25019ddc034ac50")},
                {75095, uint256S("0xb729df8182e6270456242a2afb148119c0bc27203832ab25b52b2543bcbf0036")},
                {88805, uint256S("0x90f7341ae56fff1d1294181f10fce53f849e14a987a513302b95fa206dd4ab07f")},
                {107996, uint256S("0x685db7512543f4426193b96b54c23798ec5766654c9f6300ae8af56d91c87f7b")},
                {137993, uint256S("0x270fb9f81a999a8b8cbae4fecf7291dd807118f8d9b8f4a9fc5b491f3bbb1f94")},
                {167996, uint256S("0x1c74f81b6d8012e6facf9c36543685588b06c408afb4670fa6fa345f52783d67")},
                {207992, uint256S("0x2d1218d7a848bb98d77333ce3f832a126140719ae594a7097e028fd4dfd619a1")},
                {312645, uint256S("0x2d1760e62a665b90772fc1c320d51be2c624f9c4bf20dc2f28bec2936522c0f5")},
                {407452, uint256S("0x9b3dc95d4d20c5a7b412674df622e19a06d051cc139f8d2e1248fd5afe1ee988")},
                {523412, uint256S("0xd7d3fa2be3f8d6e243653927831f8645d9aa387ef353acd83b6669978ed9f935")},
                {523930, uint256S("0x0a95a19070d2d93303caf2cdc8d324b51aeb28aa1f2c2def017e8147842597f0")},
                {750000, uint256S("0x1eaa2cc9f9fd33f5db56875bc865b099ce6d76d048791c2358f12ec5454c95e2")},
                {888900, uint256S("0x83c285e469fcb47494df14990b37bd997cf7ff8692e945f662683f7ccc1e5760")},
                {967800, uint256S("0xf714b07c1b25f0ec1c0217beec8e30e55410c6904e57e8e5969f679241e3e9fc")},
                {1067570, uint256S("0x965e2c19d9037f5859f6e8467464763c1e1e547b18282aa7ce014daa0f2750e4")},
                {1609911, uint256S("0xae74915570499671df74cc6e22672a734e2fc1e63e86285b45a3ba95ad3ca5d2")},
            }
        };

        chainTxData = ChainTxData{
            1586156579, // * UNIX timestamp of last known number of transactions (Block 1571132)
            3999193,    // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
            1.000001    // * estimated number of transactions per second after that timestamp
        };
    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 210240;
        consensus.nMasternodePaymentsStartBlock = 4010; // not true, but it's ok as long as it's less then nMasternodePaymentsIncreaseBlock
        consensus.nMasternodePaymentsIncreaseBlock = 4030;
        consensus.nMasternodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 2;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = 99999999;
        consensus.nBudgetPaymentsCycleBlocks = 50;
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nSuperblockStartBlock = 4200; // NOTE: Should satisfy nSuperblockStartBlock > nBudgetPeymentsStartBlock
        consensus.nSuperblockStartHash = uint256(); // do not check this on testnet
        consensus.nSuperblockCycle = 24; // Superblocks can be issued hourly on testnet
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 500;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256S("16ac2683f6ccab2f095a6270d485087e5c441f39a18fd32e00c7bfa996cdf696");
        consensus.BIP65Height = 1; // Start enforcing BIP65 (CHECKLOCKTIMEVERIFY) for blocks with version 9 and higher
        consensus.BIP66Height = 1; // Start enforcing BIP66 (Strict DER signatures) for blocks with version 7 and higher
        consensus.DIP0001Height = 117000;
        consensus.DIP0003Height = 117000;
        consensus.DIP0003EnforcementHeight = std::numeric_limits<int>::max();
//        consensus.DIP0003EnforcementHash = uint256S("059e5ee39302d06e3df836154db9818cffb1fcefd2733179b6aec86fb454b9c3");
        consensus.IIP0006Height = 117000;
        consensus.CSVHeight = 117000;
        consensus.BIP147Height = 117000;
        consensus.DIP0008Height = 117000;
        consensus.powLimit = uint256S("3fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 2
        consensus.hybridPowLimit = ArithToUint256(~arith_uint256(0) >> 12);
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Ion: 1 day
        consensus.nPowTargetSpacing = 1 * 60; // Ion: 1 minute
        consensus.nHybridPowTargetSpacing = 2 * consensus.nPowTargetSpacing; // target at 50% of blocks Pow
        consensus.nHybridPosTargetSpacing = 2 * consensus.nPowTargetSpacing; // target at 50% of blocks Pos
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nPowKGWHeight = 4002; // nPowKGWHeight >= nPowDGWHeight means "no KGW"
        consensus.nPowDGWHeight = 4002;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 25;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008
        // ION
        consensus.nCoinbaseMaturity = 15;
        // POS
        consensus.POSStartHeight = 203;
        consensus.MidasStartHeight = 0;
        consensus.DGWStartHeight = 300;
        consensus.DGWDifficultyStartHeight = 0;
        consensus.DGWStartTime = 1554332940;
        consensus.posLimit = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 24
        consensus.nPosTargetTimespanMidas = 7 * 24 * 60 * 60; // 1 week
        consensus.nPosTargetSpacingMidas = 64;
        consensus.nPosTargetSpacing = 1 * 60; // 1 minute
        consensus.nStakeMinDepth = 100;
        // Zerocoin
        consensus.nZerocoinRequiredStakeDepth = 200;
        consensus.nZerocoinStartHeight = 300;
        consensus.nZerocoinStartTime = 1501776000;
        consensus.nBlockZerocoinV2 = 1012710;
        consensus.nPublicZCSpends = 5500;
        consensus.nBlockStakeModifierV2 = 5530;
        consensus.nFakeSerialBlockheightEnd = -1;
        consensus.nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        consensus.nRequiredAccumulation = 1;
        consensus.zerocoinModulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784"
            "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
            "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
            "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
            "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
            "31438167899885040445364023527381951378636564391212010397122822120720357";

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 25;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // ATP
        consensus.ATPStartHeight = 5530; // Start enforcing the Atomic Token Protocol (ATP) for blocks with version 11 and higher
        consensus.strTokenManagementKey = "gBi3gDLnGfw8HA2rN4HmNxHk9hMC4GLFbh";
        consensus.nOpGroupNewRequiredConfirmations = 1;
        // POSPOW
        consensus.POSPOWStartHeight = 117000;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000000106cee2b2baf64cba"); // 119800

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x0000000005ae4db9746d6cad8e0ccebdef1e05afec9c40809f31457fdaf7d843"); // 95930

        pchMessageStart[0] = 0xdb;
        pchMessageStart[1] = 0x86;
        pchMessageStart[2] = 0xfc;
        pchMessageStart[3] = 0x69;
        nDefaultPort = 27170;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1486045800, 1491737471, 2263997, 0x1e0ffff0, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00000a5e695356de7ccae09478a4aa7053a402f7c2f57a40c44310d8fbe5d3c7"));

        vFixedSeeds.clear();
        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("testnet.seeder.baseserv.com", true);
        vSeeds.emplace_back("testnet.seeder.uksafedns.net", true);

        // Testnet Ion addresses start with 'y'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,97);
        // Testnet Ion script addresses start with '8' or '9'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        // Testnet Ion BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        // Testnet Ion BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x91};

        bech32_hrp = "tion";

        // Testnet Ion BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_50_60] = llmq50_60;
        consensus.llmqs[Consensus::LLMQ_400_60] = llmq400_60;
        consensus.llmqs[Consensus::LLMQ_400_85] = llmq400_85;
        consensus.llmqTypeChainLocks = Consensus::LLMQ_50_60;
        consensus.llmqTypeInstantSend = Consensus::LLMQ_50_60;

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fRequireRoutableExternalIP = true;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = false;
        fAllowMultiplePorts = true;

        nPoolMinParticipants = 3;
        nPoolMaxParticipants = 5;
        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        vSporkAddresses = {"gPp4w9xaDFjsiQ4KhTT5K9ATo8CVNhT7G2"};
        nMinSporkKeys = 1;
        fBIP9CheckMasternodesUpgraded = true;

        checkpointData = (CCheckpointData) {
            {
                {1, uint256S("0x16ac2683f6ccab2f095a6270d485087e5c441f39a18fd32e00c7bfa996cdf696")},
                {5530, uint256S("0xa7332c2034d501bb11f686ae6c224dbcbdf5332c2522204a63224a8ca670c18b")},
                {117000, uint256S("0x4c092ef6ad08622df9be8b5287ea29a1e9bfbe6756fd3056b9caebed61c697ca")},
                {119254, uint256S("0x65442ee4e4b0116ca7f85fdce633c21cc9e0fafdc7710505adfa21e30c291f84")},
                {119800, uint256S("0x445c9f1f85fc15cdadc04f5f9d58de3edfcc8419a0ec8a691e5b235f38a97bc7")},
                {127434, uint256S("0x00000105ad8f1b1981b9f710cb5377aefd79f143d992731cd20e5151aad3a06f")},
            }
        };

        chainTxData = ChainTxData{
            1586155781, // * UNIX timestamp of last known number of transactions (Block 119800)
            253924,     // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0.029999    // * estimated number of transactions per second after that timestamp (2 per minute)
        };

    }
};

/**
 * Devnet
 */
class CDevNetParams : public CChainParams {
public:
    CDevNetParams() {
        strNetworkID = "dev";
        consensus.nSubsidyHalvingInterval = 210240;
        consensus.nMasternodePaymentsStartBlock = 4010; // not true, but it's ok as long as it's less then nMasternodePaymentsIncreaseBlock
        consensus.nMasternodePaymentsIncreaseBlock = 4030;
        consensus.nMasternodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 2;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = 4100;
        consensus.nBudgetPaymentsCycleBlocks = 50;
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nSuperblockStartBlock = 4200; // NOTE: Should satisfy nSuperblockStartBlock > nBudgetPeymentsStartBlock
        consensus.nSuperblockStartHash = uint256(); // do not check this on devnet
        consensus.nSuperblockCycle = 24; // Superblocks can be issued hourly on devnet
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 500;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.BIP34Height = 1; // BIP34 activated immediately on devnet
        consensus.BIP65Height = 1; // BIP65 activated immediately on devnet
        consensus.BIP66Height = 1; // BIP66 activated immediately on devnet
        consensus.DIP0001Height = 2; // DIP0001 activated immediately on devnet
        consensus.DIP0003Height = 2; // DIP0003 activated immediately on devnet
        consensus.DIP0003EnforcementHeight = 2; // DIP0003 activated immediately on devnet
        consensus.DIP0003EnforcementHash = uint256();
        consensus.IIP0006Height = 1; // IIP0006 activated immediately on devnet
        consensus.CSVHeight = 2;
        consensus.BIP147Height = 2;
        consensus.DIP0008Height = 2;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.hybridPowLimit = ArithToUint256(~arith_uint256(0) >> 12);
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Ion: 1 day
        consensus.nPowTargetSpacing = 1 * 60; // Ion: 1 minute
        consensus.nHybridPowTargetSpacing = 2 * consensus.nPowTargetSpacing; // target at 50% of blocks Pow
        consensus.nHybridPosTargetSpacing = 2 * consensus.nPowTargetSpacing; // target at 50% of blocks Pos
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nPowKGWHeight = 4001; // nPowKGWHeight >= nPowDGWHeight means "no KGW"
        consensus.nPowDGWHeight = 4001;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 25;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008
        // ION
        consensus.nCoinbaseMaturity = 15;
        // POS
        consensus.POSStartHeight = 250;
        consensus.MidasStartHeight = 999999999;
        consensus.DGWStartHeight = 300;
        consensus.DGWDifficultyStartHeight = 300;
        consensus.DGWStartTime = 1554332940;
        consensus.posLimit = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 24
        consensus.nPosTargetTimespanMidas = 7 * 24 * 60 * 60; // 1 week
        consensus.nPosTargetSpacingMidas = 64;
        consensus.nPosTargetSpacing = 1 * 60; // 1 minute
        consensus.nStakeMinDepth = 100;
        // Zerocoin
        consensus.nZerocoinRequiredStakeDepth = 200;
        consensus.nZerocoinStartHeight = 300;
        consensus.nZerocoinStartTime = 1501776000;
        consensus.nBlockZerocoinV2 = 1012710;
        consensus.nPublicZCSpends = 5500;
        consensus.nBlockStakeModifierV2 = 5530;
        consensus.nFakeSerialBlockheightEnd = -1;
        consensus.nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        consensus.nRequiredAccumulation = 1;
        consensus.zerocoinModulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784"
            "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
            "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
            "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
            "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
            "31438167899885040445364023527381951378636564391212010397122822120720357";

        // ATP
        consensus.ATPStartHeight = 5530; // Start enforcing the Atomic Token Protocol (ATP) for blocks with version 11 and higher
        consensus.strTokenManagementKey = "gBi3gDLnGfw8HA2rN4HmNxHk9hMC4GLFbh";
        consensus.nOpGroupNewRequiredConfirmations = 1;
        // POSPOW
        consensus.POSPOWStartHeight = std::numeric_limits<int>::max();

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000000000000000000000");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x000000000000000000000000000000000000000000000000000000000000000");

        pchMessageStart[0] = 0xe2;
        pchMessageStart[1] = 0xca;
        pchMessageStart[2] = 0xff;
        pchMessageStart[3] = 0xce;
        nDefaultPort = 19799;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1486045800, 1491737471, 1096447, 0x207fffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();

        devnetGenesis = FindDevNetGenesisBlock(consensus, genesis, 1 * COIN);
        consensus.hashDevnetGenesisBlock = devnetGenesis.GetHash();

        vFixedSeeds.clear();
        vSeeds.clear();
        //vSeeds.push_back(CDNSSeedData("ionevo.org",  "devnet-seed.ionevo.org"));

        // Testnet Ion addresses start with 'y'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,97);
        // Testnet Ion script addresses start with '8' or '9'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        // Testnet Ion BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        // Testnet Ion BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x91};

        bech32_hrp = "iondn";

        // Testnet Ion BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_50_60] = llmq50_60;
        consensus.llmqs[Consensus::LLMQ_400_60] = llmq400_60;
        consensus.llmqs[Consensus::LLMQ_400_85] = llmq400_85;
        consensus.llmqTypeChainLocks = Consensus::LLMQ_50_60;
        consensus.llmqTypeInstantSend = Consensus::LLMQ_50_60;

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = true;
        fAllowMultiplePorts = true;

        nPoolMinParticipants = 3;
        nPoolMaxParticipants = 5;
        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        vSporkAddresses = {"g6JXj9btw63aVnVhJCYc8frJqPF4ZggAes"};
        nMinSporkKeys = 1;
        // devnets are started with no blocks and no MN, so we can't check for upgraded MN (as there are none)
        fBIP9CheckMasternodesUpgraded = false;

        checkpointData = (CCheckpointData) {
            {
                {0, genesis.GetHash()},
            }
        };

        chainTxData = ChainTxData{
            genesis.GetBlockTime(), // * UNIX timestamp of devnet genesis block
            1,                            // * we only have 2 coinbase transactions when a devnet is started up
            0.01                          // * estimated number of transactions per second
        };
    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMasternodePaymentsStartBlock = 240;
        consensus.nMasternodePaymentsIncreaseBlock = 350;
        consensus.nMasternodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 2;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = 1000;
        consensus.nBudgetPaymentsCycleBlocks = 50;
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nSuperblockStartBlock = 1500;
        consensus.nSuperblockStartHash = uint256(); // do not check this on regtest
        consensus.nSuperblockCycle = 10;
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 100;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.BIP34Height = 100000000; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1351; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.DIP0001Height = 2000;
        consensus.DIP0003Height = 432;
        consensus.DIP0003EnforcementHeight = 500;
        consensus.DIP0003EnforcementHash = uint256();
        consensus.IIP0006Height = 1;
        consensus.CSVHeight = 2;
        consensus.BIP147Height = 2;
        consensus.DIP0008Height = 2;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.hybridPowLimit = ArithToUint256(~arith_uint256(0) >> 12);
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Ion: 1 day
        consensus.nPowTargetSpacing = 1 * 60; // Ion: 1 minute
        consensus.nHybridPowTargetSpacing = 2 * consensus.nPowTargetSpacing; // target at 50% of blocks Pow
        consensus.nHybridPosTargetSpacing = 2 * consensus.nPowTargetSpacing; // target at 50% of blocks Pos
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nPowKGWHeight = 15200; // same as mainnet
        consensus.nPowDGWHeight = 34140; // same as mainnet
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        // ION
        consensus.nCoinbaseMaturity = 15;
        // POS
        consensus.POSStartHeight = 250;
        consensus.MidasStartHeight = 999999999;
        consensus.DGWStartHeight = 300;
        consensus.DGWDifficultyStartHeight = 300;
        consensus.DGWStartTime = 1521414629;
        consensus.posLimit = uint256S("000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 24
        consensus.nPosTargetTimespanMidas = 7 * 24 * 60 * 60; // 1 week
        consensus.nPosTargetSpacingMidas = 64;
        consensus.nPosTargetSpacing = 1 * 60; // 1 minute
        consensus.nStakeMinDepth = 1;
        // Zerocoin
        consensus.nZerocoinRequiredStakeDepth = 200;
        consensus.nZerocoinStartHeight = 350;
        consensus.nZerocoinStartTime = 1521414629;
        consensus.nBlockZerocoinV2 = 351;
        consensus.nPublicZCSpends = 5500;
        consensus.nBlockStakeModifierV2 = 1;
        consensus.nFakeSerialBlockheightEnd = -1;
        consensus.nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        consensus.nRequiredAccumulation = 1;
        consensus.zerocoinModulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784"
            "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
            "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
            "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
            "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
            "31438167899885040445364023527381951378636564391212010397122822120720357";
        // ATP
        consensus.ATPStartHeight = 300; // Start enforcing the Atomic Token Protocol (ATP) for blocks with version 11 and higher
        consensus.strTokenManagementKey = "gAQQQjA4DCT2EZDVK6Jae4mFfB217V43Nt";
        consensus.nOpGroupNewRequiredConfirmations = 1;
        // POSPOW
        consensus.POSPOWStartHeight = 350;

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 25;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xb5;
        pchMessageStart[1] = 0x9a;
        pchMessageStart[2] = 0x39;
        pchMessageStart[3] = 0x9e;
        nDefaultPort = 19994;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1575851148, 1575851149, 2263997, 0x207fffff, 1, 1 * COIN);

        arith_uint256 bnTarget;
        bnTarget.SetCompact(genesis.nBits);

        for (uint32_t nNonce = 0; nNonce < UINT32_MAX; nNonce++) {
            genesis.nNonce = nNonce;

            uint256 hash = genesis.GetHash();
            if (UintToArith256(hash) <= bnTarget) {
                break;
            }
        }
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("096c2a836db3e72323c8486865a266f422778852a38f62e3af9691b95ae28eab"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fRequireRoutableExternalIP = false;
        fMineBlocksOnDemand = true;
        fAllowMultipleAddressesFromGroup = true;
        fAllowMultiplePorts = true;

        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes
        nPoolMinParticipants = 3;
        nPoolMaxParticipants = 5;

        // privKey: 93QPD8M8SrVb4yL3E679sCGztzy1NRWYH3fs2wJQr2LMKnppFCJ
        vSporkAddresses = {"g6JXj9btw63aVnVhJCYc8frJqPF4ZggAes"};
        nMinSporkKeys = 1;
        // regtest usually has no masternodes in most tests, so don't check for upgraged MNs
        fBIP9CheckMasternodesUpgraded = false;

        checkpointData = (CCheckpointData) {
            {
                {0, uint256S("0x6cb21cdfc47afcd4a8fafcd024282a2ec0349b045b00234fb873f165ae11e91a")},
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        // Regtest Ion addresses start with 'y'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,97);
        // Regtest Ion script addresses start with '8' or '9'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        // Regtest private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        // Regtest Ion BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        // Regtest Ion BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x91};

        bech32_hrp = "ionrt";

        // Regtest Ion BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_5_60] = llmq5_60;
        consensus.llmqs[Consensus::LLMQ_50_60] = llmq50_60;
        consensus.llmqTypeChainLocks = Consensus::LLMQ_5_60;
        consensus.llmqTypeInstantSend = Consensus::LLMQ_5_60;
    }
};

static std::unique_ptr<CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::DEVNET) {
        return std::unique_ptr<CChainParams>(new CDevNetParams());
    } else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams());
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}

void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout, int64_t nWindowSize, int64_t nThreshold)
{
    globalChainParams->UpdateVersionBitsParameters(d, nStartTime, nTimeout, nWindowSize, nThreshold);
}

void UpdateDIP3Parameters(int nActivationHeight, int nEnforcementHeight)
{
    globalChainParams->UpdateDIP3Parameters(nActivationHeight, nEnforcementHeight);
}

void UpdateBudgetParameters(int nMasternodePaymentsStartBlock, int nBudgetPaymentsStartBlock, int nSuperblockStartBlock)
{
    globalChainParams->UpdateBudgetParameters(nMasternodePaymentsStartBlock, nBudgetPaymentsStartBlock, nSuperblockStartBlock);
}

void UpdateDevnetSubsidyAndDiffParams(int nMinimumDifficultyBlocks, int nHighSubsidyBlocks, int nHighSubsidyFactor)
{
    globalChainParams->UpdateSubsidyAndDiffParams(nMinimumDifficultyBlocks, nHighSubsidyBlocks, nHighSubsidyFactor);
}

void UpdateDevnetLLMQChainLocks(Consensus::LLMQType llmqType)
{
    globalChainParams->UpdateLLMQChainLocks(llmqType);
}
