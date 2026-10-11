#include "AsyncResource.hpp"

using namespace Abyssgraphics;
using namespace Abyssutils::Memory;

IAsyncResource::IAsyncResource() : m_impl(makeUnique<SAsyncResourceImpl>()) {
    ;
}