// #include "map_book.hpp"
// #include <fmt/format.h>
// #include <cstdint>


// namespace { // unnamed

//     // each std::map node needs extra memory for at least four pointers
//     // which we should account for when creating memory pool
//     constexpr std::uint32_t StdMapNodeExtra = sizeof(std::uintptr_t) * 4;

//     // allocate this many price levels in the memory pool
//     constexpr std::uint32_t NumPriceLevels = 50;

// } // namespace


// namespace itch {

//     map_book::map_book() noexcept
//             // : bids_()
//             // , asks_()
//             : bid_pool_(
//                     sizeof(std::pair<price_t const, price_level>) + StdMapNodeExtra, NumPriceLevels)
//             , bids_(bid_pool_)
//             , ask_pool_(sizeof(std::pair<price_t const, price_level>) + StdMapNodeExtra,
//                       NumPriceLevels)
//             , asks_(ask_pool_)
//     {
//         // empty
//     }

//     void
//     map_book::add_order(order& order) noexcept
//     {
//         // emplace functions may throw, in which case we abort
//         try {
//             if (order.side == Side::Bid) {
//                 if (bids_.empty()) {
//                     auto [itr, success] = bids_.emplace(order.price, order);
//                     // order.pl = itr;
//                 } else {
//                     auto o_itr = bids_.find(order.price);
//                     if (o_itr == bids_.end()) {
//                         auto [itr, success] = bids_.emplace(order.price, order);
//                         // order.pl = &itr->second;
//                     } else {
//                         // o_itr->second.qty += order.qty;
//                         // order.pl = &(o_itr->second);
//                     }
//                 }
//             } else if (order.side == Side::Ask) {
//                 if (asks_.empty()) {
//                     auto [itr, success] = asks_.emplace(order.price, order);
//                     // order.pl = &itr->second;
//                 } else {
//                     auto o_itr = asks_.find(order.price);
//                     if (o_itr == asks_.end()) {
//                         auto [itr, success] = asks_.emplace(order.price, order);
//                         // order.pl = &itr->second;
//                     } else {
//                         // o_itr->second.qty += order.qty;
//                         // order.pl = &(o_itr->second);
//                     }
//                 }
//             }

//             order.valid_pl = true;
//         } catch (std::exception const& e) {
//             std::fprintf(
//                     stderr, "[ERROR] exception thrown in %s: %s\n", __builtin_FUNCTION(), e.what());
//             std::abort();
//         }
//     }

//     void
//     map_book::delete_order(order& order) noexcept
//     {
//         DEBUG_ASSERT(order.valid_pl);
//         DEBUG_ASSERT(order.qty <= order.pl->agg_qty());
//         if (!order.valid_pl)
//             return;

//         // if order is the last on a price_level, delete the level
//         order.pl->delete_order(order);
//         if (order.pl->size() == 0) {
//             if (order.side == Side::Bid)
//                 bids_.erase(order.price);
//             else
//                 asks_.erase(order.price);
//         }

//         order.clear();
//     }

//     void
//     map_book::cancel_order(order& order, qty_t remove_qty) noexcept
//     {
//         DEBUG_ASSERT(remove_qty < order.qty);

//         if (remove_qty < order.qty) {
//             order.qty -= remove_qty;
//             order.pl->cancel_order(order, remove_qty);
//         } else {
//             // this cancel will remove the order (technically shouldn't
//             // happen)
//             delete_order(order);
//         }
//     }

//     void
//     map_book::replace_order(order& old_order, order& new_order) noexcept
//     {
//         DEBUG_ASSERT(old_order.valid_pl);
//         DEBUG_ASSERT(!new_order.valid_pl);

//         delete_order(old_order);
//         DEBUG_ASSERT(!old_order.valid_pl);

//         add_order(new_order);
//         DEBUG_ASSERT(new_order.valid_pl);
//     }

//     decltype(map_book::bids_) const&
//     map_book::bids() const noexcept
//     {
//         return bids_;
//     }

//     decltype(map_book::asks_) const&
//     map_book::asks() const noexcept
//     {
//         return asks_;
//     }

//     pq
//     map_book::best_bid() const noexcept
//     {
//         if (bids_.empty())
//             return {0, 0};

//         return {bids_.begin()->second.price(), bids_.begin()->second.agg_qty()};
//     }

//     pq
//     map_book::best_ask() const noexcept
//     {
//         if (asks_.empty())
//             return {0, 0};

//         return {asks_.begin()->second.price(), asks_.begin()->second.agg_qty()};
//     }

// } // namespace itch
