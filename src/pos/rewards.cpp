// Copyright (c) 2020 The Ion Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pos/rewards.h"

#include "chainparams.h"
#include "tokens/tokengroupmanager.h"

CAmount GetBlockSubsidyION(const int nPrevHeight, const bool fPos, const Consensus::Params& consensusParams)
{
    CAmount nSubsidy = 0;
    int nHeight = nPrevHeight + 1;

    // TESTNET and REGTEST
    if (Params().NetworkIDString() == CBaseChainParams::REGTEST && nHeight < 86400) {
        if (nHeight == 0) {
            nSubsidy = 1 * COIN;
        } else if (nHeight < 200 && nHeight > 0) {
            nSubsidy = 250000 * COIN;
        } else if (nHeight < 86400 && nHeight >= 200) {
            nSubsidy = 250 * COIN;
        }
    } else if ((Params().NetworkIDString() == CBaseChainParams::TESTNET) && nHeight <= 570062) {
        if (nHeight == 0) {
            nSubsidy = 1 * COIN;
        } else if (nHeight == 1) {
            nSubsidy = 1 * COIN;
        } else if (nHeight == 2) {
            nSubsidy = 16400000 * COIN;
        } else if (nHeight >= 3 && nHeight <= 125146) {
            nSubsidy = 23 * COIN;
        } else if (nHeight > 125146 && nHeight <= 570062) {
            nSubsidy = 17 * COIN;
        }
    } else if ((Params().NetworkIDString() == CBaseChainParams::DEVNET) && nHeight <= 570062) {
        if (nHeight == 0) {
            nSubsidy = 1 * COIN;
        } else if (nHeight == 1) {
            nSubsidy = 1 * COIN;
        } else if (nHeight == 2) {
            nSubsidy = 16399999 * COIN;
        } else if (nHeight >= 2 && nHeight <= 125146) {
            nSubsidy = 23 * COIN;
        } else if (nHeight > 125146 && nHeight <= 570062) {
            nSubsidy = 17 * COIN;
        }
    } else {
        if (nHeight == 0) {
            nSubsidy = 1 * COIN;
        } else if (nHeight == 1) {
            nSubsidy = 16400000 * COIN;
        } else if (nHeight <= 125146) {
            nSubsidy = 23 * COIN;
        } else if (nHeight <= consensusParams.DGWStartHeight + 1) {
            nSubsidy = 17 * COIN;
        } else if (nHeight <= consensusParams.DGWStartHeight + 1 + 1440) {
            nSubsidy = 0.02 * COIN;
        } else if (nHeight <= 570062 + 1) { // 568622 + 1440 = 570062
            nSubsidy = 17 * COIN;
        } else if (nHeight <= 1013538 + 1) {    // 568622+1440=570062   1012098+1440=1013538
            nSubsidy = 11.5 * COIN;
        } else if (nHeight < consensusParams.POSPOWStartHeight) {    // phase 4-9
            nSubsidy = 5.75 * COIN;
        } else if (nHeight < consensusParams.POSPOWStartHeight + 7*24*60) { // See IIP0006
            nSubsidy = 0.5 * COIN;
        } else {
            nSubsidy = 0.5 * COIN;
        }
    }
    return nSubsidy;
}

CReward::CReward(const RewardType typeIn, const CAmount IONAmountIn, const CTokenGroupID group, const CAmount tokenAmount) : type(typeIn), IONAmount(IONAmountIn) {
    if (group != NoGroup && tokenAmount != 0) {
        tokenAmounts.insert(std::pair<CTokenGroupID, CAmount>(group, tokenAmount));
    }
};
CReward::CReward(const RewardType typeIn, const CTxOut& out) {
    type = typeIn;
    IONAmount = 0;

    CTokenGroupInfo tokenGrp(out.scriptPubKey);
    if ((tokenGrp.associatedGroup != NoGroup) && !tokenGrp.isAuthority()) {
        AddRewardAmounts(out.nValue, tokenGrp.associatedGroup, tokenGrp.getAmount());
    } else {
        AddRewardAmounts(out.nValue);
    }
}
CReward& CReward::operator+=(const CReward& b) {
    AddRewardAmounts(b.IONAmount);
    std::map<CTokenGroupID, CAmount>::const_iterator it = b.tokenAmounts.begin();
    while (it != b.tokenAmounts.end()) {
        AddRewardAmounts(0, it->first, it->second);
        it++;
    }
    return *this;
}

int CReward::CompareTo(const CReward& rhs) const {
    if (IONAmount == rhs.IONAmount && tokenAmounts == rhs.tokenAmounts) {
        return 0;
    } else if ((IONAmount > rhs.IONAmount || tokenAmounts > rhs.tokenAmounts)) {
        return 1;
    }
    return -1;
}

