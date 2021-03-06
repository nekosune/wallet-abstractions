// Copyright (c) 2018-2019 Daniel Krawisz
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef ABSTRACTIONS_TIMECHAIN_HEADER
#define ABSTRACTIONS_TIMECHAIN_HEADER

#include <abstractions/abstractions.hpp>
#include "timechain.hpp"

namespace abstractions::timechain::header {
    template <typename digest>
    const extern digest Genesis;

    template <typename Header, typename Z, typename Digest, typename N>
    struct interface {
        Z version(Header& h) const {
            return h.version();
        }
        
        Digest parent(Header& h) const {
            return h.parent();
        }
        
        Digest root(Header& h) const {
            return h.root();
        }
        
        N time(Header& h) const {
            return h.time();
        }
        
        N target(Header& h) const {
            return h.target();
        }
        
        N nonce(Header& h) const {
            return h.nonce();
        }
        
    };
    
    struct serialized {
        data::slice<byte, 80> Data;
        bool valid() const;
        int32_little version() const;
        digest_little parent() const;
        digest_little root() const;
        uint32_little time() const;
        uint32_little target() const;
        uint32_little nonce() const;
        constexpr static interface<serialized, int32_little, digest_little, uint32_little> is_header;
    };
    
} 

#endif
