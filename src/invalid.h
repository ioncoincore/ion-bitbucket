// Copyright (c) 2020 The Ion Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ION_INVALID_H
#define ION_INVALID_H

#include <univalue/include/univalue.h>
#include "script/script.h"

namespace invalid_out
{
    extern std::set<CScript> setInvalidScripts;

    bool ContainsScript(const CScript& out);
    bool LoadScripts();
}

#endif //ION_INVALID_H
