#pragma once

#include <flow/optional_bundles/network_bundle/IHostService.h>
#include <flow/optional_bundles/logging_bundle/Logger.h>
#include <flow/optional_bundles/network_bundle/tcp/TcpConnectionService.h>
#include <thread>

namespace flow {
    class TcpHostService final : public IHostService, public Service {
    public:
        TcpHostService(DependencyRegister &reg, flowProperties props);
        ~TcpHostService() final = default;

        bool start() final;
        bool stop() final;

        void addDependencyInstance(ILogger *logger);
        void removeDependencyInstance(ILogger *logger);

        void setPriority(uint64_t priority) final;
        uint64_t getPriority() final;

    private:
        int _socket;
        int _bindFd;
        std::atomic<uint64_t> _priority;
        std::atomic<bool> _quit;
        std::thread _listenThread;
        ILogger *_logger{nullptr};
        std::vector<TcpConnectionService*> _connections;
    };
}