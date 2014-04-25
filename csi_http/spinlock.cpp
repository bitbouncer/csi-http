#include "spinlock.h"

namespace csi
{
    static boost::detail::spinlock initializer = BOOST_DETAIL_SPINLOCK_INIT;
    spinlock::spinlock() : _sl(initializer) {}
}; // namespace