void CReward::AddRewardAmounts(const CAmount IONAmountIn, const CTokenGroupID group, const CAmount tokenAmount) {
    IONAmount += IONAmountIn;
    if (group != NoGroup && tokenAmount != 0) {
        tokenAmounts[group] += tokenAmount;
    }
}

CBlockReward::CBlockReward(const CBlock& block, const bool fHybridPowBlock, const CAmount coinstakeValueIn) {
    rewards.clear();
    fBurnUnpaidMasternodeReward = false;

    const int coinbaseVoutSize = block.vtx[0]->vout.size();
    if (coinbaseVoutSize > 0) {
        AddReward(CReward(CReward::REWARD_COINBASE, block.vtx[0]->vout[0]));
    }

    if (block.IsProofOfStake()) {
        const int coinstakeVoutSize = block.vtx[1]->vout.size();
        if (coinstakeVoutSize > 1) {
            AddReward(CReward(CReward::REWARD_COINSTAKE, block.vtx[1]->vout[1]));
            for (int i = 2; i < coinstakeVoutSize; i++) {
                AddReward(CReward(CReward::REWARD_MASTERNODE, block.vtx[1]->vout[i]));
            }
        }
    } else {
        if (coinbaseVoutSize <= 1) {
            fBurnUnpaidMasternodeReward = fHybridPowBlock;
        } else {
            for (int i = 1; i < coinbaseVoutSize; i++) {
                AddReward(CReward(CReward::REWARD_MASTERNODE, block.vtx[0]->vout[i]));
            }
        }
    }
/*
    for (auto out : block.vtx[0]->vout) {
        CReward reward(CReward::REWARD_COINBASE, out);
        AddReward(reward);
    }
    if (block.IsProofOfStake()) {
        for (auto out : block.vtx[1]->vout) {
            CReward reward(CReward::REWARD_COINSTAKE, out);
            AddReward(reward);
        }
    }
*/
}

CBlockReward::CBlockReward(const int nHeight, const CAmount nFees, const bool fPos, const Consensus::Params& consensusParams) {
    rewards.clear();
    fBurnUnpaidMasternodeReward = false;
    CAmount IONBlockValue = GetBlockSubsidyION(nHeight - 1, fPos, consensusParams);
    SetRewards(IONBlockValue, nFees, nHeight >= consensusParams.POSPOWStartHeight, fPos);
}

int CBlockReward::CompareTo(const CBlockReward& rhs) const {
    CReward lhsTotalReward(CReward::RewardType::REWARD_TOTAL);
    std::map<CReward::RewardType, CReward>::const_iterator it = rewards.begin();
    while (it != rewards.end()) {
        lhsTotalReward += it->second;
        it++;
    }

    CReward rhsTotalReward(CReward::RewardType::REWARD_TOTAL);
    it = rhs.rewards.begin();
    while (it != rhs.rewards.end()) {
        rhsTotalReward += it->second;
        it++;
    }

    if (lhsTotalReward < rhsTotalReward) {
        return -1;
    } else if (lhsTotalReward > rhsTotalReward) {
        return 1;
    }
    return 0;
}

CReward CBlockReward::GetTotalRewards() {
    CReward totalRewards(CReward::REWARD_TOTAL);
    totalRewards += GetCoinbaseReward();
    totalRewards += GetCoinstakeReward();
    totalRewards += GetMasternodeReward();
    return totalRewards;
}

bool CBlockReward::SetReward(CReward &reward, const CAmount IONAmount, const CTokenGroupID tokenID, const CAmount tokenAmount) {
    if (tokenID != NoGroup && tokenAmount != 0) {
        reward = CReward(reward.type, IONAmount, tokenID, tokenAmount);
        return true;
    } else {
        reward = CReward(reward.type, IONAmount);
        return false;
    }
}

void CBlockReward::AddReward(const CReward::RewardType rewardType, const CAmount IONAmount, const CTokenGroupID tokenID, const CAmount tokenAmount) {
    CReward additionalReward(rewardType, IONAmount, tokenID, tokenAmount);
    CReward reward = GetReward(rewardType);
    reward += additionalReward;
    rewards[rewardType] = reward;
}

void CBlockReward::AddReward(const CReward rewardIn) {
    auto r_it = rewards.find(rewardIn.type);
    if (r_it != rewards.end()) {
        r_it->second += rewardIn;
    } else {
        rewards.insert(std::pair<CReward::RewardType, CReward>(rewardIn.type, rewardIn));
    }
}

CReward CBlockReward::GetReward(CReward::RewardType rewardType) {
    auto r_it = rewards.find(rewardType);
    if (r_it != rewards.end()) {
        return r_it->second;
    } else {
        rewards[rewardType] = CReward(rewardType);
        return rewards[rewardType];
    }
}
CReward CBlockReward::GetCoinbaseReward() {
    return GetReward(CReward::REWARD_COINBASE);
}
CReward CBlockReward::GetCoinstakeReward() {
    return GetReward(CReward::REWARD_COINSTAKE);
}
CReward CBlockReward::GetMasternodeReward() {
    return GetReward(CReward::REWARD_MASTERNODE);
}

