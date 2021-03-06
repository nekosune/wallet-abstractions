// Copyright (c) 2018-2019 Daniel Krawisz
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef ABSTRACTIONS_WALLET_WALLET
#define ABSTRACTIONS_WALLET_WALLET

#include <abstractions/redeem/funds.hpp>
#include "outpoint.hpp"
#include "spendable.hpp"
#include "keys.hpp"

namespace abstractions::bitcoin {
    
    using fee_calculator = satoshi (*)(uint32 size, uint32 sigops);
    
    inline satoshi one_satoshi_per_byte(uint32 size, uint32 sigops) {
        return size;
    }
    
    enum spend_policy {unset = 0, all = 1, fifo = 2};
    
    struct change {
        pattern<pubkey> Pattern;
        secret Key;
        satoshi Fee;
        fee_calculator Calculator;
        
        // provide a means of creating a change output and 
        // ensure that the mining fee is the fee that is given. 
        change(pattern<pubkey> p, secret k, satoshi fee) : Pattern{p}, Key{k}, Fee{fee}, Calculator{nullptr} {}
        
        // provide a fee calculator instead. 
        change(pattern<pubkey> p, secret k  , fee_calculator c) : Pattern{p}, Key{k}, Fee{0}, Calculator{c} {}
        change() : Pattern{nullptr}, Key{}, Fee{0}, Calculator{nullptr} {}
        
        bool valid() const {
            return Pattern != nullptr && Key.valid();
        }
    };
    
    struct to_pattern final {
        satoshi Value;
        payable Pattern;
        pubkey Key;
        
        to_pattern(satoshi v, payable p, pubkey k) : Value{v}, Pattern{p}, Key{k} {}
    };
    
    struct to_address final {
        satoshi Value;
        address Address;
        
        to_address(satoshi v, address a) : Value{v}, Address{a} {}
    };
    
    using funds = redeem::funds<script, txid, secret, pubkey>;
    
    struct wallet {
        funds Funds;
        
        wallet() : Funds{} {}
        wallet(funds f) : Funds{f} {}
        wallet(spendable x) : Funds{x} {}
        
        bool valid() const {
            return Funds.Valid;
        }
        
        bool value() const {
            return Funds.Value;
        }
        
        struct spent;
        
        template <typename ... X> 
        spent spend(X ... payments) const;
        
    private:
        struct payment {
            queue<output> Payments;
            change Change;
            index ChangeIndex;
            spend_policy SpendPolicy;
            
            spent spend(funds) const;
            
            satoshi value() const;
            
            payment pay() const {
                return *this;
            }
            
            template <typename ... X>
            payment pay(output o, X ... rest) const {
                return payment{
                    Payments.append(o), 
                    Change, ChangeIndex, 
                    SpendPolicy}.pay(rest...);
            }
            
            template <typename ... X>
            payment pay(change c, X ... rest) const {
                if (Change.valid()) throw std::logic_error{"only one change output allowed"};
                return payment{
                    Payments.append(output{0, c.Pattern->pay(c.Key)}), c, 
                    Payments.size(), SpendPolicy}.pay(rest...);
            }
            
            template <typename ... X>
            payment pay(spend_policy p, X ... rest) const {
                if (SpendPolicy != unset) throw std::logic_error{"only one policy output allowed"};
                return payment{Payments, Change, ChangeIndex, p}.pay(rest...);
            }
            
            template <typename ... X>
            payment pay(to_address a, X ... rest) const {
                return pay(output{a.Value, abstractions::script::compile(abstractions::script::pay_to(a.Address))}, rest...);
            }
            
            template <typename ... X>
            payment pay(to_pattern p, X ... rest) const {
                return pay(output{p.Value, p.Pattern->pay(p.Key)}, rest...);
            }
        };
    };
    
    struct wallet::spent {
        transaction Transaction;
        wallet Remainder;
        
        bool valid() const {
            return Transaction.valid() && Remainder.valid();
        }
        
        spent(transaction t, wallet w) : Transaction{t}, Remainder{w} {}
        friend struct wallet;
    private :
        spent() : Transaction{}, Remainder{} {}
    };
        
    template <typename ... X> 
    inline wallet::spent wallet::spend(X ... payments) const {
        return payment{}.pay(payments...).spend(Funds);
    }

}

#endif
