// Copyright (c) 2019 Daniel Krawisz
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef ABSTRACTIONS_WALLET_SPENDABLE
#define ABSTRACTIONS_WALLET_SPENDABLE

#include <abstractions/redeem.hpp>
#include <abstractions/wallet/transaction.hpp>
#include <abstractions/wallet/address.hpp>
#include <abstractions/wallet/keys.hpp>
#include <abstractions/pattern/pay_to_address.hpp>
#include <abstractions/pattern/pay_to_pubkey.hpp>

namespace abstractions::bitcoin {

    using redeemer = abstractions::pattern::abstract::redeemer<const secret&, const script,
        const abstractions::transaction<input, output>&>;
    using pattern = abstractions::pattern::abstract::pattern<const secret&, const script,
        const abstractions::transaction<input, output>&>;
    
    using vertex = vertex<const secret&, const script, txid>;
    using spendable = vertex::spendable;
    
    const auto pay_to_address_compressed =
        abstractions::pattern::pay_to_address<secret,
            const bitcoin::pubkey, bitcoin::address,
            abstractions::transaction<input, output>&>{};
    const auto pay_to_address_uncompressed =
        abstractions::pattern::pay_to_address<secret,
            const bitcoin::uncompressed_pubkey, bitcoin::address, 
            const abstractions::transaction<input, output>&>{};
    const auto pay_to_pubkey_compressed =
        abstractions::pattern::pay_to_pubkey<secret,
            const bitcoin::pubkey,
            abstractions::transaction<input, output>&>{};
    const auto pay_to_pubkey_uncompressed = 
        abstractions::pattern::pay_to_pubkey<secret,
            const bitcoin::uncompressed_pubkey,
            const abstractions::transaction<input, output>&>{};
    
}

#endif 