void CBlockReward::SetCoinbaseReward(const CAmount IONAmount, const CTokenGroupID tokenID, const CAmount tokenAmount) {
    CReward coinbaseReward(CReward::REWARD_COINBASE);
    SetReward(coinbaseReward, IONAmount, tokenID, tokenAmount);
    rewards[CReward::REWARD_COINBASE] = coinbaseReward;
}
void CBlockReward::SetCoinstakeReward(const CAmount IONAmount, const CTokenGroupID tokenID, const CAmount tokenAmount) {
    CReward coinstakeReward(CReward::REWARD_COINSTAKE);
    SetReward(coinstakeReward, IONAmount, tokenID, tokenAmount);
    rewards[CReward::REWARD_COINSTAKE] = coinstakeReward;
}
void CBlockReward::SetMasternodeReward(const CAmount IONAmount, const CTokenGroupID tokenID, const CAmount tokenAmount) {
    CReward masternodeReward(CReward::REWARD_MASTERNODE);
    SetReward(masternodeReward, IONAmount, tokenID, tokenAmount);
    rewards[CReward::REWARD_MASTERNODE] = masternodeReward;
}

void CBlockReward::MoveMasternodeRewardToCoinbase() {
    auto r_it = rewards.find(CReward::RewardType::REWARD_MASTERNODE);
    if (r_it != rewards.end()) {
        CReward masternodeReward(r_it->second);
        masternodeReward.type = CReward::RewardType::REWARD_COINBASE;
        AddReward(masternodeReward);
        rewards.erase(r_it);
    }
}

void CBlockReward::MoveMasternodeRewardToCoinstake() {
    auto r_it = rewards.find(CReward::RewardType::REWARD_MASTERNODE);
    if (r_it != rewards.end()) {
        CReward masternodeReward(r_it->second);
        masternodeReward.type = CReward::RewardType::REWARD_COINSTAKE;
        AddReward(masternodeReward);
        rewards.erase(r_it);
    }
}

void CBlockReward::SetHybridPOSRewards(const CAmount blockSubsidy) {
    SetMasternodeReward(blockSubsidy * 0.7);
    SetCoinstakeReward(blockSubsidy * 0.6);
}

void CBlockReward::SetHybridPOWRewards(const CAmount blockSubsidy) {
    if (tokenGroupManager->ElectronTokensCreated()) {
        SetCoinbaseReward(1, tokenGroupManager->GetElectronID(), blockSubsidy);
    } else {
        SetCoinbaseReward(0);
    }
    SetMasternodeReward(blockSubsidy * 0.7);
}

void CBlockReward::AddHybridFees(const CAmount nFees) {
    CAmount nMasternodeFees = nFees * 0.5;
    CAmount nCoinstakeFees = 0;
    AddReward(CReward::RewardType::REWARD_MASTERNODE, nMasternodeFees);

    auto r_it = rewards.find(CReward::RewardType::REWARD_COINSTAKE);
    if (r_it != rewards.end()) {
        nCoinstakeFees = nFees * 0.3;
        AddReward(CReward::RewardType::REWARD_COINSTAKE, nCoinstakeFees);
    }
    AddReward(CReward::RewardType::REWARD_BURN, nFees - nMasternodeFees - nCoinstakeFees);
}

void CBlockReward::SetHybridRewards(const CAmount blockSubsidy, const CAmount nFees, const bool fHybridPOS) {
    if (fHybridPOS) {
        SetHybridPOSRewards(blockSubsidy);
    } else {
        SetHybridPOWRewards(blockSubsidy);
    }
    AddHybridFees(nFees);
}

void CBlockReward::SetRewards(const CAmount blockSubsidy, const CAmount nFees, const bool fHybrid, const bool fPOS) {
    if (fHybrid) {
        if (fPOS) {
            SetHybridPOSRewards(blockSubsidy);
        } else {
            SetHybridPOWRewards(blockSubsidy);
            fBurnUnpaidMasternodeReward = true;
        }
        AddHybridFees(nFees);
    } else {
        SetMasternodeReward(blockSubsidy * 0.5);
        if (fPOS) {
            SetCoinstakeReward(blockSubsidy - GetMasternodeReward().IONAmount);
            AddReward(CReward::RewardType::REWARD_COINSTAKE, nFees);
        } else {
            SetCoinbaseReward(blockSubsidy - GetMasternodeReward().IONAmount);
            AddReward(CReward::RewardType::REWARD_COINBASE, nFees);
        }
    }
}