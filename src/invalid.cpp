// Copyright (c) 2018 The ION Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "invalid.h"
#include "invalid_scripts.h"

#include "utilstrencodings.h"
#include "util.h"

namespace invalid_out
{
    std::set<CScript> setInvalidScripts;

    bool LoadScripts()
    {
        for (std::string i : setInvalidScriptStrings) {
            std::vector<unsigned char> vch = ParseHex(i);
            setInvalidScripts.insert(CScript(vch.begin(), vch.end()));
        }
        return true;
    }

    bool ContainsScript(const CScript& out)
    {
        return static_cast<bool>(setInvalidScripts.count(out));
    }
}

