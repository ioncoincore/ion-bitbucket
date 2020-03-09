// Copyright (c) 2020 The ION Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ION_REWARDS_H
#define ION_REWARDS_H

#include "amount.h"
#include "consensus/params.h"
#include "primitives/block.h"
#include "tokens/groups.h"

class CReward {
public:
    typedef enum RewardType_t {
        REWARD_COINBASE,
        REWARD_COINSTAKE,
        REWARD_MASTERNODE,

        REWARD_BURN,
        REWARD_TOTAL,
        REWARD_UNDEFINED
    } RewardType;

    RewardType type;
    CAmount IONAmount;
    std::map<CTokenGroupID, CAmount> tokenAmounts;

    CReward() : type(RewardType::REWARD_UNDEFINED), IONAmount(0) { };

    CReward(const RewardType typeIn) : type(typeIn), IONAmount(0) { };
    CReward(const RewardType typeIn, const CAmount IONAmountIn, const CTokenGroupID group = NoGroup, const CAmount tokenAmount = 0);
    CReward(const RewardType typeIn, const CTxOut& out);

    CReward& operator+=(const CReward& b);

    bool operator<(const CReward& rhs) const { return CompareTo(rhs) < 0; }
    bool operator>(const CReward& rhs) const { return CompareTo(rhs) > 0; }
    bool operator<=(const CReward& rhs) const { return CompareTo(rhs) <= 0; }
    bool operator>=(const CReward& rhs) const { return CompareTo(rhs) >= 0; }
    bool operator==(const CReward& rhs) const { return CompareTo(rhs) == 0; }
    bool operator!=(const CReward& rhs) const { return CompareTo(rhs) != 0; }

    int CompareTo(const CReward& rhs) const;

    void AddRewardAmounts(const CAmount IONAmountIn, const CTokenGroupID group = NoGroup, const CAmount tokenAmount = 0);
};

class CBlockReward {
private:
    std::map<CReward::RewardType, CReward> rewards;

    bool SetReward(CReward &reward, const CAmount IONAmount, const CTokenGroupID tokenID = NoGroup, const CAmount tokenAmount = 0);

    void SetHybridPOSRewards(const CAmount blockSubsidy);
    void SetHybridPOWRewards(const CAmount blockSubsidy);
    void SetHybridRewards(const CAmount blockSubsidy, const CAmount nFees, const bool fHybridPOS);

    void SetClassicPOSRewards(const CAmount blockSubsidy);
    void SetClassicPOWRewards(const CAmount blockSubsidy);
    void SetClassicRewards(const CAmount blockSubsidy, const CAmount nFees, const bool fClassicPOS);

public:
    bool fBurnUnpaidMasternodeReward;

    CBlockReward() {
        fBurnUnpaidMasternodeReward = false;
    };

    CBlockReward(const CBlock& block, const bool fHybridPowBlock, const CAmount coinstakeValueIn);
    CBlockReward(const int nHeight, const CAmount nFees, const bool fPos, const Consensus::Params& consensusParams);

    bool operator<(const CBlockReward& rhs) const { return CompareTo(rhs) < 0; }
    bool operator>(const CBlockReward& rhs) const { return CompareTo(rhs) > 0; }
    bool operator<=(const CBlockReward& rhs) const { return CompareTo(rhs) <= 0; }
    bool operator>=(const CBlockReward& rhs) const { return CompareTo(rhs) >= 0; }
    bool operator==(const CBlockReward& rhs) const { return CompareTo(rhs) == 0; }
    bool operator!=(const CBlockReward& rhs) const { return CompareTo(rhs) != 0; }

    int CompareTo(const CBlockReward& rhs) const;

    void AddReward(const CReward::RewardType rewardType, const CAmount IONAmount, const CTokenGroupID tokenID = NoGroup, const CAmount tokenAmount = 0);
    void AddReward(const CReward reward);

    CReward GetReward(CReward::RewardType rewardType);
    CReward GetCoinbaseReward();
    CReward GetCoinstakeReward();
    CReward GetMasternodeReward();

    void SetCoinbaseReward(const CAmount IONAmount, const CTokenGroupID tokenID = NoGroup, const CAmount tokenAmount = 0);
    void SetCoinstakeReward(const CAmount IONAmount, const CTokenGroupID tokenID = NoGroup, const CAmount tokenAmount = 0);
    void SetMasternodeReward(const CAmount IONAmount, const CTokenGroupID tokenID = NoGroup, const CAmount tokenAmount = 0);

    void MoveMasternodeRewardToCoinbase();
    void MoveMasternodeRewardToCoinstake();

    void AddHybridFees(const CAmount nFees);

    void SetRewards(const CAmount blockSubsidy, const CAmount nFees, const bool fHybrid, const bool fPOS);

    CReward GetTotalRewards();
};

CAmount GetBlockSubsidyION(const int nPrevHeight, const bool fPos, const Consensus::Params& consensusParams);

#endif //ION_REWARDS_H
