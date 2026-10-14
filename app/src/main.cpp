#include <httplib.h>
#include <oneapi/tbb/task_arena.h>

// A wrapper enforcing cpp-httplib's TaskQueue interface using oneTBB
class TBBTaskQueue : public httplib::TaskQueue {
public:
    TBBTaskQueue() = default;
    virtual ~TBBTaskQueue() override = default;

    virtual bool enqueue(std::function<void()> fn) override {
        // Enqueue the request handler execution into the oneTBB implicit task arena
        oneapi::tbb::task_arena().enqueue(fn);
        return true;
    }

    virtual void shutdown() override {
        // oneTBB handles its own thread lifetime lifecycle
    }
};

int main() {
    // Inject the custom TBB worker queue into the server
    httplib::Server svr(std::make_shared<TBBTaskQueue>());

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Hello World from oneTBB threads!", "text/plain");
    });

    svr.listen("0.0.0.0", 8080);
}
