#pragma once

#include <flow/Service.h>

namespace flow {
    class IHostService : public virtual IService {
    public:
        static constexpr InterfaceVersion version = InterfaceVersion{1, 0, 0};

        ~IHostService() override = default;

        virtual void setPriority(uint64_t priority) = 0;
        virtual uint64_t getPriority() = 0;
    };
}